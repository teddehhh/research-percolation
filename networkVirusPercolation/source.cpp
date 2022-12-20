#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <random>

using namespace std;

#pragma region Вспомогательные структуры
/// @brief Хранение параметров модели
struct config
{
public:
  int N;           // Размерность решетки
  int M;           // Количество проводимых экспериментов
  double q1 = 0.5; // Вероятность занятости соседнего узла
  double q2;       // Вероятность освобождения узла
  bool is_sphere;
  config(int N, int M, double q2, bool is_sphere)
  {
    this->N = N;
    this->M = M;
    this->q2 = q2;
    this->is_sphere = is_sphere;
  }
  config() {}
};
/// @brief Позиция в решетке
struct position
{
public:
  int row; // Номер строки
  int col; // Номер столбца
  position(int row, int col)
  {
    this->row = row;
    this->col = col;
  }
  position() {}

  bool operator==(const position &a)
  {
    return this->row == a.row && this->col == a.col;
  }
};
#pragma endregion

#pragma region Глобальные переменные
config dataConfig; // Анализируемая модель
#pragma endregion

/* Сигнатуры функций и процедур */
#pragma region Вспомогательные процедуры
/// @brief Запись и валидация входных данных, поступающих от пользователя
/// @return true - данные введены корректно, иначе - false
bool readVariables();
#pragma endregion
#pragma region Моделирование занятых и особожденных узлов
/// @brief Процедура моделирования распространения занятых узлов и их восстановления
/// @param p концентрация занятых узлов
/// @param q1 вероятность занятости соседнего узла
/// @param q2 вероятность освобождения занятого узла
/// @param grid решетка
void calculateSpread(double, double, double, vector<vector<int>> &);
/// @brief Процедура добавления соседей текущего узла
/// @param pos позиция текущего узла
/// @param applicants вектор соседей
/// @param grid решетка
void addNeighbors(position, vector<position> &, vector<vector<int>> &);
/// @brief Проверка узла на возможность его занять
/// @param node позиция узла в решетке
/// @param grid решетка
/// @return true - узел может быть занят, false - узел нельзя занять
bool isNodeCorrect(position, vector<vector<int>> &);
#pragma endregion
#pragma region Маркирование узлов и поиск стягивающего кластера
/// @brief Процедура маркирования кластеров в решетке
/// @param grid решетка
void createClusters(vector<vector<int>> &);
/// @brief Поиск паравильной метки в векторе кластерных меток
/// @param x рассматриваемая метка
/// @param labels вектор кластерных меток
/// @return номер правильной метки
int find(int, vector<int> &);
/// @brief Процедура объединения кластерных меток
/// @param x первая метка
/// @param y вторая метка
/// @param labels вектор кластерных меток
void unionLabels(int, int, vector<int> &);
/// @brief Проверка наличия стигивающего кластера
/// @param grid решетка
/// @return true - стягивающий кластер существует, false - отсутствует
bool searchConstrictionCluster(vector<vector<int>>);
#pragma endregion
#pragma region Случайные значения
/// @brief Получение коэффициента вероятности
/// @return число в диапазоне от 0 до 1
double randomProbability();
/// @brief Получение случайного числа в заданном диапазоне
/// @param min минимальное значение
/// @param max максимальное значение
/// @return число от min до max
int randNum(int, int);
#pragma endregion

/* Точка входа в программу */
int main(int argc, char **argv)
{
  /*
   * Чтение и проверка корректности введенных данных
   */
  if (!readVariables())
  {
    return 1;
  }

  srand((unsigned)time(NULL)); // установка генерации случайных чисел, относительно времени

  dataConfig.is_sphere = true;
  vector<vector<int>> grid;     // решетка
  size_t MCurrent = 0;          // текущий номер эксперимента
  int constrictionClusterCount; // счетчик стягивающих кластеров
  ofstream file("output.txt");  // файл для записи выходных данных

  /*
   * Вычисление вероятности возникновения стягивающего кластера в решетке
   * для разных значений концентрации
   */
  for (double p = 0; p < 1.001; p += 0.01)
  {
    /* Обнуление счетчика счягивающих кластеров */
    constrictionClusterCount = 0;
    /*
     * Проведение экспериментов с заданной концентрацией узлов
     */
    for (size_t i = 0; i < dataConfig.M; i++)
    {
      /*
       * Очистка решетки перед новым экспериментом
       */
      grid.clear();
      grid = vector<vector<int>>(dataConfig.N, vector<int>(dataConfig.N, 0));
      /*
       * Моделирование заполнения узлов
       */
      calculateSpread(p, dataConfig.q1, dataConfig.q2, grid);
      /*
       * Маркирование кластеров на решетке
       */
      createClusters(grid);
      /*
       * Поиск стягивающего кластера и увеличение счетчика найденных
       */
      if (searchConstrictionCluster(grid))
      {
        constrictionClusterCount++;
      }
    }
    /* Запись в файл вероятности возникновения стягивающего кластера */
    file << constrictionClusterCount / double(dataConfig.M) << endl;
  }
  /* Закрытие файла и выход из программы */
  file.close();
  return 0;
}

int randNum(int min, int max)
{
  if (max == 0)
  {
    return 0;
  }
  random_device r;
  mt19937 mersane(r());
  return min + mersane() % max;
}

bool readVariables()
{
  /*
   * на вход программе подаются аргументы в следующем порядке:
   * N - размерность квадратной решетки
   * M - число экспериментов
   * q2 - вероятность восстановления узла
   */
  errno = 0;
  char *p;
  // чтение параметров
  printf("Введите параметры\n");
  printf("Размерность решетки: ");
  scanf("%d", &dataConfig.N);
  printf("Количество экспериментов: ");
  scanf("%d", &dataConfig.M);
  printf("Вероятность восстановления узла: ");
  scanf("%lf", &dataConfig.q2);
  return true;
}

bool searchConstrictionCluster(vector<vector<int>> grid)
{
  if (dataConfig.is_sphere)
  {
    return false;
  }
  else
  {
    /*
     * Поиск пары с одинаковыми метками в верхней и нижней части решетки
     */
    for (size_t i = 0; i < grid[0].size(); i++)
    {
      if (grid[0][i] && find(grid[grid.size() - 1].begin(), grid[grid.size() - 1].end(), grid[0][i]) != grid[grid.size() - 1].end())
      {
        return true;
      }
    }
    return false;
  }
}

double randomProbability()
{
  return (double)rand() / RAND_MAX;
}

bool isNodeCorrect(position node, vector<vector<int>> &grid)
{
  int minWall = -1;          // минимальная граница решетки
  int maxWall = grid.size(); // максимальная граница решетки
  /* Узел не должен выходить за границы решетки и должен находиться только в свободном состоянии */
  return node.row != minWall && node.row != maxWall && node.col != minWall && node.col != maxWall &&
         grid[node.row][node.col] == 0;
}

void addNeighbors(position pos, vector<position> &applicants, vector<vector<int>> &grid)
{
  /* Добавление верхнего соседа */
  if (isNodeCorrect(position(pos.row - 1, pos.col), grid) &&
      find(applicants.begin(), applicants.end(), position(pos.row - 1, pos.col)) == applicants.end())
  {
    applicants.push_back(position(pos.row - 1, pos.col));
  }
  /* Добавление нижнего соседа */
  if (isNodeCorrect(position(pos.row + 1, pos.col), grid) &&
      find(applicants.begin(), applicants.end(), position(pos.row + 1, pos.col)) == applicants.end())
  {
    applicants.push_back(position(pos.row + 1, pos.col));
  }
  /* Добавление левого соседа */
  if (isNodeCorrect(position(pos.row, pos.col - 1), grid) &&
      find(applicants.begin(), applicants.end(), position(pos.row, pos.col - 1)) == applicants.end())
  {
    applicants.push_back(position(pos.row, pos.col - 1));
  }
  /* Добавление правого соседа */
  if (isNodeCorrect(position(pos.row, pos.col + 1), grid) &&
      find(applicants.begin(), applicants.end(), position(pos.row, pos.col + 1)) == applicants.end())
  {
    applicants.push_back(position(pos.row, pos.col + 1));
  }
}

void calculateSpread(double p, double q1, double q2, vector<vector<int>> &grid)
{
  vector<position> avaliableNodes; // вектор доступных для занятия узлов
  /* Инициализация вектора */
  for (size_t i = 0; i < grid.size(); i++)
  {
    for (size_t j = 0; j < grid.size(); j++)
    {
      avaliableNodes.push_back(position(i, j));
    }
  }

  int occupiedNodesCount = p * 100; // доля занятых узлов
  vector<position> neighbors;       // вектор соседей для занятия
  vector<position> infected;        // вектор занятых узлов
  position selectedNeighbor;        // выбранный сосед
  int randomNum;                    // случайное значение
  int prevNeighborsSize;            // предыдущий размер вектора соседей
  int bottomInfectedSize = 0;       // нижняя граница предыдущих занятых в векторе infected
  int topInfectedSize = 0;          // верхняя граница предыдущих занятых в векторе infected
  int i = 0;                        // счетчик занятых узлов
  /* Занимаем узлы, пока не достигнем нужной концентрации */
  while (i < occupiedNodesCount)
  {
    topInfectedSize = infected.size(); // получение верхнего порога infected на предыдущей итерации
    /* Восстановление узлов, занятых на предыдущей итерации */
    for (size_t j = bottomInfectedSize; j < topInfectedSize; j++)
    {
      if (randomProbability() < q2)
      {
        grid[infected[j].row][infected[j].col] = -1;
      }
    }
    /* Проверка на наличие соседей */
    if (!neighbors.size())
    {
      /*
       * Ветка условия, когда закончились соседи для распространения.
       * Происходит выбор случайного узла в решетке
       */
      randomNum = randNum(0, avaliableNodes.size() - 1);        // получение случайного узла из доступных
      selectedNeighbor = avaliableNodes[randomNum];             // полчение его позиции
      avaliableNodes.erase(avaliableNodes.begin() + randomNum); // удаление из вектора доступных узлов
      grid[selectedNeighbor.row][selectedNeighbor.col] = true;  // занятие узла в решетке
      infected.push_back(selectedNeighbor);                     // добавление занятого узла в вектор занятых
      addNeighbors(selectedNeighbor, neighbors, grid);          // получение его соседей
      i++;                                                      // увеличение счетчика
    }
    else
    {
      /*
       * Ветка условия, когда существуют соседи.
       * Происходит занятие среди соседей узлов, занятых на предыдущей итерации
       */
      prevNeighborsSize = neighbors.size(); // предыдущий размер соседей
      bottomInfectedSize = infected.size(); // нижний порог количества зараженных
      /* Занятие соседей с вероятностью q1 */
      for (size_t j = 0; j < prevNeighborsSize && i < occupiedNodesCount; j++)
      {
        if (randomProbability() < q1)
        {
          selectedNeighbor = neighbors[j]; // получение позиции занятого узла
          avaliableNodes.erase(remove(avaliableNodes.begin(),
                                      avaliableNodes.end(),
                                      selectedNeighbor));          // удаление узла из доступных
          grid[selectedNeighbor.row][selectedNeighbor.col] = true; // занятие узла в решетке
          infected.push_back(selectedNeighbor);                    // добавление узла в вектор занятых
          addNeighbors(selectedNeighbor, neighbors, grid);         // получение его соседей
          i++;                                                     // увеличение счетчика
        }
      }
      neighbors.erase(neighbors.begin(), neighbors.begin() + prevNeighborsSize); // удаление старых соседей
    }
  }
  /* Маркирование восстановленных узлов меткой 0 для правильного дальнейшего маркирования */
  for (auto &&row : grid)
  {
    for (auto &&elem : row)
    {
      if (elem == -1)
      {
        elem = 0;
      }
    }
  }
}

int find(int x, vector<int> &labels)
{
  int y = x;
  /* Поиск первой правильной метки */
  while (labels[y] != y)
    y = labels[y];
  while (labels[x] != x)
  {
    int z = labels[x];
    labels[x] = y;
    x = z;
  }
  return y;
}

void unionLabels(int x, int y, vector<int> &labels)
{
  labels[find(x, labels)] = find(y, labels); // присвоение метке x правильной кластерной метке
}

void createClusters(vector<vector<int>> &grid)
{
  /* Добавление в решетку нулевой строки и столбца в начало */
  grid.insert(grid.begin(), vector<int>(grid.size(), 0));
  for (auto &&i : grid)
  {
    i.insert(i.begin(), 0);
  }

  int largestCluster = 0;                      // номер для маркирования нового кластера
  int maxSize = grid.size() * grid.size() / 2; // максимальное количество кластеров в решетке
  vector<int> labels;                          // вектор меток для узлов

  /* Инициализация вектора правильными метками */
  for (size_t i = 0; i < maxSize; i++)
  {
    labels.push_back(i);
  }
  /* Маркирование узлов */
  for (size_t i = 1; i < grid.size(); i++)
  {
    for (size_t j = 1; j < grid.size(); j++)
    {
      if (grid[i][j])
      {
        int left = grid[i][j - 1];  // левый сосед
        int above = grid[i - 1][j]; // верхний сосед
        int label;
        /* Найден новый кластер */
        if (left == 0 && above == 0)
        {
          largestCluster++;
          grid[i][j] = largestCluster;
        }
        /* Текущий узел входит в один кластер с левым соседом */
        else if (left != 0 && above == 0)
        {
          label = find(left, labels);
          grid[i][j] = label;
        }
        /* Текущий узел входит в один кластер с верхним соседом */
        else if (left == 0 && above != 0)
        {
          label = find(above, labels);
          grid[i][j] = label;
        }
        /* Объединение текущей и соседних меток */
        else
        {
          unionLabels(left, above, labels);
          grid[i][j] = find(left, labels);
        }
      }
    }
  }

  vector<int> newLabels(maxSize, 0); // вектор хранения новых меток
  /* Перемаркирование кластеров с различными метками */
  for (int i = 1; i < grid.size(); i++)
    for (int j = 1; j < grid.size(); j++)
      if (grid[i][j])
      {
        int x = find(grid[i][j], labels);
        if (newLabels[x] == 0)
        {
          newLabels[0]++;
          newLabels[x] = newLabels[0];
        }
        grid[i][j] = newLabels[x];
      }

  /* Удаление пустой первой строки и первого столбца */
  grid.erase(grid.begin());
  for (auto &&i : grid)
  {
    i.erase(i.begin());
  }
}

#include "../include/network.h"

Network::Network(Model model)
{
  this->model = model;
}

Network::~Network()
{
}

void Network::do_experiment(ofstream &stream)
{
  /* Установка генерации случайных чисел, относительно времени */
  srand((unsigned)time(NULL));

  vector<vector<int>> grid;        // решетка
  size_t current_experiment = 0;   // текущий номер эксперимента
  int constriction_clusters_count; // счетчик стягивающих кластеров

  /**
   * Вычисление вероятности возникновения стягивающего кластера в решетке
   * для разных значений концентрации
   */
  for (double p = 0; p < 1.001; p += 0.01)
  {
    /* Обнуление счетчика счягивающих кластеров */
    constriction_clusters_count = 0;
    /* Проведение экспериментов с заданной концентрацией узлов */
    for (size_t i = 0; i < this->model.m; i++)
    {
      /* Очистка решетки перед новым экспериментом */
      grid.clear();
      grid = vector<vector<int>>(this->model.n, vector<int>(this->model.n, 0));
      /* Моделирование заполнения узлов */
      // TODO!!!!!!!! DONE
      calculate_spread(p, grid);
      /* Маркирование кластеров на решетке */
      // TODO!!!!!!!!
      create_clusters(grid);
      /* Поиск стягивающего кластера и увеличение счетчика найденных */
      // TODO!!!!!!!!
      if (search_constriction_cluster(grid))
      {
        constriction_clusters_count++;
      }
    }
    /* Запись в файл вероятности возникновения стягивающего кластера */
    stream << constriction_clusters_count / double(this->model.m) << endl;
  }
}

void Network::calculate_spread(double p, vector<vector<int>> &grid)
{
  vector<Node> avaliableNodes; // вектор доступных для занятия узлов
  /* Инициализация вектора */
  for (size_t i = 0; i < grid.size(); i++)
  {
    for (size_t j = 0; j < grid.size(); j++)
    {
      avaliableNodes.push_back(Node(i, j));
    }
  }

  int occupiedNodesCount = p * 100; // доля занятых узлов
  vector<Node> neighbors;           // вектор соседей для занятия
  vector<Node> infected;            // вектор занятых узлов
  Node selectedNeighbor;            // выбранный сосед
  int randomNum;                    // случайное значение
  int prevNeighborsSize;            // предыдущий размер вектора соседей
  int bottomInfectedSize = 0;       // нижняя граница предыдущих занятых в векторе infected
  int topInfectedSize = 0;          // верхняя граница предыдущих занятых в векторе infected
  int i = 0;                        // счетчик занятых узлов
  /* Занимаем узлы, пока не достигнем нужной концентрации */
  while (i < occupiedNodesCount)
  {
    /* Получение верхнего порога infected на предыдущей итерации */
    topInfectedSize = infected.size();
    /* Восстановление узлов, занятых на предыдущей итерации */
    for (size_t j = bottomInfectedSize; j < topInfectedSize; j++)
    {
      if (random_probability() < this->model.q2)
      {
        grid[infected[j].row][infected[j].col] = -1;
      }
    }
    /* Проверка на наличие соседей */
    if (!neighbors.size())
    {
      /**
       * Ветка условия, когда закончились соседи для распространения.
       * Происходит выбор случайного узла в решетке
       */
      randomNum = rand_num(0, avaliableNodes.size() - 1);       // получение случайного узла из доступных
      selectedNeighbor = avaliableNodes[randomNum];             // полчение его позиции
      avaliableNodes.erase(avaliableNodes.begin() + randomNum); // удаление из вектора доступных узлов
      grid[selectedNeighbor.row][selectedNeighbor.col] = true;  // занятие узла в решетке
      infected.push_back(selectedNeighbor);                     // добавление занятого узла в вектор занятых
      add_neighbors(selectedNeighbor, neighbors, grid);         // получение его соседей
      i++;                                                      // увеличение счетчика
    }
    else
    {
      /**
       * Ветка условия, когда существуют соседи.
       * Происходит занятие среди соседей узлов, занятых на предыдущей итерации
       */
      prevNeighborsSize = neighbors.size(); // предыдущий размер соседей
      bottomInfectedSize = infected.size(); // нижний порог количества зараженных
      /* Занятие соседей с вероятностью q1 */
      for (size_t j = 0; j < prevNeighborsSize && i < occupiedNodesCount; j++)
      {
        if (random_probability() < this->model.q1)
        {
          selectedNeighbor = neighbors[j]; // получение позиции занятого узла
          avaliableNodes.erase(remove(avaliableNodes.begin(),
                                      avaliableNodes.end(),
                                      selectedNeighbor));          // удаление узла из доступных
          grid[selectedNeighbor.row][selectedNeighbor.col] = true; // занятие узла в решетке
          infected.push_back(selectedNeighbor);                    // добавление узла в вектор занятых
          add_neighbors(selectedNeighbor, neighbors, grid);        // получение его соседей
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

bool Network::search_constriction_cluster(vector<vector<int>> grid)
{
  /* Поиск пары с одинаковыми метками в верхней и нижней части решетки */
  for (size_t i = 0; i < grid[0].size(); i++)
  {
    if (grid[0][i] && std::find(grid[grid.size() - 1].begin(), grid[grid.size() - 1].end(), grid[0][i]) != grid[grid.size() - 1].end())
    {
      return true;
    }
  }
  return false;
}

bool Network::is_node_correct(Node node, vector<vector<int>> &grid)
{
  int minWall = -1;          // минимальная граница решетки
  int maxWall = grid.size(); // максимальная граница решетки
  /* Узел не должен выходить за границы решетки и должен находиться только в свободном состоянии */
  return node.row != minWall && node.row != maxWall && node.col != minWall && node.col != maxWall &&
         grid[node.row][node.col] == 0;
}

void Network::add_neighbors(Node pos, vector<Node> &neighbors, vector<vector<int>> &grid)
{
  /* Добавление верхнего соседа */
  if (is_node_correct(Node(pos.row - 1, pos.col), grid) &&
      std::find(neighbors.begin(), neighbors.end(), Node(pos.row - 1, pos.col)) == neighbors.end())
  {
    neighbors.push_back(Node(pos.row - 1, pos.col));
  }
  /* Добавление нижнего соседа */
  if (is_node_correct(Node(pos.row + 1, pos.col), grid) &&
      std::find(neighbors.begin(), neighbors.end(), Node(pos.row + 1, pos.col)) == neighbors.end())
  {
    neighbors.push_back(Node(pos.row + 1, pos.col));
  }
  /* Добавление левого соседа */
  if (is_node_correct(Node(pos.row, pos.col - 1), grid) &&
      std::find(neighbors.begin(), neighbors.end(), Node(pos.row, pos.col - 1)) == neighbors.end())
  {
    neighbors.push_back(Node(pos.row, pos.col - 1));
  }
  /* Добавление правого соседа */
  if (is_node_correct(Node(pos.row, pos.col + 1), grid) &&
      std::find(neighbors.begin(), neighbors.end(), Node(pos.row, pos.col + 1)) == neighbors.end())
  {
    neighbors.push_back(Node(pos.row, pos.col + 1));
  }

  /* Добавление соседей для периодических граничных условий */
  if (model.periodic_boundaries)
  {
    /* Текущий узел находится в первой строке */
    if (pos.row == 0)
    {
      if (is_node_correct(Node(model.n - 1, pos.col), grid) && std::find(neighbors.begin(), neighbors.end(), Node(model.n - 1, pos.col)) == neighbors.end())
      {
        neighbors.push_back(Node(model.n - 1, pos.col));
      }
    }
    /* Текущий узел находится в последней строке */
    if (pos.row == model.n - 1)
    {
      if (is_node_correct(Node(0, pos.col), grid) && std::find(neighbors.begin(), neighbors.end(), Node(0, pos.col)) == neighbors.end())
      {
        neighbors.push_back(Node(0, pos.col));
      }
    }
    /* Текущий узел находится в первом столбце */
    if (pos.col == 0)
    {
      if (is_node_correct(Node(pos.row, model.n - 1), grid) && std::find(neighbors.begin(), neighbors.end(), Node(pos.row, model.n - 1)) == neighbors.end())
      {
        neighbors.push_back(Node(pos.row, model.n - 1));
      }
    }
    /* Текущий узел находится в последнем столбце */
    if (pos.col == model.n - 1)
    {
      if (is_node_correct(Node(pos.row, 0), grid) && std::find(neighbors.begin(), neighbors.end(), Node(pos.row, 0)) == neighbors.end())
      {
        neighbors.push_back(Node(pos.row, 0));
      }
    }
  }
}

int Network::find(int x, vector<int> &labels)
{
  int y = x;
  /* Поиск первой правильной метки */
  while (labels[y] != y)
  {
    y = labels[y];
  }
  while (labels[x] != x)
  {
    int z = labels[x];
    labels[x] = y;
    x = z;
  }
  return y;
}

void Network::union_labels(int x, int y, vector<int> &labels)
{
  labels[find(x, labels)] = find(y, labels); // присвоение метке x правильной кластерной метке
}

void Network::create_clusters(vector<vector<int>> &grid)
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
          union_labels(left, above, labels);
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

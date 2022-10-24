#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>
#include <iostream>

using namespace std;

struct config
{
  int n;
  int startRow;
  int startCol;
  int m;
  double q;
};

struct position
{
public:
  int row;
  int col;
  position(int row, int col)
  {
    this->row = row;
    this->col = col;
  }

  bool operator==(const position &a)
  {
    return this->row == a.row && this->col == a.col;
  }
};

config dataConfig;

bool readVariables();
void calculateSpread(double, double, vector<vector<int>> &, int, int, int &);
double randomProbability();
int find(int, vector<int> &);
void unionLables(int, int, vector<int> &);
void createClusters(vector<vector<int>> &);
bool searchConstrictionCluster(vector<vector<int>>);
void print(vector<vector<int>>);

int main(int argc, char **argv)
{
  if (!readVariables())
  {
    return 1;
  }

  srand((unsigned)time(NULL));

  const double STEP = 0.05;

  double p = 0;
  int mC = 0;
  size_t mCur = 0;
  vector<vector<int>> grid(dataConfig.n, vector<int>(dataConfig.n, 0));

  while (p <= 1.05)
  {
    do
    {
      calculateSpread(p, dataConfig.q, grid, dataConfig.startRow, dataConfig.startCol, mC);

      mCur++;

      grid.clear();
      grid = vector<vector<int>>(dataConfig.n, vector<int>(dataConfig.n, 0));
    } while (mCur < dataConfig.m);

    printf("p = %f\np(c) = %.10f\n", p, double(mC / double(dataConfig.m)));

    p += STEP;
    mCur = mC = 0;
  }
}

bool readVariables()
{
  /*
   * на вход программе подаются аргументы в следующем порядке:
   * n - размерность квадратной решетки
   * m - число экспериментов
   * startRow - позиция первого инфицированного узла по строкам
   * startCol - позиция первого инфицированного узла по столбцам
   * q - вероятность восстановления узла
   */
  errno = 0;
  char *p;
  // чтение параметров
  printf("Введите параметры\n");
  printf("Размерность решетки: ");
  scanf("%d", &dataConfig.n);
  printf("Количество экспериментов: ");
  scanf("%d", &dataConfig.m);
  printf("Позиция по строкам источника заражения: ");
  scanf("%d", &dataConfig.startRow);
  printf("Позиция по столбцам источника заражения: ");
  scanf("%d", &dataConfig.startCol);
  printf("Вероятность восстановления узла: ");
  scanf("%lf", &dataConfig.q);
  return true;
}

bool searchConstrictionCluster(vector<vector<int>> grid)
{
  for (auto &&i : grid[grid.size() - 1])
  {
    if (i == 1)
    {
      return true;
    }
  }
  return false;
}

void print(vector<vector<int>> grid)
{
  for (auto &&i : grid)
  {
    for (auto &&j : i)
    {
      printf(" %2d ", j);
    }
    printf("\n");
  }
}

double randomProbability()
{
  return (float)rand() / RAND_MAX;
}

void calculateSpread(double p, double q, vector<vector<int>> &grid, int startRow, int startCol, int &mC)
{
  grid[startRow][startCol] = true;
  position start = {startRow, startCol};
  vector<position> infected(1, start);
  size_t prevSize = infected.size();
  map<int, int> clusterSizes;

  while (prevSize)
  {
    for (auto &&node : infected)
    {
      vector<position> neighbours = {position{node.row + 1, node.col},
                                     position{node.row, node.col - 1},
                                     position{node.row - 1, node.col},
                                     position{node.row, node.col + 1}};
      for (auto &&n : neighbours)
      {
        if (n.row >= 0 && n.row < grid.size() && n.col >= 0 && n.col < grid.size() && !grid[n.row][n.col] && randomProbability() <= p)
        {
          grid[n.row][n.col] = 1;
          infected.push_back(position{n.row, n.col});
        }
      }
    }

    vector<vector<int>> copyGrid(grid);
    createClusters(copyGrid);
    vector<int> sizes;

    for (auto &&i : copyGrid)
    {
      for (auto &&j : i)
      {
        if (!j)
        {
          continue;
        }
        if (sizes.size() < j)
        {
          sizes.push_back(1);
        }
        else
        {
          sizes[j - 1]++;
        }
      }
    }

    for (auto &&i : sizes)
    {
      if (clusterSizes.find(i) == clusterSizes.end())
      {
        clusterSizes[i] = 1;
      }
      else
      {
        clusterSizes[i]++;
      }
    }

    if (searchConstrictionCluster(copyGrid))
    {
      mC++;
      return;
    }

    infected.erase(infected.begin(), infected.begin() + prevSize);

    for (size_t i = 0; i < grid.size(); i++)
    {
      for (size_t j = 0; j < grid[i].size(); j++)
      {
        position node = {int(i), int(j)};
        if (grid[i][j] && find(infected.begin(), infected.end(), node) == infected.end() && randomProbability() <= q)
        {
          grid[i][j] = 0;
        }
      }
    }

    prevSize = infected.size();
  }
}

int find(int x, vector<int> &labels)
{
  int y = x;
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
  labels[find(x, labels)] = find(y, labels);
}

void createClusters(vector<vector<int>> &grid)
{
  grid.insert(grid.begin(), vector<int>(grid.size(), 0));
  for (auto &&i : grid)
  {
    i.insert(i.begin(), 0);
  }

  int largestCluster = 0;
  int maxSize = grid.size() * grid.size() / 2;
  vector<int> labels, clusterSizes;
  for (size_t i = 0; i < maxSize; i++)
  {
    labels.push_back(i);
  }

  for (size_t i = 1; i < grid.size(); i++)
  {
    for (size_t j = 1; j < grid.size(); j++)
    {
      if (grid[i][j])
      {
        int left = grid[i][j - 1];
        int above = grid[i - 1][j];
        int label;
        if (left == 0 && above == 0)
        {
          largestCluster++;
          grid[i][j] = largestCluster;
        }
        else if (left != 0 && above == 0)
        {
          label = find(left, labels);
          grid[i][j] = label;
        }
        else if (left == 0 && above != 0)
        {
          label = find(above, labels);
          grid[i][j] = label;
        }
        else
        {
          unionLabels(left, above, labels);
          grid[i][j] = find(left, labels);
        }
      }
    }
  }

  vector<int> newLabels(maxSize, 0);

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

  grid.erase(grid.begin());
  for (auto &&i : grid)
  {
    i.erase(i.begin());
  }
}
#include <stdio.h>
#include <vector>
#include <time.h>
#include <iostream>

using namespace std;

struct position
{
  int Row;
  int Col;
};

void calculateSpread(double, double, vector<vector<int>> &, int, int);
double randomProbability();
int find(int, vector<int>);
void unionLables(int, int, vector<int> &);
void createClusters(vector<vector<int>> &);
bool searchConstrictionCluster(vector<vector<int>>);
void print(vector<vector<int>>);

int main(int argc, char **argv)
{
  srand((unsigned)time(NULL));

  const size_t N = 10;
  const size_t startRow = 4, startCol = 4;
  const size_t M = 100000;
  const double STEP = 0.1;

  double p = 0, q = 0.1;
  size_t mC = 0;
  size_t mCur = 0;
  vector<vector<int>> grid(N, vector<int>(N, 0));

  while (p <= 1)
  {
    do
    {
      calculateSpread(p, q, grid, startRow, startCol);
      createClusters(grid);

      mC += searchConstrictionCluster(grid);
      mCur++;

      grid.clear();
      grid = vector<vector<int>>(N, vector<int>(N, 0));
    } while (mCur < M);

    printf("p = %f\np(c) = %f\n", p, double(mC / double(M)));

    p += STEP;
    mCur = mC = 0;
  }
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

void calculateSpread(double p, double q, vector<vector<int>> &grid, int startRow, int startCol)
{
  grid[startRow][startCol] = true;
  position start = {startRow, startCol};
  vector<position> infected(1, start);
  size_t prevSize = infected.size();

  while (prevSize)
  {
    for (auto &&node : infected)
    {
      vector<position> neighbours = {position{node.Row + 1, node.Col},
                                     position{node.Row, node.Col - 1},
                                     position{node.Row - 1, node.Col},
                                     position{node.Row, node.Col + 1}};
      for (auto &&n : neighbours)
      {
        if (n.Row >= 0 && n.Row < grid.size() && n.Col >= 0 && n.Col < grid.size() && !grid[n.Row][n.Col] && randomProbability() <= p)
        {
          grid[n.Row][n.Col] = 1;
          infected.push_back(position{n.Row, n.Col});
        }
      }
    }
    for (size_t i = 0; i < grid.size(); i++)
    {
      for (size_t j = 0; j < grid[i].size(); j++)
      {
        if (grid[i][j] && randomProbability() <= q)
        {
          grid[i][j] = 0;
        }
      }
    }

    infected.erase(infected.begin(), infected.begin() + prevSize);
    prevSize = infected.size();
  }
}

int find(int x, vector<int> labels)
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
  vector<int> labels;
  int maxSize = grid.size() * grid.size() / 2;
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
        if (left == 0 && above == 0)
        {
          largestCluster++;
          grid[i][j] = largestCluster;
        }
        else if (left != 0 && above == 0)
        {
          grid[i][j] = find(left, labels);
        }
        else if (left == 0 && above != 0)
        {
          grid[i][j] = find(above, labels);
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
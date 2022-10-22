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

void calculateSpread(double, double, vector<vector<bool>> &, int, int);
double random();
bool searchConstrictionCluster(vector<vector<bool>>);
void print(vector<vector<bool>>);

int main(int argc, char **argv)
{
  srand((unsigned)time(NULL));

  const size_t N = 10;
  const size_t startRow = 4, startCol = 4;
  const size_t M = 10;
  const double STEP = 0.1;

  double p = 0.6, q = 0.05;
  size_t mC = 0;
  size_t mCur = 0;
  vector<vector<bool>> grid(N, vector<bool>(N, false));

  while (p <= 0.6)
  {
    do
    {
      calculateSpread(p, q, grid, startRow, startCol);
      mC += searchConstrictionCluster(grid);
      mCur++;
      grid.clear();
      grid = vector<vector<bool>>(N, vector<bool>(N, false));
    } while (mCur < M);

    p += STEP;
    mCur = mC = 0;
  }
}

void print(vector<vector<bool>> grid)
{
  for (auto &&i : grid)
  {
    for (auto &&j : i)
    {
      printf(" %d ", bool(j));
    }
    printf("\n");
  }
}

double random()
{
  return (float)rand() / RAND_MAX;
}

void calculateSpread(double p, double q, vector<vector<bool>> &grid, int startRow, int startCol)
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
        if (n.Row >= 0 && n.Row < grid.size() && n.Col >= 0 && n.Col < grid.size() && !grid[n.Row][n.Col] && random() <= p)
        {
          grid[n.Row][n.Col] = true;
          infected.push_back(position{n.Row, n.Col});
        }
      }
    }
    for (size_t i = 0; i < grid.size(); i++)
    {
      for (size_t j = 0; j < grid[i].size(); j++)
      {
        if (grid[i][j] && random() <= q)
        {
          grid[i][j] = false;
        }
      }
    }

    infected.erase(infected.begin(), infected.begin() + prevSize);
    prevSize = infected.size();
  }
}

bool searchConstrictionCluster(vector<vector<bool>> grid)
{
  return true;
}
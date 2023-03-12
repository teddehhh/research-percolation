#include "../include/fhelp.h"

Model read_model_params()
{
  Model model;
  std::string s;
  /* Чтение параметров */
  cout << "Enter parameters" << endl;
  cout << "Grid size (N): ";
  cin >> model.n;
  cout << "Number of experiments (M): ";
  cin >> model.m;
  cout << "Chance of infection (q1): ";
  cin >> model.q1;
  cout << "Chance for recovery (q2): ";
  cin >> model.q2;
  cout << "Enable periodic boundaries? [yes/no]: ";
  cin >> s;
  model.periodic_boundaries = s == "yes" ? true : false;

  return model;
}

int rand_num(int min, int max)
{
  if (max == 0)
  {
    return 0;
  }
  std::random_device r;
  std::mt19937 mersane(r());
  return min + mersane() % max;
}

double random_probability()
{
  return (double)rand() / RAND_MAX;
}

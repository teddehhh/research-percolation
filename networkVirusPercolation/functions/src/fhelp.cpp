#include "../include/fhelp.h"

Model read_model_params()
{
  Model model;
  std::string s;
  /* Чтение параметров */
  cout << "Введите параметры" << endl;
  cout << "Размерность решетки: ";
  cin >> model.n;
  cout << "Количество экспериментов: ";
  cin >> model.m;
  cout << "Вероятность заражения узла: ";
  cin >> model.q1;
  cout << "Вероятность восстановления узла: ";
  cin >> model.q2;
  cout << "Вероятность восстановления узла: ";
  cin >> model.q2;
  cout << "Ввести периодические граничные условия [да/нет]: ";
  cin >> s;
  model.periodic_boundaries = s == "да" ? true : false;

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

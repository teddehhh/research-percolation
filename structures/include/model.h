#pragma once

/**
 * @brief Хранение параметров модели
 *
 * @tparam n размерность решетки
 * @tparam m количество проводимых экспериментов
 * @tparam q1 вероятность занятия соседнего узла
 * @tparam q2 вероятность освобождения узла
 * @tparam periodic_boundaries установка периодических границ
 */
struct Model
{
public:
  int n;
  int m;
  double q1;
  double q2;
  bool periodic_boundaries;
  Model();
};
#pragma once

#include <string>
#include <iostream>
#include <random>
#include <time.h>
#include "../../structures/include/model.h"

using std::cin;
using std::cout;
using std::endl;

/**
 * @brief Запись и валидация входных данных, поступающих от пользователя
 *
 * @return Model
 */
Model read_model_params();
/**
 * @brief Получение коэффициента вероятности
 *
 * @return double - число в диапазоне от 0 до 1
 */
double random_probability();
/**
 * @brief Получение случайного числа в заданном диапазоне
 *
 * @param min минимальное значение
 * @param max максимальное значение
 * @return int - число от min до max
 */
int rand_num(int min, int max);

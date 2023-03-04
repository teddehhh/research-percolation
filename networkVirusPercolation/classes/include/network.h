#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <math.h>
#include "../../structures/include/model.h"
#include "../../structures/include/node.h"
#include "../../functions/include/fhelp.h"

using std::endl;
using std::ofstream;
using std::vector;

class Network
{
private:
  Model model;
  /**
   * @brief Процедура моделирования распространения занятых узлов и их восстановления
   *
   * @param p концентрация занятых узлов
   * @param grid решетка
   */
  void calculate_spread(double p, vector<vector<int>> &grid);
  /**
   * @brief Процедура маркирования кластеров в решетке
   *
   * @param grid решетка
   */
  void create_clusters(vector<vector<int>> &grid);
  /**
   * @brief Процедура добавления соседей текущего узла
   *
   * @param node рассматриваемый узел
   * @param neighbors соседи для заражения на текущей итерации
   * @param grid решетка
   */
  void add_neighbors(Node node, vector<Node> &neighbors, vector<vector<int>> &grid);
  /**
   * @brief Проверка узла на возможность его занять
   *
   * @param node рассматриваемый узел
   * @param grid решетка
   * @return true - узел может быть занят
   * @return false - узел нельзя занять
   */
  bool is_node_correct(Node node, vector<vector<int>> &grid);
  /**
   * @brief Поиск паравильной метки в векторе кластерных меток
   *
   * @param x рассматриваемая метка
   * @param labels кластерные метки
   * @return int - номер правильной метки
   */
  int find(int x, vector<int> &labels);
  /**
   * @brief Процедура объединения кластерных меток
   *
   * @param x первая метка
   * @param y вторая метка
   * @param labels кластерные метки
   */
  void union_labels(int x, int y, vector<int> &labels);
  /**
   * @brief Проверка наличия стигивающего кластера
   *
   * @param grid решетка
   * @return true - стягивающий кластер существует
   * @return false - стягивающий кластер отсутствует
   */
  bool search_constriction_cluster(vector<vector<int>> grid);
  /**
   * @brief Перемаркировка граничных узлов (для переменных граничных условий)
   *
   * @param grid решетка
   * @param min_label метка для перемакировки
   * @param start_node узел, с которого нужно начать перемакировку
   */
  void relable_side_nodes(vector<vector<int>> &grid, int min_label, Node start_node);

public:
  Network(Model);
  ~Network();

  /**
   * @brief
   *
   * @param stream
   */
  void do_experiment(std::ofstream &stream);
};

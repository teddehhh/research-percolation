#include "./structures/include/node.h"
#include "./structures/include/model.h"
#include "./functions/include/fhelp.h"
#include "./classes/include/network.h"

using namespace std;

/**
 * @brief Точка входа в программу
 *
 * @param argc
 * @param argv
 * @return int - код результата работы программы
 */
int main()
{
  /* Чтение и проверка корректности введенных данных */
  Model model = read_model_params();
  if (model.n == 0)
  {
    cout << "Empty grid in the model!" << endl;
    return 1;
  }

  std::ofstream output("output.txt");

  Network *network = new Network(model);
  network->modeling(output);

  output.close();

  cout << "Done!" << endl;
  return 0;
}
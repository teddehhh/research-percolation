#include "../include/node.h"

Node::Node(int row, int col)
{
  this->row = row;
  this->col = col;
}

Node::Node() {}

bool Node::operator==(const Node &a)
{
  return this->row == a.row && this->col == a.col;
}
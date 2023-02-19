#pragma once

struct Node
{
public:
  int row;
  int col;
  Node(int, int);
  Node();

  bool operator==(const Node &);
};
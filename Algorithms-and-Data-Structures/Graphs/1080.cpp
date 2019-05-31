#include <iostream>
#include <vector>

#define uint unsigned int

struct AdjacencyMatrix {
  uint edges;
  std::vector<bool> matrix;

  AdjacencyMatrix(uint n) : edges(n), matrix(n * n) {}

  void connect(uint a, uint b) {
    this->matrix[a * edges + b] = true;
  }

  void print() {
    for (uint i = 0; i < edges; i++) {
      std::cout << i << ":";
      for (uint j = 0; j < edges; j++) {
        if (matrix[i * edges + j])
          std::cout << " " << j;
      }
      std::cout << std::endl;
    }
  }
};

int main() {
  uint n;
  std::cin >> n;

  AdjacencyMatrix edges(n);

  for (uint i = 0; i < n; ++i) {
    uint j;

    while (true) {
      std::cin >> j;
      if (j == 0)
        break;
      else
        edges.connect(i, j - 1);
    }
  }

  edges.print();
}

#include <iostream>
#include <vector>

#define uint unsigned int

struct AdjacencyMatrix {
  uint vertices;
  std::vector<bool> matrix;

  AdjacencyMatrix(uint v) : vertices(v), matrix(v * v) {}

  void connect(uint a, uint b) {
    this->matrix[a * vertices + b] = true;
    this->matrix[b * vertices + a] = true;
  }
  
  bool exists(uint a, uint b) const {
    return this->matrix[a * vertices + b];
  }

  void print() const {
    for (uint i = 0; i < vertices; ++i) {
      std::cout << i << ":";
      for (uint j = 0; j < vertices; ++j) {
        if (matrix[i * vertices+ j])
          std::cout << " " << j;
      }
      std::cout << std::endl;
    }
  }
};

struct BinaryVertexColors {
  uint colored;
  std::vector<bool> colors;

  BinaryVertexColors(uint v) : colored(0), colors(v) {}

  bool has_neighbors_of_color(uint v, bool color, AdjacencyMatrix const &edges) const {
    for (uint i = 0; i < colored; ++i)
      if (edges.exists(v, i) && color == colors[i])
        return true;
    return false;
  }

  void set_color(uint v, bool color) {
    colors[v] = color;
    colored = v + 1;
  }

  void remove_color(uint v) {
    colored = v - 1;
  }
   
  std::string get_color_string() const {
    if (colored == 0 || colored < colors.size())
      return "-1";

    std::string color_string(colors.size(), '0');
    for (uint i = 0; i < colors.size(); ++i)
      if (colors[i])
        color_string[i] = '1';
    return color_string;
  }
};

int main() {
  uint v;
  std::cin >> v;

  AdjacencyMatrix edges(v);
  BinaryVertexColors vertices(v);

  for (uint i = 0; i < v; ++i) {
    uint edge;

    while (true) {
      std::cin >> edge;
      if (edge == 0)
        break;
      else
        edges.connect(i, edge - 1);
    }
  }

  while (vertices.colored != v) {
    if (!vertices.has_neighbors_of_color(vertices.colored, false, edges)) {
      vertices.set_color(vertices.colored, false);
    }
    else if (!vertices.has_neighbors_of_color(vertices.colored, true, edges)) {
      vertices.set_color(vertices.colored, true);
    }
    else if (vertices.colored > 0 && vertices.colors[vertices.colored - 1] == false &&
        !vertices.has_neighbors_of_color(vertices.colored - 1, true, edges)) {
      vertices.set_color(vertices.colored - 1, true);
    }
    else {
      bool backtracked = false;
      while (vertices.colored > 1) {
        vertices.remove_color(vertices.colored);
        if (vertices.colors[vertices.colored] == false &&
            !vertices.has_neighbors_of_color(vertices.colored, true, edges)) {
          vertices.set_color(vertices.colored, true);
          backtracked = true;
          break;
        }
      }
      if (!backtracked) break;
    }
  }

  std::cout << vertices.get_color_string() << std::endl;
  return 0;
}

#include <iostream>
#include <vector>

#define uint unsigned int

struct AdjacencyMatrix {
  uint vertices;

  AdjacencyMatrix(uint v) : vertices(v), matrix(v * v) {}

  void connect(uint a, uint b) {
    this->matrix[a * this->vertices + b] = true;
    this->matrix[b * this->vertices + a] = true;
  }

  bool exists(uint a, uint b) const {
    return this->matrix[a * this->vertices + b];
  }

 private:
  std::vector<bool> matrix;
};

struct BinaryVertexColors {
  std::vector<bool> color;
  std::vector<bool> colored;

  BinaryVertexColors(uint v) : color(v), colored(v) {}

  void set_color(int v, bool color) {
    this->color[v] = color;
    this->colored[v] = true;
  }

  std::string get_color_string() const {
    std::string color_string(this->color.size(), '0');
    for (uint i = 0; i < this->color.size(); ++i)
      if (this->color[i])
        color_string[i] = '1';
    return color_string;
  }
};

bool color_breadth_first(AdjacencyMatrix const& edges, BinaryVertexColors& coloring, uint v) {
  for (uint i = 0; i < edges.vertices; ++i) {
    if (edges.exists(v, i)) {
      if (!coloring.colored[i]) {
        coloring.set_color(i, !coloring.color[v]);
        if (!color_breadth_first(edges, coloring, i))
          return false;
      }
      else if (coloring.colored[v] && coloring.color[v] == coloring.color[i]) {
        return false;
      }
    }
  }
  return true;
}

int main() {
  uint v;
  std::cin >> v;

  AdjacencyMatrix edges(v);
  BinaryVertexColors coloring(v);

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

  coloring.set_color(0, false);
  if (color_breadth_first(edges, coloring, 0)) {
    for (uint i = 1; i < v; ++i) {
      if (!coloring.colored[i])
        color_breadth_first(edges, coloring, i);
    }
    std::cout << coloring.get_color_string() << std::endl;
  } else
    std::cout << "-1" << std::endl;

  return 0;
}

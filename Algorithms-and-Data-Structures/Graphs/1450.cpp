#include <iostream>
#include <vector>

#define uint unsigned int
#define ushort unsigned short

struct Edge {
  ushort a;
  ushort b;
  ushort weight;
};

int main() {
  uint v_count, e_count;
  std::cin >> v_count >> e_count;

  std::vector<Edge> edges(e_count);
  for (uint i = 0; i < e_count; ++i) {
    ushort a, b, weight;
    std::cin >> a >> b >> weight;
    edges[i] = {a - 1, b - 1, weight};
  }

  ushort v_start, v_dest;
  std::cin >> v_start >> v_dest;
  --v_start;
  --v_dest;

  std::vector<int> weights(v_count, -1);
  weights[v_start] = 0;

  for (uint v = 1; v < v_count; ++v) {
    for (auto edge : edges) {
      if (weights[edge.a] != -1 && weights[edge.a] + edge.weight > weights[edge.b])
        weights[edge.b] = weights[edge.a] + edge.weight;
    }
  }

  if (weights[v_dest] == -1)
    std::cout << "No solution" << std::endl;
  else
    std::cout << weights[v_dest] << std::endl;

  return 0;
}

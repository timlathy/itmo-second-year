#include <iostream>
#include <vector>
#include <set>

#define uint unsigned int
#define ushort unsigned short

struct Edge {
  ushort a;
  ushort b;
  ushort weight;
};

int main() {
  uint vs, es, v_from, v_to;
  std::cin >> vs >> es;

  std::vector<Edge> edges(es);
  for (uint i = 0; i < es; ++i) {
    ushort a, b, weight;
    std::cin >> a >> b >> weight;
    edges[i] = {a - 1, b - 1, weight};
  }

  std::cin >> v_from >> v_to;
  --v_from; --v_to;

  std::vector<bool> visited(vs);
  std::vector<int> weights(vs, -1);

  uint v = v_from;
  uint total_visited = 0;

  weights[v] = 0;

  while (total_visited != vs) {
    if (!visited[v]) {
      visited[v] = true;
      total_visited++;
    }
    for (uint i = 0; i < es; ++i) {
      if (edges[i].a == v) {
        ushort v2 = edges[i].b;
        if (weights[v] + edges[i].weight > weights[v2]) {
          weights[v2] = weights[v] + edges[i].weight;
          if (visited[v2]) {
            visited[v2] = false;
            total_visited--;
          }
        }
      }
    }
    uint next_v = 0, max_weight = 0;
    for (uint vis = 0; vis < vs; ++vis) {
      if (!visited[vis]) continue;

      for (uint e = 0; e < es; ++e) {
        if (edges[e].a == vis && !visited[edges[e].b] && edges[e].weight > max_weight) {
          max_weight = edges[e].weight;
          next_v = edges[e].b;
        }
      }
    }
    v = next_v;
  }

  std::cout << weights[v_to] << std::endl;

  //for (int i = 0; i < weights.size(); ++i) {
  //  std::cout << i << ": " << weights[i] << std::endl;
  //}

  return 0;
}

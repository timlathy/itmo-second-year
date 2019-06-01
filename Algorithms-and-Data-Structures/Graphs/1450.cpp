#include <cstdio>
#include <vector>

#define uint unsigned int
#define ushort unsigned short

struct Edge {
  ushort a;
  ushort b;
  ushort weight;
};

#ifdef _WIN32
// getchar_unlocked is Unix-specific, on Windows it's _getchar_nolock
#define getchar_unlocked _getchar_nolock
#endif

template <class N>
static N readnum() {
  N num = 0;
  char c;

  do { c = getchar_unlocked(); }
  while (c == ' ' || c == '\n' || c == '\r');

  while (c >= '0') {
    num = 10 * num + (c - '0');
    c = getchar_unlocked();
  }

  return num;
}

int main() {
  uint v_count = readnum<uint>(), e_count = readnum<uint>();

  std::vector<Edge> edges(e_count);
  for (uint i = 0; i < e_count; ++i) {
    ushort a = readnum<ushort>(), b = readnum<ushort>(), weight = readnum<ushort>();
    edges[i] = {a - 1, b - 1, weight};
  }

  ushort v_start = readnum<uint>() - 1, v_dest = readnum<uint>() - 1;

  std::vector<int> weights(v_count, -1);
  weights[v_start] = 0;

  for (uint v = 1; v < v_count; ++v) {
    for (auto edge : edges) {
      if (weights[edge.a] != -1 && weights[edge.a] + edge.weight > weights[edge.b])
        weights[edge.b] = weights[edge.a] + edge.weight;
    }
  }

  if (weights[v_dest] == -1)
    puts("No solution");
  else
    printf("%d\n", weights[v_dest]);

  return 0;
}

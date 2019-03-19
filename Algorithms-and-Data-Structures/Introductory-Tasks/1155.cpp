#include <iostream>

const char* VNAMES[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};
const int VNEIGHBORS[8][3] = {{1, 3, 4}, {0, 2, 5}, {1, 3, 6}, {0, 3, 7},
                              {0, 5, 7}, {1, 4, 6}, {2, 5, 7}, {3, 4, 6}};

const int NON_ADJACENT_1[4] = {0, 2, 5, 7};
const int NON_ADJACENT_COMPL[4] = {1, 3, 4, 6};

int find_non_zero_complement(int* vertices, int v) {
  for (int n = 0; n < 4; n++) {
    int candidate = NON_ADJACENT_COMPL[n];
    if (vertices[candidate] > 0)
      return candidate;
  }

  exit(1);  // should not be reached
}

void drop_common_adjacent(int* vertices, int a, int b) {
  for (int n_a = 0; n_a < 3; n_a++) {
    int neighbor_a = VNEIGHBORS[a][n_a];
    for (int n_n_a = 0; n_n_a < 3; n_n_a++) {
      int neighbor_neighbor_a = VNEIGHBORS[neighbor_a][n_n_a];
      if (neighbor_neighbor_a == VNEIGHBORS[b][0] ||
          neighbor_neighbor_a == VNEIGHBORS[b][1] ||
          neighbor_neighbor_a == VNEIGHBORS[b][2]) {
        vertices[neighbor_neighbor_a]++;
        vertices[a]--;
        std::cout << VNAMES[neighbor_neighbor_a] << VNAMES[neighbor_a] << "+"
                  << std::endl;
        std::cout << VNAMES[neighbor_a] << VNAMES[a] << "-" << std::endl;
      }
    }
  }
}

int main() {
  int vertices[8];
  for (int i = 0; i < 8; i++)
    std::cin >> vertices[i];

  int sum_non_adjacent_1 =
      vertices[0] + vertices[2] + vertices[5] + vertices[7];
  int sum_non_adjacent_2 =
      vertices[1] + vertices[3] + vertices[4] + vertices[6];

  if (sum_non_adjacent_1 != sum_non_adjacent_2) {
    std::cout << "IMPOSSIBLE" << std::endl;
    return 0;
  }

  bool needs_another_run;
  do {
    needs_another_run = false;

    for (int v_idx = 0; v_idx < 4; v_idx++) {
      int v = NON_ADJACENT_1[v_idx];

      for (int n_idx = 0; n_idx < 3; n_idx++) {
        int n = VNEIGHBORS[v][n_idx];

        while (vertices[v] > 0 && vertices[n] > 0) {
          vertices[v]--;
          vertices[n]--;
          std::cout << VNAMES[v] << VNAMES[n] << "-" << std::endl;
        }
      }

      if (vertices[v] > 0) {
        int candidate = find_non_zero_complement(vertices, v);
        drop_common_adjacent(vertices, v, candidate);
        needs_another_run = true;
      }
    }
  } while (needs_another_run);

  return 0;
}

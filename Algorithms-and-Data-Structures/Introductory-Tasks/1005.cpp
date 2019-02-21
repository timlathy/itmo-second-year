#include <cmath>
#include <iostream>

int main() {
  int n;
  std::cin >> n;

  int* weights = new int[n];
  int weights_sum = 0;

  for (int i = 0; i < n; i++) {
    std::cin >> weights[i];
    weights_sum += weights[i];
  }

  int min_diff = 100000;
  int permutations = std::pow(2, n);

  for (int perm = 1; perm <= permutations; perm++) {
    int sum = 0;

    for (int i = 0; i < n; i++)
      if (perm & (1 << i))
        sum += weights[i];

    int diff = std::abs(sum - (weights_sum - sum));

    if (diff < min_diff)
      min_diff = diff;
  }

  std::cout << min_diff << std::endl;

  return 0;
}

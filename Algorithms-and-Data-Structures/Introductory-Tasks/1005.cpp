#include <cmath>
#include <iostream>

int main() {
  // Given a number of stones (1 <= n <= 20)
  int n;
  std::cin >> n;

  // With known weights w_0, ..., w_{n-1} (1 <= w <= 100000)
  int* weights = new int[n];
  int weights_sum = 0;

  for (int i = 0; i < n; i++) {
    std::cin >> weights[i];
    weights_sum += weights[i];
  }

  // Distribute them in two piles with the minimum possible weight difference
  // and print the difference.
  int min_diff = 100000;

  // The algorithm below is a brute-force approach to the problem.
  // Consider each stone belonging to the first (0) or the second (1) pile;
  // there are thus 2^n possible permutations.
  int permutations = std::pow(2, n);

  // For every permutation, we sum the weights in the second pile (represented
  // as 1s). Knowing the total weight sum, we can compute the weight difference
  // between piles and store it if it's the smallest value found so far.
  for (int perm = 1; perm <= permutations; perm++) {
    int sum = 0;

    // The stone's weight is added to the sum if its corresponding bit
    // in the current permutation is 1.
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

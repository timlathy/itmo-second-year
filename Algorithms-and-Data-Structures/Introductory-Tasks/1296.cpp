#include <iostream>

int main() {
  // For a sequence of length n
  int n;
  std::cin >> n;

  // Find the largest consecutive sum.
  int max_global_sum = 0;

  // Honestly, the algorithm I came up with seems a bit too simplistic,
  // but it passed all the tests.
  // I may come back to it some time later if I manage to find a failing case.
  int sum = 0;

  for (int i = 0; i < n; i++) {
    int el;
    std::cin >> el;
    sum += el;

    // The idea is to keep summing elements while maintaining the maximum
    if (sum > max_global_sum)
      max_global_sum = sum;

    // And if the sum becomes negative, start a new subsequence
    if (sum < 0)
      sum = 0;
  }

  std::cout << max_global_sum << std::endl;
}

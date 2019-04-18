#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  // Given n people
  unsigned long long n;
  std::cin >> n;

  // Located at (x, y)
  std::vector<unsigned int> xs(n);
  std::vector<unsigned int> ys(n);

  for (unsigned long long i = 0; i < n; ++i) {
    std::cin >> xs[i] >> ys[i];
  }

  // Find the average distance a person travels to visit every other person.
  //
  // Assuming travel in straight lines only, we compute the total X and total Y
  // distances separately then sum them.
  //
  // The algorithm treats every two adjacent points as a path segment
  // (S = x[i + 1] - x[i]) travelled by a number of people determined as
  // (i + 1) * (n - i - 1) * 2, where
  // (i + 1): the number of people travelling from the start (low coordinates),
  // (n - i - 1): the number of people travelling from the end (high coordinates),
  // 2: person i to person i + 1 <-> person i + 1 to person i

  std::sort(xs.begin(), xs.end());
  std::sort(ys.begin(), ys.end());

  unsigned long long total_distance = 0;

  for (unsigned long long i = 0; i < (n - 1); ++i) {
    total_distance += (xs[i + 1] - xs[i]) * (i + 1) * (n - i - 1) * 2;
    total_distance += (ys[i + 1] - ys[i]) * (i + 1) * (n - i - 1) * 2;
  }

  // To find the average distance, we divide total distance by the number
  // of paths taken by n people (each person visit (n - 1) people)
  total_distance /= n * (n - 1);

  std::cout << total_distance << std::endl;
}

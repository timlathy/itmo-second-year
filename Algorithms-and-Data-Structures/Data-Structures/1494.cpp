#include <iostream>
#include <stack>

int main() {
  int n;
  std::cin >> n;

  std::stack<int> uncollected;
  int largest_collected = 0;

  for (int i = 0; i < n; ++i) {
    int ball;
    std::cin >> ball;

    if (ball > largest_collected) {
      for (int b = largest_collected + 1; b < ball; ++b)
        uncollected.push(b);

      largest_collected = ball;
    }
    else if (ball == uncollected.top()) {
      uncollected.pop();
    }
    else {
      std::cout << "Cheater" << std::endl;
      return 0;
    }
  }

  std::cout << "Not a proof" << std::endl;
}

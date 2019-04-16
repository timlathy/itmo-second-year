#include <algorithm>
#include <iostream>
#include <vector>

// TODO: explain the algorithm & implementation

struct Point {
  int x;
  int y;

  bool operator<(const Point& other) const { return x < other.x; }
};

struct Line {
  unsigned short index;
  double slope;

  bool operator<(const Line& other) const { return slope < other.slope; }
};

int main() {
  unsigned short n;
  std::cin >> n;

  std::vector<Point> points(n);

  for (unsigned short i = 0; i < n; ++i) {
    std::cin >> points[i].x >> points[i].y;
  }

  auto most_distant = std::min_element(points.begin(), points.end());
  unsigned short most_distant_index = std::distance(points.begin(), most_distant);

  std::vector<Line> lines;
  lines.reserve(n - 1);
  for (auto point = points.begin(); point < points.end(); ++point) {
    if (point != most_distant) {
      Line line;
      line.index = std::distance(points.begin(), point);
      line.slope = ((double)point->y - (double)most_distant->y) / ((double)point->x - (double)most_distant->x);
      lines.push_back(line);
    }
  }

  std::sort(lines.begin(), lines.end());

  std::cout << most_distant_index + 1 << std::endl;
  std::cout << lines[lines.size() / 2].index + 1 << std::endl;
}

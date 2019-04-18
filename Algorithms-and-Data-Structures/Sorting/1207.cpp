#include <algorithm>
#include <iostream>
#include <vector>

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
  // Given n points
  unsigned short n;
  std::cin >> n;

  std::vector<Point> points(n);

  for (unsigned short i = 0; i < n; ++i) {
    std::cin >> points[i].x >> points[i].y;
  }

  // Find two points such that the line passing through them
  // divides the entire set of points into two equally sized parts.

  // First, we find the most distant point
  auto most_distant = std::min_element(points.begin(), points.end());
  unsigned short most_distant_index = std::distance(points.begin(), most_distant);

  // Then we compute line slopes between the point we picked
  // and each of the remaining ones.
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

  // The line that's located in the middle (has the median slope)
  // is the one we're looking for.
  std::sort(lines.begin(), lines.end());

  std::cout << most_distant_index + 1 << std::endl;
  std::cout << lines[lines.size() / 2].index + 1 << std::endl;
}

#include <iostream>
#include <map>

struct FSNode {
  std::map<std::string, FSNode> children;
};

void print_node(int level, FSNode const* node) {
  for (auto c = node->children.cbegin(); c != node->children.cend(); ++c) {
    std::cout << std::string(level, ' ') << c->first << std::endl;
    print_node(level + 1, &c->second);
  }
}

int main() {
  unsigned short n;
  std::cin >> n;

  const std::string path_sep = "\\";

  FSNode root_node;

  for (unsigned short i = 0; i < n; ++i) {
    std::string fs_path;
    std::cin >> fs_path;

    auto curr_node = &root_node;

    auto segm_start = 0;
    auto segm_end = fs_path.find(path_sep);
    while (segm_end != std::string::npos) {
      auto segm = fs_path.substr(segm_start, segm_end - segm_start);
      segm_start = segm_end + path_sep.length();
      segm_end = fs_path.find(path_sep, segm_start);

      curr_node = &curr_node->children[segm];
    }
    auto last_segm = fs_path.substr(segm_start);
    curr_node->children[last_segm];
  }

  print_node(0, &root_node);
}

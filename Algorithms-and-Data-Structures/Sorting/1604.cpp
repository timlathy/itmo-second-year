#include <algorithm>
#include <iostream>
#include <vector>

struct Group {
  unsigned short index;
  unsigned short frequency;
};

bool sort_groups_descending(const Group a, const Group b) {
  return a.frequency > b.frequency;
}

int main() {
  // For k different groups of elements
  int k;
  std::cin >> k;

  // Given the number of their occurrences (i.e. their frequencies)
  std::vector<Group> groups(k);
  unsigned short total_frequency = 0;

  for (unsigned short i = 0; i < k; ++i) {
    groups[i].index = i + 1;
    std::cin >> groups[i].frequency;
    total_frequency += groups[i].frequency;
  }

  // We need to arrange the elements in such a way that as few as possible
  // consecutive elements are from the same group.

  std::sort(groups.begin(), groups.end(), sort_groups_descending);

  unsigned int previously_chosen_index = -1;

// Pick the group with the highest frequency that hasn't been chosen the last time
Loop_Find_Next_From_Another_Group:
  for (auto group = groups.begin(); group < groups.end(); ++group) {
    if (group->index != previously_chosen_index && group->frequency > 0) {
      std::cout << group->index << " ";
      group->frequency--;
      previously_chosen_index = group->index;

      // Ensure that the groups are always sorted by their frequency
      auto next_group = std::next(group);
      if (next_group != groups.end() && group->frequency < next_group->frequency) {
        std::iter_swap(group, std::next(group));
      }

      goto Loop_Find_Next_From_Another_Group;
    }
  }

  // At this point, there's either no groups left or just one.
  // In the latter case have no choice but to repeat it.
  // (It'll always be the first group because we keep the groups sorted by
  // frequency)
  for (unsigned short freq = 0; freq < groups[0].frequency; ++freq) {
    std::cout << groups[0].index << " ";
  }

  std::cout << std::endl;
}

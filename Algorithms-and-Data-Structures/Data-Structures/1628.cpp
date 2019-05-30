#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

struct CalendarCell {
  unsigned short week;
  unsigned short day;

  bool operator==(const CalendarCell& other) const {
    return this->week == other.week && this->day == other.day;
  }
};

namespace std {
template <>
struct hash<CalendarCell> {
  size_t operator()(const CalendarCell& hashed) const {
    return (hashed.week << 16) | hashed.day;
  }
};
}  // namespace std

bool cmp_cell_week(CalendarCell a, CalendarCell b) {
  return a.week < b.week || (a.week == b.week && a.day < b.day);
}

bool cmp_cell_day(CalendarCell a, CalendarCell b) {
  return a.day < b.day || (a.day == b.day && a.week < b.week);
}

template <class E>
bool contains(const std::unordered_set<E>& in_set, E element) {
  return in_set.find(element) != in_set.end();
}

int main() {
  unsigned short weeks, weekdays, k;
  std::cin >> weeks >> weekdays >> k;

  unsigned int streaks = 0;
  if (k == 0) {
    streaks = (weeks == 1 || weekdays == 1) ? 1 : weeks + weekdays;
  }
  else {
    std::vector<CalendarCell> bad_days(k);

    for (unsigned short i = 0; i < k; ++i) {
      CalendarCell cell;
      std::cin >> cell.week >> cell.day;
      bad_days[i] = cell;
    }

    std::unordered_set<CalendarCell> single_cell_streaks;

    // First, count the rows
    std::sort(bad_days.begin(), bad_days.end(), cmp_cell_week);

    if (weekdays > 1) {
      streaks += bad_days[0].week - 1;
      streaks += weeks - bad_days[k - 1].week;
    }

    if (bad_days[0].day == 2 && weeks == 1)
      ++streaks;
    else if (bad_days[0].day == 2)
      single_cell_streaks.insert({bad_days[0].week, 1});
    else if (bad_days[0].day > 2)
      ++streaks;

    if (bad_days[k - 1].day == weekdays - 1 && weeks == 1)
      ++streaks;
    else if (bad_days[k - 1].day == weekdays - 1)
      single_cell_streaks.insert({bad_days[k - 1].week, weekdays});
    else if (bad_days[k - 1].day < weekdays - 1)
      ++streaks;

    for (unsigned short i = 1; i < k; ++i) {
      CalendarCell cell = bad_days[i], prev_cell = bad_days[i - 1];

      if (cell.week > prev_cell.week) {
        if (prev_cell.day == weekdays - 1)
          single_cell_streaks.insert({prev_cell.week, weekdays});
        else if (prev_cell.day < weekdays - 1)
          streaks++;

        if (weekdays > 1)
          streaks += cell.week - prev_cell.week - 1;

        if (cell.day == 2)
          single_cell_streaks.insert({cell.week, 1});
        else if (cell.day > 2)
          ++streaks;
      }
      else if (cell.day - prev_cell.day == 2 && weeks == 1)
        ++streaks;
      else if (cell.day - prev_cell.day == 2)
        single_cell_streaks.insert({cell.week, cell.day - 1});
      else if (cell.day - prev_cell.day > 2)
        ++streaks;
    }

    // Next, count the columns
    std::sort(bad_days.begin(), bad_days.end(), cmp_cell_day);

    if (weeks > 1) {
      streaks += bad_days[0].day - 1;
      streaks += weekdays - bad_days[k - 1].day;
    }

    if (bad_days[0].week == 2 && weekdays == 1)  // above the first day
      ++streaks;
    else if (bad_days[0].week > 2 ||
             (bad_days[0].week == 2 &&
              contains(single_cell_streaks, {1, bad_days[0].day})))
      ++streaks;

    if (bad_days[k - 1].week == weeks - 1 &&
        weekdays == 1)  // below the last day
      ++streaks;
    else if (bad_days[k - 1].week < weeks - 1 ||
             (bad_days[k - 1].week == weeks - 1 &&
              contains(single_cell_streaks, {weeks, bad_days[k - 1].day})))
      ++streaks;

    for (unsigned short i = 1; i < k; ++i) {
      CalendarCell cell = bad_days[i], prev_cell = bad_days[i - 1];

      if (cell.day > prev_cell.day) {
        if (prev_cell.week < weeks - 1 ||
            (prev_cell.week == weeks - 1 &&
             contains(single_cell_streaks, {weeks, prev_cell.day})))
          streaks++;

        if (weeks > 1)
          streaks += cell.day - prev_cell.day - 1;

        if (cell.week > 2 ||
            (cell.week == 2 &&
             contains(single_cell_streaks, {cell.week - 1, cell.day})))
          streaks++;
      }
      else if (cell.week - prev_cell.week == 2 && weekdays == 1)
        ++streaks;
      else if (cell.week - prev_cell.week > 2 ||
               (cell.week - prev_cell.week == 2 &&
                contains(single_cell_streaks, {cell.week - 1, cell.day})))
        ++streaks;
    }
  }

  std::cout << streaks << std::endl;
}

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

// TODO: map -> set

struct CalendarCell {
  unsigned short week;
  unsigned short day;

  bool operator==(const CalendarCell &other) const {
    return this->week == other.week && this->day == other.day;
  }
};

namespace std {
template <>
struct hash<CalendarCell> {
  size_t operator()(const CalendarCell &hashed) const {
    return (hashed.week << 16) | hashed.day;
  }
};
}

bool cmp_cell_week(CalendarCell a, CalendarCell b) {
  return a.week < b.week || (a.week == b.week && a.day < b.day);
}

bool cmp_cell_day(CalendarCell a, CalendarCell b) {
  return a.day < b.day || (a.day == b.day && a.week < b.week);
}

int main() {
  unsigned short weeks, weekdays, k;
  std::cin >> weeks >> weekdays >> k;

  std::vector<CalendarCell> bad_days(k);

  for (unsigned short i = 0; i < k; ++i) {
    CalendarCell cell;
    std::cin >> cell.week >> cell.day;
    bad_days[i] = cell;
  }

  unsigned short streaks = 0;

  if (k == 0) {
    if (weeks == 1 || weekdays == 1)
      std::cout << 1 << std::endl;
    else
      std::cout << weeks + weekdays << std::endl;
    return 0;
  }

  std::unordered_map<CalendarCell, unsigned char> single_cell_streaks;

  // First, count the rows
  std::sort(bad_days.begin(), bad_days.end(), cmp_cell_week);

  if (weekdays > 1) {
    streaks += bad_days[0].week - 1;
    streaks += weeks - bad_days[k - 1].week;
  }

  if (bad_days[0].day == 2 && weeks == 1)
    ++streaks;
  else if (bad_days[0].day == 2)
    ++single_cell_streaks[{bad_days[0].week, 1}];
  else if (bad_days[0].day > 2)
    ++streaks;

  if (bad_days[k - 1].day == weekdays - 1 && weeks == 1)
    ++streaks;
  else if (bad_days[k - 1].day == weekdays - 1)
    ++single_cell_streaks[{bad_days[k - 1].week, weekdays}];
  else if (bad_days[k - 1].day < weekdays - 1)
    ++streaks;

  for (unsigned short i = 1; i < k; ++i) {
    CalendarCell cell = bad_days[i], prev_cell = bad_days[i - 1];

    if (cell.week > prev_cell.week) {
      if (prev_cell.day == weekdays - 1)
        ++single_cell_streaks[{prev_cell.week, weekdays}];
      else if (prev_cell.day < weekdays)
        streaks++;

      streaks += cell.week - prev_cell.week - 1;

      if (cell.day == 2)
        ++single_cell_streaks[{cell.week, 1}];
      else if (cell.day > 2)
        ++streaks;
    }
    else if (cell.day - prev_cell.day == 2)
      ++single_cell_streaks[{cell.week, cell.day - 1}];
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
  else if (bad_days[0].week == 2)
    ++single_cell_streaks[{1, bad_days[0].day}];
  else if (bad_days[0].week > 2)
    ++streaks;

  if (bad_days[k - 1].week == weeks - 1 && weekdays == 1)  // below the last day
    ++streaks;
  if (bad_days[k - 1].week == weeks - 1)  // below the last day
    ++single_cell_streaks[{weeks, bad_days[k - 1].day}];
  if (bad_days[k - 1].week < weeks - 1)  // below the last day
    ++streaks;

  for (unsigned short i = 1; i < k; ++i) {
    CalendarCell cell = bad_days[i], prev_cell = bad_days[i - 1];

    if (cell.day > prev_cell.day) {
      if (prev_cell.week == weeks - 1)
        ++single_cell_streaks[{weeks, prev_cell.day}];
      else if (prev_cell.week < weeks - 1)
        streaks++;

      streaks += cell.day - prev_cell.day - 1;

      if (cell.week == 2)
        ++single_cell_streaks[{cell.week - 1, cell.day}];
      else if (cell.week > 2)
        streaks++;
    }
    else if (cell.week - prev_cell.week == 2)
      ++single_cell_streaks[{cell.week - 1, cell.day}];
    else if (cell.week - prev_cell.week > 2)
      streaks++;
  }

  for (auto it = single_cell_streaks.cbegin(); it != single_cell_streaks.cend(); ++it)
    if (it->second > 1) streaks++;

  std::cout << streaks << std::endl;
}

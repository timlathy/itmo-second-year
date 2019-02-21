#include <iostream>

int main() {
  int tests;
  std::cin >> tests;

  for (int t = 0; t < tests; t++) {
    // Divide n players into k teams, maximizing the number of unique
    // combinations of players from different teams.
    int n, k;
    std::cin >> n >> k;

    int combinations = 0;

    // The number of combinations is the highest when players are
    // distributed evenly between teams, since n^2 > (n+1) * (n-1).
    const int players_per_team = n / k;
    // The number of students is not necessarily divisible by the number of
    // teams. Remaining players are placed one by one in the first (n % k)
    // teams.
    const int unassigned_players = n % k;

    // This configuration leaves us with (n % k) teams having (n / k + 1)
    // players, and (k - n % k) teams with (n / k) players.

    // We'll tackle the (n / k + 1) teams first. Remember, each player is
    // be paired with each member of all opposing teams.
    // For any two teams, the number of pairs we can form is the multiple of the
    // number of members in each of them.
    // To avoid repetition (player A - player B, player B - player A),
    // we reduce the number of teams on every iteration, essentially multiplying
    // the number of pairs between two teams by a factorial of the team count
    // minus 1.
    // Note that the number of members in teams is not uniform, due to the first
    // n % k teams having one additional member.
    for (int i = 0; i < unassigned_players; i++) {
      combinations += (unassigned_players - i - 1) * (players_per_team + 1) *
                      (players_per_team + 1);
      combinations += (k - unassigned_players) * (players_per_team + 1) *
                      (players_per_team);
    }
    // (I think it's possible to derive a single equation that doesn't require
    // iteration here.)
    for (int i = unassigned_players; i < k; i++) {
      combinations += (k - i - 1) * players_per_team * players_per_team;
    }

    std::cout << combinations << std::endl;
  }
}

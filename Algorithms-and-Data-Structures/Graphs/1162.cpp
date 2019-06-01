#include <iostream>
#include <vector>

#define uint unsigned int
#define ushort unsigned short

struct CurrencyConversion {
  ushort a, b;
  float rate, fee;

  float exchange(float amount) const {
    return (amount - fee) * rate;
  }
};

int main() {
  ushort v_count, e_count, v_start;
  float w_start;
  std::cin >> v_count >> e_count >> v_start >> w_start;
  --v_start;
  e_count *= 2; /* edges are bidirectional */

  std::vector<CurrencyConversion> conversions(e_count);
  for (ushort e = 0; e < e_count; e += 2) {
    ushort a, b;
    float rate_ab, fee_ab, rate_ba, fee_ba;
    std::cin >> a >> b >> rate_ab >> fee_ab >> rate_ba >> fee_ba;
    conversions[e] = {a - 1, b - 1, rate_ab, fee_ab};
    conversions[e + 1] = {b - 1, a - 1, rate_ba, fee_ba};
  }

  std::vector<float> balance(v_count, 0.0);
  balance[v_start] = w_start;

  for (ushort v = 1; v < v_count; ++v) {
    for (const auto &conv : conversions) {
      if (conv.exchange(balance[conv.a]) - balance[conv.b] > 0.00001)
        balance[conv.b] = conv.exchange(balance[conv.a]);
    }
  }

  for (const auto &conv : conversions) {
    if (conv.exchange(balance[conv.a]) - balance[conv.b] > 0.00001) {
      std::cout << "YES" << std::endl;
      return 0;
    }
  }

  std::cout << "NO" << std::endl;
  return 0;
}

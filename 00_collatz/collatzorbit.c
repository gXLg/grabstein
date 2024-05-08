#include <stdint.h>

// <lookup>
// </lookup>

uint64_t collatz_orbit(uint64_t n, uint64_t k) {
  while (k >= bits) {
    uint64_t m = max_i[n & mask];
    if (n > max[m]) return 0;
    uint64_t f = func_i[n & mask];
    n = (n >> func[f][0]) * func[f][1] + func[f][2];
    k -= bits;
    if (n == 4) return 4 >> ( k      % 3);
    if (n == 2) return 4 >> ((k + 1) % 3);
    if (n == 1) return 4 >> ((k + 2) % 3);
  }

  while (k > 1) {
    if (n & 3) {
      if (n > (0xAAAAAAAAAAAAAAA8 >> (n & 1))) return 0;
      n += (n >> 1) + 1;
    } else {
      if (n == 4) return 4 >> (k % 3);
      n >>= 2;
    }
    k -= 2;
  }

  if (k) {
    if (n & 1) {
      if (n > 0x5555555555555554) return 0;
      return n + (n << 1) + 1;
    }
    return n >> 1;
  }

  return n;
}

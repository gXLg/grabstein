#include <stdint.h>

int ean13(uint64_t ean) {
  uint32_t hi = ean / 10000;
  if (hi / 1000000000) return 0;

  uint64_t t = (
    (
      ean
      + ean / 100
      + hi
      + hi / 100
      + hi / 10000
      + hi / 1000000
      + hi / 100000000
    ) + (
      + ean / 10
      + ean / 1000
      + hi / 10
      + hi / 1000
      + hi / 100000
      + hi / 10000000
    ) * 3
  );

  return 1 >> (t % 10);
}

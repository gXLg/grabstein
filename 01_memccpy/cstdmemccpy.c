#include <stddef.h>

void* memccpy(void *dest, const void *src, int c, register size_t n) {
  // from Apple FreeBSD project
  // (https://opensource.apple.com/source/Libc/Libc-825.25/string/FreeBSD/memccpy.c.auto.html)
  // adjusted for the exercise
  if (n) {
    register unsigned char *tp = dest;
    register const unsigned char *fp = src;
    register unsigned char uc = c;
    register unsigned char ch;
    do {
      if ((*tp++ = *fp++) == uc) return tp;
    } while(--n != 0);
  }
  return 0;
}

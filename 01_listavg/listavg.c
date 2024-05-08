#include "listavg.h"

double listavg(const struct node* list) {
  if (!list) return 0;

  struct node i = { list -> next, list -> val };
  struct node *l = &i;

  double sum = 0;
  double len = 0;
  do {
    sum += l -> val;
    len += 1;
    l = l -> next;
  } while (l);

  return sum / len;
}

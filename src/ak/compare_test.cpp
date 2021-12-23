#include "../../include/ak/compare.h"

#include <assert.h>

struct S {
  bool operator< (const S &that) const {
    return false;
  }
};

int main () {
  assert(ak::equals(233, 233));
  assert(!ak::equals(1926, 817));
  assert(ak::equals(S(), S()));
}

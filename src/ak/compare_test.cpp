#include "ak/compare.h"

#include <assert.h>

struct S {
  auto operator< (const S &that) const -> bool {
    return false;
  }
};

auto main () -> int {
  assert(ak::equals(233, 233));
  assert(!ak::equals(1926, 817));
  assert(ak::equals(S(), S()));
}

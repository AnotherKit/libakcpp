#include "ak/validator.h"

#include <string>

using ak::validator::expect;

auto main () -> int {
  expect(233).toBe(233);
  expect(666).Not().toBe(233);
  std::string s = "233";
  std::string s1 = s;
  expect(s1).toInclude(s);
}

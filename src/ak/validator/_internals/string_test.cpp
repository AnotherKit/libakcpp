#include "../../../../include/ak/validator/_internals/string.h"

#include <string>
#include <functional>

using std::string;
using ak::validator::_internals::Validator;

auto main () -> int {
  string s = "233";
  string s1 = s;
  Validator<std::string> v(s);
  v.toBe(s1);
  v.Not().toBe("6");
  v.toBeConsistedOf("23").butNot().toBeConsistedOf("3");
  v.toBeGreaterThan("123").toBeLessThan("333");
  v.toBeOfLength(3).butNot().toBeOfLength(4);
  v.toBeLongerThan(2).butNot().toBeLongerThan(3);
  v.toMatch("23+").butNot().toMatch("23");
  v.toPartiallyMatch("3").butNot().toPartiallyMatch("6");
  v.toInclude("23").butNot().toInclude("333");
  v.toBeOneOf({ "233", "1926" }).butNot().toBeOneOf({ "1926", "0817" });
}

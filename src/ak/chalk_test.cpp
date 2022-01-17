#include "../../include/ak/chalk.h"

#include <assert.h>
#include <iostream>

auto equal (const std::string &lhs, const std::string &rhs) -> void {
  std::cerr << lhs << std::endl;
  assert(lhs == rhs);
}

using namespace ak::chalk;

auto main () -> int {
  equal(
    bold("bold " + red("red " + dim("dim") + " red") + " bold"),
    "\x1B[1mbold \x1B[31mred \x1B[2mdim\x1B[22m\x1B[1m red\x1B[39m bold\x1B[22m"
  );
  equal(
    magenta(
      "magenta " + yellow(
        "yellow " + cyan("cyan") + " " + red("red") + " " + green("green") + " yellow"
      ) + " magenta"
    ),
    "\x1B[35mmagenta \x1B[33myellow \x1B[36mcyan\x1B[33m \x1B[31mred\x1B[33m \x1B[32mgreen\x1B[33m yellow\x1B[35m magenta\x1B[39m"
  );
  equal(blue(""), "");
}

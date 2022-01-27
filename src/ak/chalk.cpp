#include "ak/chalk.h"

namespace ak::chalk {

namespace {

auto replaceClose (size_t index, const std::string &str, const std::string &close, const std::string &replace) -> std::string {
  std::string head = str.substr(0, index) + replace;
  std::string tail = str.substr(index + close.length());
  size_t next = tail.find(close);
  return head + (next == str.npos ? tail : replaceClose(next, tail, close, replace));
}

auto filterEmpty (const std::string &open, const std::string &close, const std::string &str, const std::string &replace) -> std::string {
  if (str.empty()) return str;
  size_t pos = str.find(close, open.length() + 1);
  if (pos == str.npos) return open + str + close;
  return open + replaceClose(pos, str, close, replace) + close;
}
auto filterEmpty (const std::string &open, const std::string &close, const std::string &str) -> std::string {
  return filterEmpty(open, close, str, open);
}

} // namespace

#include "_internals/chalk.inc"

} // namespace ak::chalk

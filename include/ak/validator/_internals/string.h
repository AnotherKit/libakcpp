/**
 * validator/_internals/string.h - string validators
 *
 * this file implements these validators:
 * - toInclude
 * - toBeConsistedOf
 * - toMatch
 * - toPartiallyMatch
 * - toBeOfLength
 * - toBeShorterThan
 * - toBeLongerThan
 */

#ifndef AK_LIB_VALIDATOR_INTERNALS_STRING_H_
#define AK_LIB_VALIDATOR_INTERNALS_STRING_H_

#include <string>
#include <regex>

#include "../base.h"
#include "common.h"

namespace ak::validator::_internals {

template <>
class Validator<std::string> {
 private:
  using T = std::string;
  const T &value_;
  bool inverse_ = false;
 public:
  Validator () = delete;
  Validator (const T &value);

  const Validator &toBe (const T &value) const;
  const Validator &toBeOneOf (const std::initializer_list<T> &list) const;

  const Validator &toBeLessThan (const T &value) const;
  const Validator &toBeGreaterThan (const T &value) const;

  Validator Not () const;
  Validator butNot () const;

  /// expect(std::string("Hello World")).toInclude("Hello")
  const Validator &toInclude (const std::string &substr) const;
  /// expect(std::string("ababbbbbaaaababab")).toBeConsistedOf("ab")
  const Validator &toBeConsistedOf (const std::string &chars) const;
  /// expect(std::string("Hello World")).toMatch(R"(Hel+o\s\w+)")
  const Validator &toMatch (const std::regex &pattern) const;
  const Validator &toMatch (const std::string &pattern) const;
  /// expect(std::string("Hello World")).toPartiallyMatch(R"(Hel+o)")
  const Validator &toPartiallyMatch (const std::regex &pattern) const;
  const Validator &toPartiallyMatch (const std::string &pattern) const;
  /// expect(std::string("Hello World")).toBeShorterThan(20)
  const Validator &toBeShorterThan (size_t length) const;
  /// expect(std::string("Hello World")).toBeLongerThan(10)
  const Validator &toBeLongerThan (size_t length) const;
};

} // namespace ak::validator::_internals

#endif

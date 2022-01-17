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

  auto toBe (const T &value) const -> const Validator &;
  auto toBeOneOf (const std::initializer_list<T> &list) const -> const Validator &;

  auto toBeLessThan (const T &value) const -> const Validator &;
  auto toBeGreaterThan (const T &value) const -> const Validator &;

  Validator Not () const;
  Validator butNot () const;

  /// expect(std::string("Hello World")).toInclude("Hello")
  auto toInclude (const std::string &substr) const -> const Validator &;
  /// expect(std::string("ababbbbbaaaababab")).toBeConsistedOf("ab")
  auto toBeConsistedOf (const std::string &chars) const -> const Validator &;
  /// expect(std::string("Hello World")).toMatch(R"(Hel+o\s\w+)")
  auto toMatch (const std::regex &pattern) const -> const Validator &;
  auto toMatch (const std::string &pattern) const -> const Validator &;
  /// expect(std::string("Hello World")).toPartiallyMatch(R"(Hel+o)")
  auto toPartiallyMatch (const std::regex &pattern) const -> const Validator &;
  auto toPartiallyMatch (const std::string &pattern) const -> const Validator &;
  /// expect(std::string("Hello World")).toBeOfLength(11);
  auto toBeOfLength (size_t length) const -> const Validator &;
  /// expect(std::string("Hello World")).toBeShorterThan(20)
  auto toBeShorterThan (size_t length) const -> const Validator &;
  /// expect(std::string("Hello World")).toBeLongerThan(10)
  auto toBeLongerThan (size_t length) const -> const Validator &;
};

} // namespace ak::validator::_internals

#endif

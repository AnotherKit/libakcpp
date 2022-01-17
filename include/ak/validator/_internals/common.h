/**
 * validator/_internals/common.h - common methods and utilities for all validators
 *
 * this file implements these validators:
 * - toBe
 * - toBeOneOf
 * - toBeLessThan
 * - toBeGreaterThan
 */

#ifndef AK_LIB_VALIDATOR_INTERNALS_COMMON_H_
#define AK_LIB_VALIDATOR_INTERNALS_COMMON_H_

#include <initializer_list>

#include "../../base.h"
#include "../../compare.h"
#include "../base.h"
#include "validator.h"

namespace ak::validator::_internals {

auto validate (bool t, bool inverse) -> void;

template <typename T>
class Common {
 public:
  static auto toBe (const T &lhs, const T &rhs, bool inverse) -> void {
    validate(equals(lhs, rhs), inverse);
  }
  /// expect(1).toBeOneOf({ 1, 2, 3 })
  static auto toBeOneOf (const T &lhs, const std::initializer_list<T> &rhs, bool inverse) -> void {
    bool includes = false;
    for (const auto &el : rhs) if (equals(lhs, el)) {
      includes = true;
      break;
    }
    validate(includes, inverse);
  }
  static auto toBeLessThan (const T &lhs, const T &rhs, bool inverse) -> void {
    validate(lhs < rhs, inverse);
  }
  static auto toBeGreaterThan (const T &lhs, const T &rhs, bool inverse) -> void {
    validate(rhs < lhs, inverse);
  }
};

template <typename T>
class Validator {
 private:
  const T &value_;
  bool inverse_ = false;
 public:
  Validator () = delete;
  Validator (const T &value) : value_(value) {}

  auto toBe (const T &value) const -> const Validator & {
    Common<T>::toBe(value_, value, inverse_);
    return *this;
  }
  auto toBeOneOf (const std::initializer_list<T> &list) const -> const Validator & {
    Common<T>::toBeOneOf(value_, list, inverse_);
    return *this;
  }

  auto toBeLessThan (const T &value) const -> const Validator & {
    Common<T>::toBeLessThan(value_, value, inverse_);
    return *this;
  }
  auto toBeGreaterThan (const T &value) const -> const Validator & {
    Common<T>::toBeGreaterThan(value_, value, inverse_);
    return *this;
  }

  /// expect(2).Not().toBe(3)
  auto Not () const -> Validator {
    Validator v(value_);
    v.inverse_ = true;
    return v;
  }
  auto butNot () const -> Validator { return Not(); }
};

} // namespace ak::validator::_internals

#endif

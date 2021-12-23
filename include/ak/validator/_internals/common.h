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

void validate (bool t, bool inverse);

template <typename T>
class Common {
 public:
  static void toBe (const T &lhs, const T &rhs, bool inverse) {
    validate(equals(lhs, rhs), inverse);
  }
  /// expect(1).toBeOneOf({ 1, 2, 3 })
  static void toBeOneOf (const T &lhs, const std::initializer_list<T> &rhs, bool inverse) {
    bool includes = false;
    for (const auto &el : rhs) if (equals(lhs, el)) {
      includes = true;
      break;
    }
    validate(includes, inverse);
  }
  static void toBeLessThan (const T &lhs, const T &rhs, bool inverse) {
    validate(lhs < rhs, inverse);
  }
  static void toBeGreaterThan (const T &lhs, const T &rhs, bool inverse) {
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

  const Validator &toBe (const T &value) const {
    Common<T>::toBe(value_, value, inverse_);
    return *this;
  }
  const Validator &toBeOneOf (const std::initializer_list<T> &list) const {
    Common<T>::toBeOneOf(value_, list, inverse_);
    return *this;
  }

  const Validator &toBeLessThan (const T &value) const {
    Common<T>::toBeLessThan(value_, value, inverse_);
    return *this;
  }
  const Validator &toBeGreaterThan (const T &value) const {
    Common<T>::toBeGreaterThan(value_, value, inverse_);
    return *this;
  }

  /// expect(2).Not().toBe(3)
  Validator Not () const {
    Validator v(value_);
    v.inverse_ = true;
    return v;
  }
  Validator butNot () const { return Not(); }
};

} // namespace ak::validator::_internals

#endif

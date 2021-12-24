#include "../../../../include/ak/validator/_internals/string.h"

namespace ak::validator::_internals {

Validator<std::string>::Validator (const T &value) : value_(value) {}

const Validator<std::string> &Validator<std::string>::toBe (const T &value) const {
  Common<T>::toBe(value_, value, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toBeOneOf (const std::initializer_list<T> &list) const {
  Common<T>::toBeOneOf(value_, list, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toBeLessThan (const T &value) const {
  Common<T>::toBeLessThan(value_, value, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toBeGreaterThan (const T &value) const {
  Common<T>::toBeGreaterThan(value_, value, inverse_);
  return *this;
}

Validator<std::string> Validator<std::string>::Not () const {
  Validator v(value_);
  v.inverse_ = true;
  return v;
}

Validator<std::string> Validator<std::string>::butNot () const {
  return Not();
}

const Validator<std::string> &Validator<std::string>::toInclude (const std::string &substr) const {
  validate(value_.find(substr) != value_.npos, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toBeConsistedOf (const std::string &chars) const {
  bool consistedOf = true;
  for (const char &c : value_) if (chars.find(c) == chars.npos) {
    consistedOf = false;
    break;
  }
  validate(consistedOf, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toMatch (const std::regex &pattern) const {
  validate(std::regex_match(value_, pattern), inverse_);
  return *this;
}
const Validator<std::string> &Validator<std::string>::toMatch (const std::string &pattern) const {
  return toMatch(std::regex(pattern));
}

const Validator<std::string> &Validator<std::string>::toPartiallyMatch (const std::regex &pattern) const {
  validate(std::regex_search(value_, pattern), inverse_);
  return *this;
}
const Validator<std::string> &Validator<std::string>::toPartiallyMatch (const std::string &pattern) const {
  return toPartiallyMatch(std::regex(pattern));
}

const Validator<std::string> &Validator<std::string>::toBeOfLength (size_t length) const {
  validate(value_.length() == length, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toBeShorterThan (size_t length) const {
  validate(value_.length() < length, inverse_);
  return *this;
}

const Validator<std::string> &Validator<std::string>::toBeLongerThan (size_t length) const {
  validate(value_.length() > length, inverse_);
  return *this;
}

} // namespace ak::validator::_internals


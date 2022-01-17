#include "../../../../include/ak/validator/_internals/string.h"

namespace ak::validator::_internals {

Validator<std::string>::Validator (const T &value) : value_(value) {}

auto Validator<std::string>::toBe (const T &value) const -> const Validator<std::string> & {
  Common<T>::toBe(value_, value, inverse_);
  return *this;
}

auto Validator<std::string>::toBeOneOf (const std::initializer_list<T> &list) const -> const Validator<std::string> & {
  Common<T>::toBeOneOf(value_, list, inverse_);
  return *this;
}

auto Validator<std::string>::toBeLessThan (const T &value) const -> const Validator<std::string> & {
  Common<T>::toBeLessThan(value_, value, inverse_);
  return *this;
}

auto Validator<std::string>::toBeGreaterThan (const T &value) const -> const Validator<std::string> & {
  Common<T>::toBeGreaterThan(value_, value, inverse_);
  return *this;
}

auto Validator<std::string>::Not () const -> Validator<std::string> {
  Validator v(value_);
  v.inverse_ = true;
  return v;
}

auto Validator<std::string>::butNot () const -> Validator<std::string> {
  return Not();
}

auto Validator<std::string>::toInclude (const std::string &substr) const -> const Validator<std::string> & {
  validate(value_.find(substr) != value_.npos, inverse_);
  return *this;
}

auto Validator<std::string>::toBeConsistedOf (const std::string &chars) const -> const Validator<std::string> & {
  bool consistedOf = true;
  for (const char &c : value_) if (chars.find(c) == chars.npos) {
    consistedOf = false;
    break;
  }
  validate(consistedOf, inverse_);
  return *this;
}

auto Validator<std::string>::toMatch (const std::regex &pattern) const -> const Validator<std::string> & {
  validate(std::regex_match(value_, pattern), inverse_);
  return *this;
}
auto Validator<std::string>::toMatch (const std::string &pattern) const -> const Validator<std::string> & {
  return toMatch(std::regex(pattern));
}

auto Validator<std::string>::toPartiallyMatch (const std::regex &pattern) const -> const Validator<std::string> & {
  validate(std::regex_search(value_, pattern), inverse_);
  return *this;
}
auto Validator<std::string>::toPartiallyMatch (const std::string &pattern) const -> const Validator<std::string> & {
  return toPartiallyMatch(std::regex(pattern));
}

auto Validator<std::string>::toBeOfLength (size_t length) const -> const Validator<std::string> & {
  validate(value_.length() == length, inverse_);
  return *this;
}

auto Validator<std::string>::toBeShorterThan (size_t length) const -> const Validator<std::string> & {
  validate(value_.length() < length, inverse_);
  return *this;
}

auto Validator<std::string>::toBeLongerThan (size_t length) const -> const Validator<std::string> & {
  validate(value_.length() > length, inverse_);
  return *this;
}

} // namespace ak::validator::_internals


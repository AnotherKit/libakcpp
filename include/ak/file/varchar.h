#ifndef AK_LIB_FILE_VARCHAR_H_
#define AK_LIB_FILE_VARCHAR_H_

#include <string.h>

#include <compare>
#include <string>

#include "../base.h"

namespace ak::file {
/// a wrapper for const char * with utility functions and type conversions.
template <int maxLength>
struct Varchar {
 private:
  template <int A>
  friend class Varchar;
  char content[maxLength + 1]; // NOLINT(modernize-avoid-c-arrays): it's clear that we need to use C-style array here.
 public:
  Varchar () { content[0] = '\0'; }
  Varchar (const std::string &s) {
    if (s.length() > maxLength) throw Overflow("Varchar length overflow");
    strcpy(content, s.c_str());
  }
  Varchar (const char *cstr) : Varchar(std::string(cstr)) {}
  template<int A>
  Varchar (const Varchar<A> &that) { *this = that; }
  operator std::string () const { return std::string(content); }
  [[nodiscard]] std::string str () const { return std::string(*this); }
  template <int A>
  Varchar operator= (const Varchar<A> &that) {
    if (that.str().length() > maxLength) throw Overflow("Varchar length overflow");
    strcpy(content, that.content);
    return *this;
  }
  template <int A>
  std::weak_ordering operator<=> (const Varchar<A> &that) const {
    int res = strcmp(content, that.content);
    if (res < 0) return std::weak_ordering::less;
    if (res > 0) return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }
  template <int A>
  bool operator== (const Varchar<A> &that) const { return (*this <=> that) == std::weak_ordering::equivalent; };
  template <int A>
  bool operator!= (const Varchar<A> &that) const { return !(*this == that); }
};
} // namespace ak::file

#endif

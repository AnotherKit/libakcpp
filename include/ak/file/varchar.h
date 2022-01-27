#ifndef AK_LIB_FILE_VARCHAR_H_
#define AK_LIB_FILE_VARCHAR_H_

#include <string.h>

#include <compare>
#include <string>

#include "ak/base.h"

namespace ak::file {
/// a wrapper for const char * with utility functions and type conversions.
template <int maxLength>
struct Varchar {
 private:
  template <int A>
  friend class Varchar;
  char content[maxLength + 1];
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
  [[nodiscard]] auto str () const -> std::string { return std::string(*this); }
  template <int A>
  auto operator= (const Varchar<A> &that) -> Varchar {
    if (that.str().length() > maxLength) throw Overflow("Varchar length overflow");
    strcpy(content, that.content);
    return *this;
  }
  template <int A>
  auto operator<=> (const Varchar<A> &that) const -> std::weak_ordering {
    int res = strcmp(content, that.content);
    if (res < 0) return std::weak_ordering::less;
    if (res > 0) return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }
  template <int A>
  auto operator== (const Varchar<A> &that) const -> bool { return (*this <=> that) == std::weak_ordering::equivalent; };
  template <int A>
  auto operator!= (const Varchar<A> &that) const -> bool { return !(*this == that); }
};
} // namespace ak::file

#endif

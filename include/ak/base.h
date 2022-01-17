/**
 * base.h - base code and classes for libakcpp
 * this file defines common utility functions and exceptions.
 * almost all libakcpp code should include this header.
 */

#ifndef AK_LIB_BASE_H_
#define AK_LIB_BASE_H_

#include <exception>

#ifdef AK_DEBUG
#include <assert.h>
#include <iostream>
#define AK_ASSERT(x) assert(x)
#define AK_DEBUGONLY(x) (x)
#else
#define AK_ASSERT(x)
#define AK_DEBUGONLY(x)
#endif
#define AK_LOG(x) AK_DEBUGONLY(std::cerr << (x) << std::endl)

namespace ak {
class Exception : public std::exception {
 public:
  const char *message;
  explicit Exception (const char *message) : message(message) {}
  [[nodiscard]] auto what () const noexcept -> const char * override;
  ~Exception () override;
};

class IOException : public Exception {
 public:
  explicit IOException (const char *message) : Exception(message) {}
};

class NotFound : public Exception {
 public:
  explicit NotFound (const char *message) : Exception(message) {}
};
class OutOfBounds : public Exception {
 public:
  explicit OutOfBounds (const char *message) : Exception(message) {}
};
class Overflow : public OutOfBounds {
 public:
  explicit Overflow (const char *message) : OutOfBounds(message) {}
};
class Underflow : public OutOfBounds {
 public:
  explicit Underflow (const char *message) : OutOfBounds(message) {}
};
} // namespace ak

#endif

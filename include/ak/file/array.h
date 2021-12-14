#ifndef AK_LIB_FILE_ARRAY_H_
#define AK_LIB_FILE_ARRAY_H_

#include <string.h>

#include <functional>

#include "../base.h"

namespace ak::file {
/// an array with utility functions and bound checks.
template <typename T, size_t maxLength>
struct Array {
 private:
  void boundsCheck_ (size_t index) {
    if (index >= length) throw Overflow("Array: overflow");
    if (index < 0) throw Underflow("Array: underflow");
  }
 public:
  Array () = default;
  size_t length = 0;
  T content[maxLength]; // NOLINT(modernize-avoid-c-arrays): it's clear that we need to use C-style array here.
  size_t indexOf (const T &element) {
    for (size_t i = 0; i < length; ++i) if (element == content[i]) return i;
    throw NotFound("Array::indexOf: element not found");
  }
  void insert (const T &element, size_t offset) {
    if (offset != length) boundsCheck_(offset);
    if (length == maxLength - 1) throw Overflow("Array::insert: overflow");
    if (offset != length) memmove(&content[offset + 1], &content[offset], (length - offset) * sizeof(content[0]));
    ++length;
  }

  void remove (const T &element) { removeAt(indexOf(element)); }
  void removeAt (size_t offset) {
    boundsCheck_(offset);
    if (offset != length - 1) memmove(&content[offset], &content[offset + 1], (length - offset - 1) * sizeof(content[0]));
    --length;
  }

  void copyFrom (const Array &other, size_t fromIndex, size_t toIndex, size_t count) {
    if (this == &other) memmove(&content[toIndex], &content[fromIndex], count * sizeof(content[0]));
    else memcpy(&content[toIndex], &other.content[fromIndex], count * sizeof(content[0]));
  }

  T &operator[] (size_t index) { boundsCheck_(index); return content[index]; }
  const T &operator[] (size_t index) const { boundsCheck_(index); return content[index]; }

  T pop () {
    if (length == 0) throw Underflow("Set::pop: underflow");
    return content[--length];
  }
  T shift () {
    if (length == 0) throw Underflow("Set::pop: underflow");
    T result = content[0];
    removeAt(0);
    return result;
  }
  void push (const T &object) { insert(object, length); }
  void unshift (const T &object) { insert(object, 0); }

  void forEach (const std::function<void (const T &element)> &callback) {
    for (int i = 0; i < length; ++i) callback(content[i]);
  }
};
} // namespace ak::file

#endif

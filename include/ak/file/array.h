#ifndef AK_LIB_FILE_ARRAY_H_
#define AK_LIB_FILE_ARRAY_H_

#include <string.h>

#include <functional>

#include "ak/base.h"

namespace ak::file {
/// an array with utility functions and bound checks.
template <typename T, size_t maxLength>
struct Array {
 private:
  auto boundsCheck_ (size_t index) -> void {
    if (index >= length) throw OutOfBounds("Array: overflow or underflow");
  }
 public:
  Array () = default;
  size_t length = 0;
  T content[maxLength];
  auto indexOf (const T &element) -> size_t {
    for (size_t i = 0; i < length; ++i) if (equals(element, content[i])) return i;
    throw NotFound("Array::indexOf: element not found");
  }
  auto includes (const T &element) -> bool {
    for (size_t i = 0; i < length; ++i) if (equals(element, content[i])) return true;
    return false;
  }
  auto insert (const T &element, size_t offset) -> void {
    if (offset != length) boundsCheck_(offset);
    if (length == maxLength) throw Overflow("Array::insert: overflow");
    if (offset != length) memmove(&content[offset + 1], &content[offset], (length - offset) * sizeof(content[0]));
    content[offset] = element;
    ++length;
  }

  auto remove (const T &element) -> void { removeAt(indexOf(element)); }
  auto removeAt (size_t offset) -> void {
    boundsCheck_(offset);
    if (offset != length - 1) memmove(&content[offset], &content[offset + 1], (length - offset - 1) * sizeof(content[0]));
    --length;
  }
  auto clear () -> void { length = 0; }

  auto copyFrom (const Array &other, size_t fromIndex, size_t toIndex, size_t count) -> void {
    if (this == &other) memmove(&content[toIndex], &content[fromIndex], count * sizeof(content[0]));
    else memcpy(&content[toIndex], &other.content[fromIndex], count * sizeof(content[0]));
  }

  auto operator[] (size_t index) -> T & { boundsCheck_(index); return content[index]; }
  auto operator[] (size_t index) -> T & { boundsCheck_(index); return content[index]; }

  auto pop () -> T {
    if (length == 0) throw Underflow("Set::pop: underflow");
    return content[--length];
  }
  auto shift () -> T {
    if (length == 0) throw Underflow("Set::pop: underflow");
    T result = content[0];
    removeAt(0);
    return result;
  }
  auto push (const T &object) -> T { insert(object, length); }
  auto unshift (const T &object) -> T { insert(object, 0); }

  auto forEach (const std::function<void (const T &element)> &callback) -> T {
    for (size_t i = 0; i < length; ++i) callback(content[i]);
  }
};
} // namespace ak::file

#endif

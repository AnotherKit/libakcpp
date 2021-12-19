#ifndef AK_LIB_FILE_SET_H_
#define AK_LIB_FILE_SET_H_

#include <string.h>

#include <algorithm>

#include "../base.h"

// FIXME: remove dupe code of Set and Array. does C++ support mixins?
namespace ak::file {
/// a sorted array with utility functions and bound checks.
template <typename T, size_t maxLength>
struct Set {
 private:
  void boundsCheck_ (size_t index) {
    if (index >= length) throw OutOfBounds("Set: overflow or underflow");
  }
 public:
  Set () = default;
  size_t length = 0;
  T content[maxLength]; // NOLINT(modernize-avoid-c-arrays): it's clear that we need to use C-style array here.
  size_t indexOfInsert (const T &element) {
    return std::lower_bound(content, content + length, element) - content;
  }
  size_t indexOf (const T &element) {
    size_t index = indexOfInsert(element);
    if (index >= length || content[index] != element) throw NotFound("Set::indexOf: element not found");
    return index;
  }
  bool includes (const T &element) {
    return content[indexOfInsert(element)] == element;
  }
  void insert (const T &element) {
    if (length == maxLength) throw Overflow("Set::insert: overflow");
    size_t offset = indexOfInsert(element);
    if (offset != length) memmove(&content[offset + 1], &content[offset], (length - offset) * sizeof(content[0]));
    content[offset] = element;
    ++length;
  }

  void remove (const T &element) { removeAt(indexOf(element)); }
  void removeAt (size_t offset) {
    boundsCheck_(offset);
    if (offset != length - 1) memmove(&content[offset], &content[offset + 1], (length - offset - 1) * sizeof(content[0]));
    --length;
  }
  void clear () { length = 0; }

  void copyFrom (const Set &other, size_t fromIndex, size_t toIndex, size_t count) {
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

  void forEach (const std::function<void (const T &element)> &callback) {
    for (int i = 0; i < length; ++i) callback(content[i]);
  }
};
} // namespace ak::file

#endif

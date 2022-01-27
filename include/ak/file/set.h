#ifndef AK_LIB_FILE_SET_H_
#define AK_LIB_FILE_SET_H_

#include <string.h>

#include <algorithm>

#include "ak/base.h"
#include "ak/compare.h"

// FIXME: remove dupe code of Set and Array. does C++ support mixins?
namespace ak::file {
/// a sorted array with utility functions and bound checks.
template <typename T, size_t maxLength>
struct Set {
 private:
  auto boundsCheck_ (size_t index) -> void {
    if (index >= length) throw OutOfBounds("Set: overflow or underflow");
  }
 public:
  Set () = default;
  size_t length = 0;
  T content[maxLength];
  auto indexOfInsert (const T &element) -> size_t {
    return std::lower_bound(content, content + length, element) - content;
  }
  auto indexOf (const T &element) -> size_t {
    size_t index = indexOfInsert(element);
    if (index >= length || !equals(content[index], element)) throw NotFound("Set::indexOf: element not found");
    return index;
  }
  auto includes (const T &element) -> bool {
    size_t ix = indexOfInsert(element);
    return ix < length && equals(content[ix], element);
  }
  auto insert (const T &element) -> void {
    if (length == maxLength) throw Overflow("Set::insert: overflow");
    size_t offset = indexOfInsert(element);
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

  auto copyFrom (const Set &other, size_t fromIndex, size_t toIndex, size_t count) -> void {
    if (this == &other) memmove(&content[toIndex], &content[fromIndex], count * sizeof(content[0]));
    else memcpy(&content[toIndex], &other.content[fromIndex], count * sizeof(content[0]));
  }

  auto &operator[] (size_t index) -> T { boundsCheck_(index); return content[index]; }
  auto operator[] (size_t index) const -> const T & { boundsCheck_(index); return content[index]; }

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

  auto forEach (const std::function<void (const T &element)> &callback) -> void {
    for (int i = 0; i < length; ++i) callback(content[i]);
  }
};
} // namespace ak::file

#endif

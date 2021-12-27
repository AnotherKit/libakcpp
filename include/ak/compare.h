/**
 * compare.h - comparison helpers.
 */

#ifndef AK_LIB_COMPARE_H_
#define AK_LIB_COMPARE_H_

namespace ak {

template <typename A, typename B>
bool equals (const A &lhs, const B &rhs) {
  return !(lhs < rhs || rhs < lhs);
}

} // namespace ak

#endif

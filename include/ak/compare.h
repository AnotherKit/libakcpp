/**
 * compare.h - comparison helpers.
 */

#ifndef AK_LIB_COMPARE_H_
#define AK_LIB_COMPARE_H_

#include <concepts>

namespace ak {

template <typename A, typename B = A>
concept Comparable = requires(A a, B b) {
  { a < b } -> std::same_as<bool>;
};

template <typename A, typename B> requires Comparable<A, B>
auto equals (const A &lhs, const B &rhs) -> bool {
  return !(lhs < rhs || rhs < lhs);
}

} // namespace ak

#endif

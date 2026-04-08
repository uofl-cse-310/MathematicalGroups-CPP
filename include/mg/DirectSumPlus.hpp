#ifndef MG_DIRECT_SUM_PLUS_HPP
#define MG_DIRECT_SUM_PLUS_HPP

#include <mg/DirectSum.hpp>
#include <mg/GroupConcept.hpp>

namespace mg {

template <class L, class R>
  requires GroupConcept<L> && GroupConcept<R>
constexpr DirectSum<L, R> operator+(const L& lhs, const R& rhs) {
  return DirectSum<L, R>(lhs, rhs);
}

} // namespace mg

#endif // MG_DIRECT_SUM_PLUS_HPP


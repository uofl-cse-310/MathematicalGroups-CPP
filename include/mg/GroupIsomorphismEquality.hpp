#ifndef MG_GROUP_ISOMORPHISM_EQUALITY_HPP
#define MG_GROUP_ISOMORPHISM_EQUALITY_HPP

#include <mg/GroupConcept.hpp>
#include <mg/Isomorphism.hpp>

namespace mg {

template <GroupConcept GA, GroupConcept GB>
bool operator==(const GA& a, const GB& b) {
  return isIsomorphicTo(a, b);
}

template <GroupConcept GA, GroupConcept GB>
bool operator!=(const GA& a, const GB& b) {
  return !isIsomorphicTo(a, b);
}

} // namespace mg

#endif // MG_GROUP_ISOMORPHISM_EQUALITY_HPP


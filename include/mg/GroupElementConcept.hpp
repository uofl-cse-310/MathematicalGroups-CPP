#ifndef MG_GROUP_ELEMENT_CONCEPT_HPP
#define MG_GROUP_ELEMENT_CONCEPT_HPP
#include <concepts>

namespace mg {

template <class T>
concept GroupElementConcept = requires(const T a, const T b) {
  { a + b } -> std::same_as<T>;
  { a - b } -> std::same_as<T>;
  { -a } -> std::same_as<T>;
  { a == b } -> std::convertible_to<bool>;

  // Identity as a static factory (works well for types like ZmodN<N>).
  { T::identity() } -> std::same_as<T>;
};

} // namespace mg

#endif // MG_GROUP_ELEMENT_CONCEPT_HPP

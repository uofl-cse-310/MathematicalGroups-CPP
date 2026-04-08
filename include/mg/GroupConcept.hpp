#ifndef MG_GROUP_CONCEPT_HPP
#define MG_GROUP_CONCEPT_HPP
#include <concepts>
#include <cstddef>

namespace mg {

template <class G>
concept GroupConcept = requires(G g, const G cg, std::size_t n, std::size_t i,
                                typename G::pointer a, typename G::pointer b,
                                typename G::pointer r) {
  typename G::value_type;
  typename G::pointer;

  { g.setOrder(n) } -> std::same_as<void>;
  { cg.order() } -> std::same_as<std::size_t>;

  { g.setElement(i, a) } -> std::same_as<void>;
  { cg.element(i) } -> std::same_as<typename G::pointer>;

  { g.setOperation(a, b, r) } -> std::same_as<void>;
  { cg.operate(a, b) } -> std::same_as<typename G::pointer>;
};

} // namespace mg

#endif // MG_GROUP_CONCEPT_HPP

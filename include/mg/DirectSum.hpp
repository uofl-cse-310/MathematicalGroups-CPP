#ifndef MG_DIRECT_SUM_HPP
#define MG_DIRECT_SUM_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include <mg/GroupConcept.hpp>

namespace mg {

template <GroupConcept G1, GroupConcept G2>
class DirectSum {
public:
  using left_group_type = G1;
  using right_group_type = G2;

  using left_pointer = typename G1::pointer;
  using right_pointer = typename G2::pointer;

  struct value_type {
    left_pointer first = nullptr;
    right_pointer second = nullptr;
  };

  using pointer = value_type*;

  DirectSum(const G1& g1, const G2& g2) : g1_(g1), g2_(g2) { buildElements_(); }

  std::size_t order() const { return elements_.size(); }

  pointer element(std::size_t i) const {
    if (i >= elements_.size()) throw std::out_of_range("element index out of range");
    return elements_[i];
  }

  void setOrder(std::size_t) { throw std::logic_error("DirectSum order is derived from factors"); }
  void setElement(std::size_t, pointer) { throw std::logic_error("DirectSum elements are derived from factors"); }
  void setOperation(pointer, pointer, pointer) {
    throw std::logic_error("DirectSum operation is derived from factors");
  }

  pointer operate(pointer a, pointer b) const {
    if (a == nullptr || b == nullptr) throw std::invalid_argument("null element pointer");
    if (a->first == nullptr || a->second == nullptr || b->first == nullptr || b->second == nullptr)
      throw std::invalid_argument("null component pointer");

    auto* out = new value_type{g1_.operate(a->first, b->first), g2_.operate(a->second, b->second)};
    results_.push_back(out);
    return out;
  }

  ~DirectSum() {
    for (auto* p : elements_) delete p;
    for (auto* p : results_) delete p;
  }

  DirectSum(const DirectSum&) = delete;
  DirectSum& operator=(const DirectSum&) = delete;

  DirectSum(DirectSum&& other) noexcept
      : g1_(other.g1_), g2_(other.g2_), elements_(std::move(other.elements_)),
        results_(std::move(other.results_)) {
    other.elements_.clear();
    other.results_.clear();
  }

  DirectSum& operator=(DirectSum&&) = delete;

private:
  const G1& g1_;
  const G2& g2_;

  std::vector<pointer> elements_{};
  mutable std::vector<pointer> results_{};

  void buildElements_() {
    const auto n1 = g1_.order();
    const auto n2 = g2_.order();
    if (n1 == 0 || n2 == 0) throw std::logic_error("both groups must have nonzero order");

    elements_.reserve(n1 * n2);
    for (std::size_t i = 0; i < n1; ++i) {
      for (std::size_t j = 0; j < n2; ++j) {
        auto* a = g1_.element(i);
        auto* b = g2_.element(j);
        if (a == nullptr || b == nullptr) throw std::logic_error("factor group has null element");
        elements_.push_back(new value_type{a, b});
      }
    }
  }
};

template <GroupConcept G1, GroupConcept G2>
DirectSum<G1, G2> operator+(const G1& g1, const G2& g2) {
  return DirectSum<G1, G2>(g1, g2);
}

} // namespace mg

#endif // MG_DIRECT_SUM_HPP

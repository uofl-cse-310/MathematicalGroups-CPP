#ifndef MG_DIRECT_SUM_HPP
#define MG_DIRECT_SUM_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <mg/GroupConcept.hpp>
#include <mg/Isomorphism.hpp>

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

  DirectSum(const G1& g1, const G2& g2) : g1_(g1), g2_(g2) { build_(); }

  std::size_t order() const { return elements_.size(); }

  pointer element(std::size_t i) const {
    if (i >= elements_.size()) throw std::out_of_range("element index out of range");
    return elements_[i].get();
  }

  void setOrder(std::size_t) { throw std::logic_error("DirectSum order is derived from factors"); }
  void setElement(std::size_t, pointer) { throw std::logic_error("DirectSum elements are derived from factors"); }
  void setOperation(pointer, pointer, pointer) {
    throw std::logic_error("DirectSum operation is derived from factors");
  }

  pointer operate(pointer a, pointer b) const {
    if (a == nullptr || b == nullptr) throw std::invalid_argument("null element pointer");
    const auto itA = index_.find(key_(a));
    const auto itB = index_.find(key_(b));
    if (itA == index_.end() || itB == index_.end())
      throw std::logic_error("DirectSum::operate called with pointer not owned by this DirectSum");

    const auto ai = itA->second;
    const auto bi = itB->second;
    return element(opIndex_[ai][bi]);
  }

  ~DirectSum() = default;

  DirectSum(const DirectSum&) = default;
  DirectSum& operator=(const DirectSum&) = delete;

  DirectSum(DirectSum&&) noexcept = default;
  DirectSum& operator=(DirectSum&&) = delete;

private:
  const G1& g1_;
  const G2& g2_;

  struct Key {
    left_pointer a{};
    right_pointer b{};
    bool operator==(const Key& o) const noexcept { return a == o.a && b == o.b; }
  };
  struct KeyHash {
    std::size_t operator()(const Key& k) const noexcept {
      const auto h1 = std::hash<const void*>{}(static_cast<const void*>(k.a));
      const auto h2 = std::hash<const void*>{}(static_cast<const void*>(k.b));
      return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
    }
  };

  static Key key_(const value_type* v) {
    if (v == nullptr) return {};
    return Key{v->first, v->second};
  }

  std::vector<std::unique_ptr<value_type>> elements_{};
  std::unordered_map<Key, std::size_t, KeyHash> index_{};
  std::vector<std::vector<std::size_t>> opIndex_{};

  void build_() {
    const auto n1 = g1_.order();
    const auto n2 = g2_.order();
    if (n1 == 0 || n2 == 0) throw std::logic_error("both groups must have nonzero order");

    elements_.reserve(n1 * n2);
    for (std::size_t i = 0; i < n1; ++i) {
      for (std::size_t j = 0; j < n2; ++j) {
        auto* a = g1_.element(i);
        auto* b = g2_.element(j);
        if (a == nullptr || b == nullptr) throw std::logic_error("factor group has null element");
        elements_.push_back(std::make_unique<value_type>(value_type{a, b}));
      }
    }

    index_.reserve(elements_.size());
    for (std::size_t i = 0; i < elements_.size(); ++i) {
      index_.emplace(Key{elements_[i]->first, elements_[i]->second}, i);
    }

    const auto n = elements_.size();
    opIndex_.assign(n, std::vector<std::size_t>(n, 0));

    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        const auto& x = *elements_[i];
        const auto& y = *elements_[j];
        auto* r1 = g1_.operate(x.first, y.first);
        auto* r2 = g2_.operate(x.second, y.second);
        const auto it = index_.find(Key{r1, r2});
        if (it == index_.end())
          throw std::logic_error(
              "factor operate() returned pointer not present in factor element() list");
        opIndex_[i][j] = it->second;
      }
    }
  }
};

} // namespace mg

#include <mg/DirectSumPlus.hpp>

#endif // MG_DIRECT_SUM_HPP

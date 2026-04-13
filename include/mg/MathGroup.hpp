#ifndef MG_MATHGROUP_HPP
#define MG_MATHGROUP_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <mg/Isomorphism.hpp>

namespace mg {

template <class T>
class MathGroup {
public:
  using value_type = T;
  using pointer = T*;

  MathGroup() = default;

  void setOrder(std::size_t n) {
    order_ = n;
    elements_.assign(n, nullptr);
    index_.clear();
    op_.clear();
  }

  std::size_t order() const { return order_; }

  void setElement(std::size_t i, pointer p) {
    if (i >= order_) throw std::out_of_range("element index out of range");
    if (p == nullptr) throw std::invalid_argument("null element pointer");
    elements_[i] = p;
    index_[p] = i;
  }

  pointer element(std::size_t i) const {
    if (i >= order_) throw std::out_of_range("element index out of range");
    return elements_[i];
  }

  void setOperation(pointer a, pointer b, pointer result) {
    requireKnown_(a);
    requireKnown_(b);
    requireKnown_(result);
    op_[key_(a, b)] = result;
  }

  pointer operate(pointer a, pointer b) const {
    requireKnown_(a);
    requireKnown_(b);

    auto it = op_.find(key_(a, b));
    if (it == op_.end()) throw std::logic_error("operation not defined for pair");
    return it->second;
  }

  template <class Other>
    requires GroupConcept<Other>
  bool isIsomorphicTo(const Other& other) const {
    return mg::isIsomorphicTo(*this, other);
  }

private:
  std::size_t order_ = 0;
  std::vector<pointer> elements_{};
  std::unordered_map<pointer, std::size_t> index_{};

  using Key = std::uint64_t;
  std::unordered_map<Key, pointer> op_{};

  static constexpr std::size_t kUnassigned_ = std::numeric_limits<std::size_t>::max();

  void requireKnown_(pointer p) const {
    if (p == nullptr) throw std::invalid_argument("null element pointer");
    if (index_.find(p) == index_.end())
      throw std::invalid_argument("pointer not registered as element");
  }

  Key key_(pointer a, pointer b) const {
    const auto ia = static_cast<std::uint64_t>(index_.at(a));
    const auto ib = static_cast<std::uint64_t>(index_.at(b));
    return (ia << 32U) | ib;
  }

  std::size_t findIdentityIndex_() const {
    if (order_ == 0) throw std::logic_error("empty group has no identity");

    for (std::size_t ei = 0; ei < order_; ++ei) {
      auto* e = element(ei);
      if (e == nullptr) continue;

      bool isId = true;
      for (std::size_t ai = 0; ai < order_ && isId; ++ai) {
        auto* a = element(ai);
        if (a == nullptr) {
          isId = false;
          break;
        }
        if (operate(e, a) != a) isId = false;
        else if (operate(a, e) != a) isId = false;
      }

      if (isId) return ei;
    }

    throw std::logic_error("no identity element found (operation table incomplete or invalid)");
  }

  std::vector<std::size_t> elementOrders_(std::size_t identityIndex) const {
    const auto n = order_;
    std::vector<std::size_t> ord(n, 0);

    auto* e = element(identityIndex);
    if (e == nullptr) throw std::logic_error("identity pointer is null");

    for (std::size_t i = 0; i < n; ++i) {
      auto* a = element(i);
      if (a == nullptr) throw std::logic_error("group element pointer is null");

      if (a == e) {
        ord[i] = 1;
        continue;
      }

      auto* cur = a;
      for (std::size_t k = 1; k <= n; ++k) {
        if (cur == e) {
          ord[i] = k;
          break;
        }
        cur = operate(cur, a);
      }
      if (ord[i] == 0)
        throw std::logic_error(
            "element order did not reach identity within group order (invalid group)");
    }

    return ord;
  }

  static bool sameMultiset_(std::vector<std::size_t> a, std::vector<std::size_t> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
  }
};

} // namespace mg

#endif // MG_MATHGROUP_HPP


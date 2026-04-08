#ifndef MG_MATHGROUP_HPP
#define MG_MATHGROUP_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace mg {

// Runtime-defined finite group.
//
// This is a non-owning container -> it stores pointers to externally-managed
// elements and a pointer-based operation table.
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

  // Group isomorphism check (Cayley-table based).
  // Returns true iff there exists a bijective homomorphism
  bool isIsomorphicTo(const MathGroup& other) const {
    if (order_ != other.order_) return false;
    if (order_ == 0) return true;

    const auto idA = findIdentityIndex_();
    const auto idB = other.findIdentityIndex_();

    const auto ordA = elementOrders_(idA);
    const auto ordB = other.elementOrders_(idB);

    if (!sameMultiset_(ordA, ordB)) return false;

    // Build candidate mapping sets using element orders as an invariant.
    const std::size_t n = order_;
    std::vector<std::vector<std::size_t>> candidates(n);
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        if (ordA[i] == ordB[j]) candidates[i].push_back(j);
      }
      if (candidates[i].empty()) return false;
    }

    // Force identity -> identity.
    if (std::find(candidates[idA].begin(), candidates[idA].end(), idB) == candidates[idA].end())
      return false;

    std::vector<std::size_t> mapAtoB(n, kUnassigned_);
    std::vector<std::size_t> mapBtoA(n, kUnassigned_);
    mapAtoB[idA] = idB;
    mapBtoA[idB] = idA;

    // Variables (indices) to assign, ordered by least candidates first.
    std::vector<std::size_t> vars;
    vars.reserve(n - 1);
    for (std::size_t i = 0; i < n; ++i)
      if (i != idA) vars.push_back(i);

    std::sort(vars.begin(), vars.end(), [&](std::size_t x, std::size_t y) {
      return candidates[x].size() < candidates[y].size();
    });

    return backtrackIsomorphism_(other, candidates, vars, 0, mapAtoB, mapBtoA);
  }

  friend bool operator==(const MathGroup& a, const MathGroup& b) {
    return a.isIsomorphicTo(b);
  }

  friend bool operator!=(const MathGroup& a, const MathGroup& b) {
    return !a.isIsomorphicTo(b);
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
        throw std::logic_error("element order did not reach identity within group order (invalid group)");
    }

    return ord;
  }

  static bool sameMultiset_(std::vector<std::size_t> a, std::vector<std::size_t> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
  }

  // Verify homomorphism constraints implied by already-assigned elements.
  bool respectsConstraints_(const MathGroup& other,
                           const std::vector<std::size_t>& mapAtoB,
                           std::size_t i,
                           std::size_t jCandidate) const {
    auto* ai = element(i);
    auto* bj = other.element(jCandidate);

    for (std::size_t k = 0; k < order_; ++k) {
      const auto fk = mapAtoB[k];
      if (fk == kUnassigned_) continue;

      // If f(i*k) is assigned, enforce f(i)*f(k) = f(i*k).
      {
        auto* prodA = operate(ai, element(k));
        const auto ip = index_.at(prodA);
        const auto fip = mapAtoB[ip];
        if (fip != kUnassigned_) {
          auto* want = other.operate(bj, other.element(fk));
          if (want != other.element(fip)) return false;
        }
      }

      // If f(k*i) is assigned, enforce f(k)*f(i) = f(k*i).
      {
        auto* prodA = operate(element(k), ai);
        const auto ip = index_.at(prodA);
        const auto fip = mapAtoB[ip];
        if (fip != kUnassigned_) {
          auto* want = other.operate(other.element(fk), bj);
          if (want != other.element(fip)) return false;
        }
      }
    }

    return true;
  }

  bool backtrackIsomorphism_(const MathGroup& other,
                            const std::vector<std::vector<std::size_t>>& candidates,
                            const std::vector<std::size_t>& vars,
                            std::size_t pos,
                            std::vector<std::size_t>& mapAtoB,
                            std::vector<std::size_t>& mapBtoA) const {
    if (pos == vars.size()) {
      // bijective and constraints were checked incrementally -> isomorphism found.
      return true;
    }

    const auto i = vars[pos];

    for (const auto j : candidates[i]) {
      if (mapBtoA[j] != kUnassigned_) continue;
      if (!respectsConstraints_(other, mapAtoB, i, j)) continue;

      mapAtoB[i] = j;
      mapBtoA[j] = i;

      if (backtrackIsomorphism_(other, candidates, vars, pos + 1, mapAtoB, mapBtoA)) return true;

      mapAtoB[i] = kUnassigned_;
      mapBtoA[j] = kUnassigned_;
    }

    return false;
  }
};

} // namespace mg

#endif // MG_MATHGROUP_HPP


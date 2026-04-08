#ifndef MG_MATHGROUP_HPP
#define MG_MATHGROUP_HPP
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace mg {

// Runtime-defined finite group.
//
// This is a non-owning container: it stores pointers to externally-managed
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

private:
  std::size_t order_ = 0;
  std::vector<pointer> elements_{};
  std::unordered_map<pointer, std::size_t> index_{};

  using Key = std::uint64_t;
  std::unordered_map<Key, pointer> op_{};

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
};

} // namespace mg


#endif // MG_MATHGROUP_HPP
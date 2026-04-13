#ifndef MG_GROUP_VALIDATION_HPP
#define MG_GROUP_VALIDATION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#include <mg/GroupConcept.hpp>

namespace mg {

namespace detail {

template <GroupConcept G>
std::size_t findIdentityIndexOrThrow(const G& g) {
  const auto n = g.order();
  if (n == 0) throw std::logic_error("empty group has no identity");

  for (std::size_t ei = 0; ei < n; ++ei) {
    auto* e = g.element(ei);
    if (e == nullptr) continue;

    bool isId = true;
    for (std::size_t ai = 0; ai < n && isId; ++ai) {
      auto* a = g.element(ai);
      if (a == nullptr) {
        isId = false;
        break;
      }
      if (g.operate(e, a) != a) isId = false;
      else if (g.operate(a, e) != a) isId = false;
    }

    if (isId) return ei;
  }

  throw std::logic_error("no identity element found (operation table incomplete or invalid)");
}

} // namespace detail

template <GroupConcept G>
void requireClosure(const G& g) {
  const auto n = g.order();
  if (n == 0) return;

  for (std::size_t i = 0; i < n; ++i) {
    if (g.element(i) == nullptr)
      throw std::logic_error("group has null element pointer at index " + std::to_string(i));
  }

  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      auto* r = g.operate(g.element(i), g.element(j));
      if (r == nullptr)
        throw std::logic_error("operate() returned null pointer for pair (" + std::to_string(i) +
                               "," + std::to_string(j) + ")");

      bool found = false;
      for (std::size_t k = 0; k < n; ++k) {
        if (g.element(k) == r) {
          found = true;
          break;
        }
      }
      if (!found)
        throw std::logic_error(
            "operate() returned pointer not present in element() list for pair (" +
            std::to_string(i) + "," + std::to_string(j) + ")");
    }
  }
}

template <GroupConcept G>
void requireIdentity(const G& g) {
  (void)detail::findIdentityIndexOrThrow(g);
}

template <GroupConcept G>
void requireInverses(const G& g) {
  const auto n = g.order();
  if (n == 0) return;

  const auto idIndex = detail::findIdentityIndexOrThrow(g);
  auto* e = g.element(idIndex);
  if (e == nullptr) throw std::logic_error("identity pointer is null");

  for (std::size_t ai = 0; ai < n; ++ai) {
    auto* a = g.element(ai);
    if (a == nullptr) throw std::logic_error("group element pointer is null");

    bool hasInv = false;
    for (std::size_t bi = 0; bi < n; ++bi) {
      auto* b = g.element(bi);
      if (b == nullptr) throw std::logic_error("group element pointer is null");

      if (g.operate(a, b) == e && g.operate(b, a) == e) {
        hasInv = true;
        break;
      }
    }

    if (!hasInv)
      throw std::logic_error("no two-sided inverse found for element index " + std::to_string(ai));
  }
}

template <GroupConcept G>
void requireAssociativity(const G& g) {
  const auto n = g.order();
  if (n == 0) return;

  for (std::size_t i = 0; i < n; ++i) {
    if (g.element(i) == nullptr)
      throw std::logic_error("group element pointer is null at index " + std::to_string(i));
  }

  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      for (std::size_t k = 0; k < n; ++k) {
        auto* a = g.element(i);
        auto* b = g.element(j);
        auto* c = g.element(k);

        auto* left = g.operate(a, g.operate(b, c));
        auto* right = g.operate(g.operate(a, b), c);
        if (left != right) {
          throw std::logic_error(
              "associativity violated at indices (" + std::to_string(i) + "," +
              std::to_string(j) + "," + std::to_string(k) + ")");
        }
      }
    }
  }
}

template <GroupConcept G>
void requireGroupAxioms(const G& g) {
  requireClosure(g);
  requireIdentity(g);
  requireInverses(g);
  requireAssociativity(g);
}

template <GroupConcept G>
void requireGroupAxiomsExceptAssociativity(const G& g) {
  requireClosure(g);
  requireIdentity(g);
  requireInverses(g);
}

} // namespace mg

#endif // MG_GROUP_VALIDATION_HPP


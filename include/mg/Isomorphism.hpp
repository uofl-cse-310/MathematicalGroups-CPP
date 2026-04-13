#ifndef MG_ISOMORPHISM_HPP
#define MG_ISOMORPHISM_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

#include <mg/GroupConcept.hpp>

namespace mg {

namespace detail {

inline constexpr std::size_t kUnassigned = std::numeric_limits<std::size_t>::max();

template <GroupConcept G>
std::size_t findIdentityIndex(const G& g) {
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

template <GroupConcept G>
std::vector<std::size_t> elementOrders(const G& g, std::size_t identityIndex) {
  const auto n = g.order();
  std::vector<std::size_t> ord(n, 0);

  auto* e = g.element(identityIndex);
  if (e == nullptr) throw std::logic_error("identity pointer is null");

  for (std::size_t i = 0; i < n; ++i) {
    auto* a = g.element(i);
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
      cur = g.operate(cur, a);
    }

    if (ord[i] == 0)
      throw std::logic_error(
          "element order did not reach identity within group order (invalid group)");
  }

  return ord;
}

inline bool sameMultiset(std::vector<std::size_t> a, std::vector<std::size_t> b) {
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());
  return a == b;
}

} // namespace detail

// Returns true iff there exists a bijective homomorphism A -> B.
// Works for any two group container types modeling mg::GroupConcept.
template <GroupConcept GA, GroupConcept GB>
bool isIsomorphicTo(const GA& A, const GB& B) {
  const auto n = A.order();
  if (n != B.order()) return false;
  if (n == 0) return true;

  const auto idA = detail::findIdentityIndex(A);
  const auto idB = detail::findIdentityIndex(B);

  const auto ordA = detail::elementOrders(A, idA);
  const auto ordB = detail::elementOrders(B, idB);
  if (!detail::sameMultiset(ordA, ordB)) return false;

  // Precompute multiplication tables in index form.
  std::vector<std::vector<std::size_t>> mulA(n, std::vector<std::size_t>(n, 0));
  std::vector<std::vector<std::size_t>> mulB(n, std::vector<std::size_t>(n, 0));

  // pointer->index maps via linear scan (n is expected to be small; keeps GroupConcept minimal).
  auto idxInA = [&](typename GA::pointer p) -> std::size_t {
    for (std::size_t i = 0; i < n; ++i)
      if (A.element(i) == p) return i;
    throw std::logic_error("operate() returned pointer not in element() list (A)");
  };
  auto idxInB = [&](typename GB::pointer p) -> std::size_t {
    for (std::size_t i = 0; i < n; ++i)
      if (B.element(i) == p) return i;
    throw std::logic_error("operate() returned pointer not in element() list (B)");
  };

  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      mulA[i][j] = idxInA(A.operate(A.element(i), A.element(j)));
      mulB[i][j] = idxInB(B.operate(B.element(i), B.element(j)));
    }
  }

  // Candidates by element order.
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

  std::vector<std::size_t> mapAtoB(n, detail::kUnassigned);
  std::vector<std::size_t> mapBtoA(n, detail::kUnassigned);
  mapAtoB[idA] = idB;
  mapBtoA[idB] = idA;

  // Variables to assign, ordered by fewest candidates.
  std::vector<std::size_t> vars;
  vars.reserve(n - 1);
  for (std::size_t i = 0; i < n; ++i)
    if (i != idA) vars.push_back(i);

  std::sort(vars.begin(), vars.end(), [&](std::size_t x, std::size_t y) {
    return candidates[x].size() < candidates[y].size();
  });

  auto respects = [&](std::size_t i, std::size_t jCand) -> bool {
    for (std::size_t k = 0; k < n; ++k) {
      const auto fk = mapAtoB[k];
      if (fk == detail::kUnassigned) continue;

      const auto ip = mulA[i][k];
      const auto fip = mapAtoB[ip];
      if (fip != detail::kUnassigned) {
        if (mulB[jCand][fk] != fip) return false;
      }

      const auto ip2 = mulA[k][i];
      const auto fip2 = mapAtoB[ip2];
      if (fip2 != detail::kUnassigned) {
        if (mulB[fk][jCand] != fip2) return false;
      }
    }

    return true;
  };

  std::function<bool(std::size_t)> dfs = [&](std::size_t pos) -> bool {
    if (pos == vars.size()) return true;
    const auto i = vars[pos];

    for (const auto j : candidates[i]) {
      if (mapBtoA[j] != detail::kUnassigned) continue;
      if (!respects(i, j)) continue;

      mapAtoB[i] = j;
      mapBtoA[j] = i;
      if (dfs(pos + 1)) return true;
      mapAtoB[i] = detail::kUnassigned;
      mapBtoA[j] = detail::kUnassigned;
    }

    return false;
  };

  return dfs(0);
}

// Cross-type equality for any two group containers.
// (This is intentionally not constrained to same type.)
template <GroupConcept GA, GroupConcept GB>
bool operator==(const GA& a, const GB& b) {
  return isIsomorphicTo(a, b);
}

template <GroupConcept GA, GroupConcept GB>
bool operator!=(const GA& a, const GB& b) {
  return !isIsomorphicTo(a, b);
}

} // namespace mg

#endif // MG_ISOMORPHISM_HPP


#include <gtest/gtest.h>

#include <mg/mg.hpp>

namespace {

// Build Z_n as a MathGroup over stable element objects.
struct E {
  int id = 0;
};

template <std::size_t N>
static mg::MathGroup<E> makeZn(std::array<E, N>& elems) {
  mg::MathGroup<E> g;
  g.setOrder(N);
  for (std::size_t i = 0; i < N; ++i) g.setElement(i, &elems[i]);

  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      g.setOperation(&elems[i], &elems[j], &elems[(i + j) % N]);
    }
  }

  return g;
}

TEST(Isomorphism, DirectSumAssociativityLikeStructure_Z6Z6MatchesZ3Z3PlusV4) {
  std::array<E, 6> e6{};
  for (int i = 0; i < 6; ++i) e6[i].id = i;
  auto Z6 = makeZn<6>(e6);

  std::array<E, 3> e3a{};
  std::array<E, 3> e3b{};
  for (int i = 0; i < 3; ++i) {
    e3a[i].id = i;
    e3b[i].id = i;
  }
  auto Z3a = makeZn<3>(e3a);
  auto Z3b = makeZn<3>(e3b);

  std::array<E, 2> e2a{};
  std::array<E, 2> e2b{};
  e2a[0].id = 0;
  e2a[1].id = 1;
  e2b[0].id = 0;
  e2b[1].id = 1;
  auto Z2a = makeZn<2>(e2a);
  auto Z2b = makeZn<2>(e2b);

  auto klein4 = Z2a + Z2b;     // V4
  auto ds33 = Z3a + Z3b;       // Z3 x Z3
  auto rhs = ds33 + klein4;    // (Z3xZ3) x (Z2xZ2)
  auto lhs = Z6 + Z6;          // Z6 x Z6

  EXPECT_TRUE(lhs == rhs);
}

} // namespace


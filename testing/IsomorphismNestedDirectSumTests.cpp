#include <gtest/gtest.h>

#include <array>

#include <mg/mg.hpp>

namespace {

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

TEST(Isomorphism, NestedDirectSum_Z2Z3_DoublesTo_Z6Z6) {
  std::array<E, 2> e2{};
  std::array<E, 3> e3{};
  std::array<E, 6> e6{};
  for (int i = 0; i < 2; ++i) e2[i].id = i;
  for (int i = 0; i < 3; ++i) e3[i].id = i;
  for (int i = 0; i < 6; ++i) e6[i].id = i;

  auto G2 = makeZn<2>(e2);
  auto G3 = makeZn<3>(e3);
  auto G6 = makeZn<6>(e6);

  EXPECT_TRUE(G2 + G3 == G6);
  EXPECT_TRUE((G2 + G3) + (G2 + G3) == (G6 + G6));
}

}

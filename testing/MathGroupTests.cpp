#include <gtest/gtest.h>

#include <mg/MathGroup.hpp>

namespace {

struct Z2 {
  int v = 0;
};

TEST(MathGroup, CanSetOrderElementsAndOperation) {
  mg::MathGroup<Z2> g;
  g.setOrder(2);

  Z2 a{0};
  Z2 b{1};

  g.setElement(0, &a);
  g.setElement(1, &b);

  // Cayley table for Z2 under addition
  g.setOperation(&a, &a, &a);
  g.setOperation(&a, &b, &b);
  g.setOperation(&b, &a, &b);
  g.setOperation(&b, &b, &a);

  EXPECT_EQ(g.operate(&b, &b), &a);
  EXPECT_EQ(g.operate(&a, &b), &b);
}

struct E {
  int id = 0; // unique identity for pointer stability
};

static mg::MathGroup<E> makeZ2(E& e0, E& e1, bool swapped) {
  mg::MathGroup<E> g;
  g.setOrder(2);


  if (!swapped) {
    g.setElement(0, &e0);
    g.setElement(1, &e1);
  } else {
    g.setElement(0, &e1);
    g.setElement(1, &e0);
  }

  auto* a = g.element(0);
  auto* b = g.element(1);

  g.setOperation(a, a, a);
  g.setOperation(a, b, b);
  g.setOperation(b, a, b);
  g.setOperation(b, b, a);

  return g;
}

static mg::MathGroup<E> makeZ4(E& e0, E& e1, E& e2, E& e3) {
  mg::MathGroup<E> g;
  g.setOrder(4);
  g.setElement(0, &e0); // 0
  g.setElement(1, &e1); // 1
  g.setElement(2, &e2); // 2
  g.setElement(3, &e3); // 3

  // addition mod 4
  for (std::size_t i = 0; i < 4; ++i) {
    for (std::size_t j = 0; j < 4; ++j) {
      g.setOperation(g.element(i), g.element(j), g.element((i + j) % 4));
    }
  }
  return g;
}

static mg::MathGroup<E> makeV4(E& e0, E& eA, E& eB, E& eC) {
  mg::MathGroup<E> g;
  g.setOrder(4);
  g.setElement(0, &e0); // identity
  g.setElement(1, &eA);
  g.setElement(2, &eB);
  g.setElement(3, &eC);

  // Klein four group: all non-identity elements have order 2.
  auto* e = g.element(0);
  auto* a = g.element(1);
  auto* b = g.element(2);
  auto* c = g.element(3);

  // e acts as identity
  for (auto* x : {e, a, b, c}) {
    g.setOperation(e, x, x);
    g.setOperation(x, e, x);
  }

  // a^2=b^2=c^2=e
  g.setOperation(a, a, e);
  g.setOperation(b, b, e);
  g.setOperation(c, c, e);

  // ab=ba=c, ac=ca=b, bc=cb=a
  g.setOperation(a, b, c);
  g.setOperation(b, a, c);

  g.setOperation(a, c, b);
  g.setOperation(c, a, b);

  g.setOperation(b, c, a);
  g.setOperation(c, b, a);

  return g;
}

TEST(MathGroup, EqualityMeansIsomorphic_Z2PermutedIsEqual) {
  E e0{0}, e1{1};
  E f0{0}, f1{1};

  auto g1 = makeZ2(e0, e1, false);
  auto g2 = makeZ2(f0, f1, true);

  EXPECT_TRUE(g1 == g2);
  EXPECT_FALSE(g1 != g2);
}

TEST(MathGroup, EqualityMeansIsomorphic_Z4NotIsomorphicToV4) {
  E z0{0}, z1{1}, z2{2}, z3{3};
  E v0{0}, vA{1}, vB{2}, vC{3};

  auto z4 = makeZ4(z0, z1, z2, z3);
  auto v4 = makeV4(v0, vA, vB, vC);

  EXPECT_FALSE(z4 == v4);
  EXPECT_TRUE(z4 != v4);
}

} // namespace

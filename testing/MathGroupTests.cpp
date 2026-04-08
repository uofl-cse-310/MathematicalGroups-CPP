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

} // namespace


#include <gtest/gtest.h>

#include <mg/DirectSum.hpp>
#include <mg/MathGroup.hpp>

namespace {

struct Z2 {
  int v = 0;
};

static mg::MathGroup<Z2> makeZ2(Z2& a, Z2& b) {
  mg::MathGroup<Z2> g;
  g.setOrder(2);
  g.setElement(0, &a);
  g.setElement(1, &b);
  g.setOperation(&a, &a, &a);
  g.setOperation(&a, &b, &b);
  g.setOperation(&b, &a, &b);
  g.setOperation(&b, &b, &a);
  return g;
}

TEST(DirectSum, HasOrderProductAndOperatesComponentwise) {
  Z2 a{0}, b{1};
  Z2 c{0}, d{1};

  auto g1 = makeZ2(a, b);
  auto g2 = makeZ2(c, d);

  auto ds = g1 + g2;
  EXPECT_EQ(ds.order(), 4u);

  auto* x = ds.element(0);
  auto* y = ds.element(3);

  auto* s = ds.operate(x, y);

  EXPECT_EQ(s->first, &b);
  EXPECT_EQ(s->second, &d);
}

}

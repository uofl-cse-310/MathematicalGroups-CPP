#include <iostream>

#include <mg/DirectSum.hpp>
#include <mg/MathGroup.hpp>

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

int main() {
  Z2 a{0}, b{1};
  Z2 c{0}, d{1};

  auto g1 = makeZ2(a, b);
  auto g2 = makeZ2(c, d);

  auto ds = g1 + g2;

  std::cout << "G1 order: " << g1.order() << "\n";
  std::cout << "G2 order: " << g2.order() << "\n";
  std::cout << "G1 + G2 order: " << ds.order() << "\n";

  auto* x = ds.element(0);
  auto* y = ds.element(3);
  auto* s = ds.operate(x, y);

  std::cout << "(0,0) + (1,1) = (" << s->first->v << "," << s->second->v << ")\n";
  return 0;
}

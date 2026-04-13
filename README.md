# MathematicalGroups-CPP

C++20 header-only utilities for finite groups represented by pointers and a Cayley operation table, plus direct sums and isomorphism testing.

## Layout

- `include/` public headers
- `testing/` GoogleTest unit tests
- `examples/` small demo executables

## Using from another CMake project (FetchContent)

```cmake
include(FetchContent)

FetchContent_Declare(
  MathematicalGroupsCPP
  GIT_REPOSITORY https://github.com/uofl-cse-310/MathematicalGroups-CPP
  GIT_TAG main
)

FetchContent_MakeAvailable(MathematicalGroupsCPP)

target_link_libraries(your_target PRIVATE MathematicalGroups::MathematicalGroups)
```

In your C++ code:

```cpp
#include <mg/mg.hpp>
```

## Concepts

### `mg::GroupConcept<G>`

A group container type must provide:

- `std::size_t order() const`
- `pointer element(std::size_t) const`
- `pointer operate(pointer, pointer) const`
- `void setOrder(std::size_t)`
- `void setElement(std::size_t, pointer)`
- `void setOperation(pointer, pointer, pointer)`

Pointers are treated as labels. Algorithms do not compare element object values.

## Build a group with `mg::MathGroup`

```cpp
#include <mg/mg.hpp>

struct E { int id = 0; };

mg::MathGroup<E> makeZ2(E& e0, E& e1) {
  mg::MathGroup<E> g;
  g.setOrder(2);
  g.setElement(0, &e0);
  g.setElement(1, &e1);
  g.setOperation(&e0, &e0, &e0);
  g.setOperation(&e0, &e1, &e1);
  g.setOperation(&e1, &e0, &e1);
  g.setOperation(&e1, &e1, &e0);
  return g;
}
```

## Direct sums

`operator+` builds a direct sum:

```cpp
auto ds = G1 + G2;
static_assert(mg::GroupConcept<decltype(ds)>);
```

The order is the product of factor orders.

## Isomorphism testing

Isomorphism is provided by this library and is based only on the operation induced by `element(i)` and `operate(a,b)`.

If you include `<mg/GroupIsomorphismEquality.hpp>` (or `<mg/mg.hpp>`), then `==` and `!=` mean group isomorphism for any two `GroupConcept` types.

```cpp
#include <mg/mg.hpp>

auto A = G2 + G3;
auto B = G6;

bool iso = (A == B);
```

You can also call the algorithm directly:

```cpp
bool iso = mg::isIsomorphicTo(A, B);
```

## Validation

This library can validate a group representation using only pointers and the operation:

```cpp
mg::requireGroupAxioms(G);
```

## Building standalone

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

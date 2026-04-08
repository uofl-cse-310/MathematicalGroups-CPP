# MathematicalGroups-CPP

This repo provides a small C++20 library for building finite groups from an explicit operation table and composing them via direct sum.

## Layout

- `include/` public headers
- `src/` (currently header-only; reserved for future non-template code)
- `testing/` GoogleTest unit tests
- `examples/` small demo executables

## Using from another CMake project (FetchContent)

```cmake
include(FetchContent)

FetchContent_Declare(
  MathematicalGroupsCPP
  GIT_REPOSITORY https://github.com/uofl-cse-310/MathematicalGroups-CPP
  GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(MathematicalGroupsCPP)

target_link_libraries(your_target PRIVATE MathematicalGroups::MathematicalGroups)
```

Then include headers like:

```cpp
#include <mg/MathGroup.hpp>
#include <mg/DirectSum.hpp>
// or: #include <mg/mg.hpp>
```

## Building standalone

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Concepts

- `mg::GroupElementConcept<T>`: optional concept for *element types* that support operators like `+`, unary `-`, and `==` and provide `T::identity()`.
- `mg::GroupConcept<G>`: concept for *group container types* that support:
  - `setOrder(n)`, `setElement(i, ptr)`, `setOperation(a,b,result)`
  - `order()`, `element(i)`, `operate(a,b)`

## Direct sum

`operator+` is overloaded for group containers:

```cpp
auto ds = g1 + g2; // ds is mg::DirectSum<decltype(g1), decltype(g2)>
```

Elements of the direct sum are ordered pairs of pointers into each factor group, and the operation is componentwise.

## MathGroup API

The `MathGroup` class template models a mathematical group. It requires the element type to model the `GroupElementConcept` and provides group operations.

### Example

```cpp
using MyGroup = MathGroup<MyElementType>;
MyGroup g;
g.setOrder(5);
```

## DirectSum API

The `DirectSum` class template models the direct sum of two groups. It requires both groups to have the same order and provides an operation that combines elements of the underlying groups.

### Example

```cpp
using MyDirectSum = DirectSum<Group1, Group2>;
MyDirectSum ds;
ds.setOrder(10);
```

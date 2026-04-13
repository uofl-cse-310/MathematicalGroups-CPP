include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# For MSVC runtime selection; harmless elsewhere.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(MathematicalGroupsTests
  MathGroupTests.cpp
  DirectSumTests.cpp
  IsomorphismDirectSumRegressionTests.cpp
)

target_link_libraries(MathematicalGroupsTests
  PRIVATE
    MathematicalGroups::MathematicalGroups
    GTest::gtest_main
)

target_compile_features(MathematicalGroupsTests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(MathematicalGroupsTests)

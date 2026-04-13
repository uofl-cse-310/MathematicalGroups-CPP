cmake_minimum_required(VERSION 3.20)

project(MathematicalGroupsCPP VERSION 1.7.0 LANGUAGES CXX)

# When used via FetchContent, turn tests/examples OFF by default.
option(MG_BUILD_TESTING "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(MG_BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})

add_library(MathematicalGroups INTERFACE)
add_library(MathematicalGroups::MathematicalGroups ALIAS MathematicalGroups)

target_compile_features(MathematicalGroups INTERFACE cxx_std_20)

target_include_directories(MathematicalGroups
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MG_BUILD_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()

if(MG_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()


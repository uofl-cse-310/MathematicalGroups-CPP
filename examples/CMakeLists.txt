add_executable(direct_sum_example direct_sum_example.cpp)

target_link_libraries(direct_sum_example
  PRIVATE
    MathematicalGroups::MathematicalGroups
)

target_compile_features(direct_sum_example PRIVATE cxx_std_20)


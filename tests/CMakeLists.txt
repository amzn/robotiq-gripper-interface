# Copyright 2020-2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.

# Set the library name
set(test rai_robotiq_tests)

# Set the test file names
set(test_srcs
  ${CMAKE_CURRENT_SOURCE_DIR}/test_helpers.cc
)

# -----------------------------------------------------------------------------
# Test target
# -----------------------------------------------------------------------------
add_executable(${test} ${test_srcs})

add_test(${test} ${test})

add_dependencies(${test}
  "robotiq-gripper-interface"
)

target_link_libraries(${test} PRIVATE
  "robotiq-gripper-interface"
  ${GTEST_LIBRARIES}
  ${GTEST_MAIN_LIBRARIES}
  ${Boost_LIBRARIES}
  pthread  # This needs to come after gtest libs
)

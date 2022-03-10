# Copyright 2020-2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.

# -----------------------------------------------------------------------------
# Demo target
# -----------------------------------------------------------------------------
set(example position_gripper)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(srcs
  ${CMAKE_CURRENT_SOURCE_DIR}/position_gripper.cc
)

add_executable(${example} ${srcs})

add_dependencies(${example}
  "robotiq-gripper-interface"
)

target_link_libraries(${example} PRIVATE
  "robotiq-gripper-interface"
)

set_target_properties(${example} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

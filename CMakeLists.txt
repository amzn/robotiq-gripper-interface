cmake_minimum_required(VERSION 3.10.2)

project(RobotiqGripperInterface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(TARGET_NAME robotiq-gripper-interface)

# Include custom functions
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

include(CTest)
enable_testing()

add_compile_options(-Wall -Wextra -O3 -g)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# -----------------------------------------------------------------------------
# Set dependencies
# -----------------------------------------------------------------------------
find_package(Boost 1.65.1 REQUIRED COMPONENTS system)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
)

# -----------------------------------------------------------------------------
# Library
# -----------------------------------------------------------------------------

# Set the public header names
set(library_public_hdrs
  ${PROJECT_SOURCE_DIR}/include/robotiq/constants.h
  ${PROJECT_SOURCE_DIR}/include/robotiq/types.h
  ${PROJECT_SOURCE_DIR}/include/robotiq/robotiq_gripper_interface.h
)

# Set the header names
set(library_private_hdrs
 ${PROJECT_SOURCE_DIR}/src/helpers.h
 ${PROJECT_SOURCE_DIR}/src/timeout_reader.h
)

# Set the source file names
set(library_srcs
  ${PROJECT_SOURCE_DIR}/src/robotiq_gripper_interface.cc
  ${PROJECT_SOURCE_DIR}/src/helpers.cc
  ${PROJECT_SOURCE_DIR}/src/timeout_reader.cc
)

add_library(${TARGET_NAME} SHARED ${library_srcs})

set_target_properties(${TARGET_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
  PUBLIC_HEADER "${library_public_hdrs}"
  PRIVATE_HEADER "${library_private_hdrs}"
)

target_link_libraries(${TARGET_NAME} PRIVATE 
  ${Boost_LIBRARIES}
  pthread
)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/RobotiqGripperInterfaceConfig.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/RobotiqGripperInterfaceConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  TARGETS ${TARGET_NAME}
  EXPORT ${PROJECT_NAME}Targets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/robotiq
)

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES
    ${CMAKE_BINARY_DIR}/cmake/RobotiqGripperInterfaceConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIR})
  add_subdirectory(tests)
endif()

# -----------------------------------------------------------------------------
# Examples
# -----------------------------------------------------------------------------
add_subdirectory(examples/activate_gripper)
add_subdirectory(examples/open_gripper)
add_subdirectory(examples/close_gripper)
add_subdirectory(examples/position_gripper)

cmake_minimum_required(VERSION 2.8)
project(darc_procedure)

find_package(catkin)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES darc_procedure
)

find_package(Boost COMPONENTS system signals thread)
find_package(catkin COMPONENTS darc_common darc_ns iris)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS})

add_library(darc_procedure SHARED
  src/procedure_service.cpp)
target_link_libraries(darc_procedure ${catkin_LIBRARIES})

catkin_add_gtest(darc_procedure_gtest test/darc_procedure_gtest.cpp)
target_link_libraries(darc_procedure_gtest
  ${GTEST_BOTH_LIBRARIES}
  darc_procedure)

# Install
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hpp")

# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.15)

project( stl2swc )

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()


# Eigen3 and its components
find_package( Eigen3 REQUIRED )

if ( NOT Eigen3_FOUND )

  message(STATUS "This project requires the Eigen3 library, and will not be compiled.")

  return()  

endif()

include(CGAL_Eigen_support)
#include( ${EIGEN3_USE_FILE} )

# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

# include for local directory

# include for local package


# Creating entries for target: stl2swc
# ############################

#create_single_source_cgal_program( stl2swc.cpp)

add_executable( stl2swc stl2swc.cpp )

add_to_cached_list( CGAL_EXECUTABLE_TARGETS stl2swc )
#
## Link the executable to CGAL and third-party libraries
target_link_libraries(stl2swc PRIVATE CGAL::CGAL CGAL::Eigen_support)
#target_link_libraries(stl2swc PUBLIC CGAL::Eigen_support)

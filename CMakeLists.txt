#   Copyright 2017 KeycapEmu
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

cmake_minimum_required(VERSION 3.26)
project(KeycapRoot)
include(cmake/PreventInSourceBuilds.cmake)


option(KeycapRoot_ENABLE_TESTING "Enable unit-testing" OFF)

enable_testing()
 
 set_property(GLOBAL PROPERTY USE_FOLDERS ON)
 
 set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
 set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/contrib/sanitizers-cmake/cmake ${CMAKE_MODULE_PATH})

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(keycaproot_project_options INTERFACE)
add_library(keycap::root::project_options ALIAS keycaproot_project_options)

set_target_properties(keycaproot_project_options PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(keycaproot_project_options INTERFACE cxx_std_23)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
  if(ENABLE_BUILD_WITH_TIME_TRACE)
    target_compile_options(keycaproot_project_options INTERFACE -ftime-trace)
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  target_compile_definitions(keycaproot_project_options INTERFACE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
  target_compile_options(keycaproot_project_options INTERFACE 
    /std:c++latest 
    /permissive- 
    /experimental:external 
    /external:W0 
    #/external:env:cpp_external_paths
    /external:anglebrackets
  )
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(keycaproot_project_warnings INTERFACE)
add_library(keycap::root::project_warnings ALIAS keycaproot_project_warnings)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(keycaproot_project_warnings)


include(FetchContent)
 
 ################################
 # Git
 ################################
 
 include(GetGitRevisionDescription)
 get_git_head_revision(GIT_REFSPEC GIT_SHA1)
 git_describe(GIT_BRANCH --all --tags --dirty=-d)
 configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/libs/Utility/Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" @ONLY)
 list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" Version.h)
 set(version_file "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")
 
 ################################
 # Zlib
 ################################
 
 message("keycap::root - Downloading Zlib")

FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG  v1.3
)
FetchContent_GetProperties(zlib)
 if(NOT zlib_POPULATED)
     FetchContent_Populate(zlib)
     add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
 endif()
target_include_directories(zlib PUBLIC  
    $<BUILD_INTERFACE:${zlib_BINARY_DIR}>  
    $<BUILD_INTERFACE:${zlib_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:include>
)
target_include_directories(zlibstatic PUBLIC  
    $<BUILD_INTERFACE:${zlib_BINARY_DIR}> 
    $<BUILD_INTERFACE:${zlib_SOURCE_DIR}>   
    $<INSTALL_INTERFACE:include>
)

 message("${zlib_POPULATED} - ${zlib_SOURCE_DIR} - ${zlib_BINARY_DIR}")
 
 ################################
 # Boost
 ################################

 message("keycap::root - Downloading Boost. This takes forever! Grab a coffee or five.")

set(Boost_USE_STATIC_LIBS ON)
set(BOOST_IOSTREAMS_ENABLE_ZLIB OFF)

FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.81.0
)
FetchContent_MakeAvailable(Boost)
 
# ################################
# # external submodules
# ################################

add_subdirectory("${PROJECT_SOURCE_DIR}/contrib/rapidcheck")
 
# ################################
# # sanitizers
# ################################
find_package(Sanitizers)
# 
 ################################
 # Source Directories
 ################################
 
add_subdirectory (src/libs)

if(KeycapRoot_ENABLE_TESTING)
    add_subdirectory (src/test)
endif()
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(SciToolbox LANGUAGES CXX)

# Set C++ standard (C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set global compiler flags
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Find GSL
find_package(GSL REQUIRED)

# Find Boost with the JSON, system and filesystemcomponents
find_package(Boost REQUIRED COMPONENTS json system filesystem)

if (Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

# Add the src and apps directories

add_subdirectory(src)
add_subdirectory(apps)

# Options to select which libraries and apps to build
option(BUILD_LIB_DATAFRAME "Build the Datafame library and its app" ON)
option(BUILD_LIB_INTERPOLATION "Build the Interpolation library and its app" ON)

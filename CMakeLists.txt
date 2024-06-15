cmake_minimum_required(VERSION 3.28.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GDShare VERSION 1.0.1)

file(GLOB_RECURSE SOURCES
	src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_link_libraries(${PROJECT_NAME} geode-sdk)
setup_geode_mod(${PROJECT_NAME} EXTERNALS geode.node-ids:1.12.0)

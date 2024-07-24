cmake_minimum_required (VERSION 3.28.3)

project(LLanguageCompiler CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB SOURCES "*.cpp")

add_library(components STATIC ${SOURCES})
target_include_directories(components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
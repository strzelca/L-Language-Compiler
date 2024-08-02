cmake_minimum_required(VERSION 3.28.3)

project(LLanguageCompiler)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)

if(CMAKE_HOST_WIN32 AND DEFINED ENV{LLVM_DIR})
  set(LLVM_DIR "$ENV{LLVM_DIR}")
  add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
endif()

 
find_package(LLVM REQUIRED CONFIG HINTS "${LLVM_DIR}")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(
  ${LLVM_INCLUDE_DIRS}
  include
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(compiler main.cpp)

llvm_map_components_to_libnames(llvm_libs support core orcjit native irreader)

target_link_libraries(
  compiler
  ${llvm_libs}
)

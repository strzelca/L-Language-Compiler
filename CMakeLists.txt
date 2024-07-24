cmake_minimum_required(VERSION 3.28.3)

project(LLanguageCompiler)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(
  /usr/local/include
  /opt/homebrew/opt/llvm/include/c++/v1
  ${LLVM_INCLUDE_DIRS}
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_subdirectory(src)

add_executable(compiler main.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(
  compiler
  components
  ${llvm_libs}
)

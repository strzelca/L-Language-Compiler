$CMAKE_FLAGS = '-DCMAKE_BUILD_TYPE=Release'
cmake $CMAKE_FLAGS $PWD -G Ninja
ninja
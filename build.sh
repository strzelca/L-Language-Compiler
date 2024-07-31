CC="/opt/homebrew/opt/llvm/bin/clang"
CXX="/opt/homebrew/opt/llvm/bin/clang++"

CMAKE_FLAGS="-DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX}"

cmake $CMAKE_FLAGS .
make clean
make -j$(nproc)
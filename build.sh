cmake -S . -B build
#-DCMAKE_BUILD_TYPE=Debug
cmake --build build -j12
#gdb ./build/ImgProc
./build/ImgProc

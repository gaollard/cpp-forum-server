#g++ -std=c++11 main.cpp -o main -ljsoncpp
#mkdir build
rm -rf cmake-build-debug
rm -rf CmakeFiles
rm -rf build
rm -rf Makefile

mkdir build
cd build
cmake ..
make
#/bin/sh

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../source
make -j 8

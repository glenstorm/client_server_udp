rm -r build
mkdir build
cd build
cp -f ../data/* ./
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ../
ninja && ./udp_server

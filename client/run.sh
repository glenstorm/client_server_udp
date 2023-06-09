rm -r build
mkdir build
cd build
cp -f ../data/* ./
cmake -GNinja ../
ninja
./udp_client 1.jpg&
./udp_client 2.jpg&
./udp_client 3.jpg&
./udp_client 4.jpg&
./udp_client 5.jpg&
./udp_client 6.jpg&

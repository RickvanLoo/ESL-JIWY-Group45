rm -rf build
mkdir build
cd build
cmake -DARM=False ..
make
cd ..
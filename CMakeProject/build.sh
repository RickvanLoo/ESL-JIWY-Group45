rm -rf build
mkdir build
cd build
cmake -DARM=False -DBUILD_SHARED_LIBS=OFF ..
make
cd ..
rm -rf arm_build
mkdir arm_build
cd arm_build
cmake -DARM=True ..
make
cd ..
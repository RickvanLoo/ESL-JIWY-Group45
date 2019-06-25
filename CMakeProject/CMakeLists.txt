cmake_minimum_required(VERSION 2.8)

message("ARM=${ARM}")

if(ARM)
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
SET(CMAKE_SYSTEM_PROCESSOR arm)
SET(CMAKE_FIND_ROOT_PATH /home/rick/opencv/platforms/linux/overo)
endif()

project( Overo )
#find_package( OpenCV REQUIRED )

#message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
#message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
#include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( Overo OpenCVProject.cpp SerialConnection.cpp SerialConnection.h )
#target_link_libraries( Overo ${OpenCV_LIBS} )
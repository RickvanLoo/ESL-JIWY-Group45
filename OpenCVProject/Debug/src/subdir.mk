################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AngleConvert.cpp \
../src/FaceDetection.cpp \
../src/OpenCVProject.cpp \
../src/SerialConnection.cpp 

OBJS += \
./src/AngleConvert.o \
./src/FaceDetection.o \
./src/OpenCVProject.o \
./src/SerialConnection.o 

CPP_DEPS += \
./src/AngleConvert.d \
./src/FaceDetection.d \
./src/OpenCVProject.d \
./src/SerialConnection.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



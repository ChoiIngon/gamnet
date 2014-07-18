################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Log/File.cpp \
../Log/Log.cpp \
../Log/Logger.cpp 

OBJS += \
./Log/File.o \
./Log/Log.o \
./Log/Logger.o 

CPP_DEPS += \
./Log/File.d \
./Log/Log.d \
./Log/Logger.d 


# Each subdirectory must supply rules for building sources it contributes
Log/%.o: ../Log/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



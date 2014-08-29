################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Library/Base64.cpp \
../Library/Buffer.cpp \
../Library/Exception.cpp 

OBJS += \
./Library/Base64.o \
./Library/Buffer.o \
./Library/Exception.o 

CPP_DEPS += \
./Library/Base64.d \
./Library/Buffer.d \
./Library/Exception.d 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o: ../Library/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



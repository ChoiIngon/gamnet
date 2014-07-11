################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Library/HttpClient.cpp 

OBJS += \
./Library/HttpClient.o 

CPP_DEPS += \
./Library/HttpClient.d 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o: ../Library/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_DEBUG -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



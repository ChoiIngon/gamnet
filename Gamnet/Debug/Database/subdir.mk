################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Database/Database.cpp \
../Database/DatabaseImpl.cpp 

OBJS += \
./Database/Database.o \
./Database/DatabaseImpl.o 

CPP_DEPS += \
./Database/Database.d \
./Database/DatabaseImpl.d 


# Each subdirectory must supply rules for building sources it contributes
Database/%.o: ../Database/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


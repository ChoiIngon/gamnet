################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Database/Redis/Connection.cpp \
../Database/Redis/Redis.cpp \
../Database/Redis/ResultSet.cpp 

OBJS += \
./Database/Redis/Connection.o \
./Database/Redis/Redis.o \
./Database/Redis/ResultSet.o 

CPP_DEPS += \
./Database/Redis/Connection.d \
./Database/Redis/Redis.d \
./Database/Redis/ResultSet.d 


# Each subdirectory must supply rules for building sources it contributes
Database/Redis/%.o: ../Database/Redis/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



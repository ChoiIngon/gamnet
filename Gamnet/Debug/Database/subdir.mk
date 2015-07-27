################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Database/Connection.cpp \
../Database/Database.cpp \
../Database/DatabaseImpl.cpp \
../Database/ResultSet.cpp \
../Database/Transaction.cpp 

OBJS += \
./Database/Connection.o \
./Database/Database.o \
./Database/DatabaseImpl.o \
./Database/ResultSet.o \
./Database/Transaction.o 

CPP_DEPS += \
./Database/Connection.d \
./Database/Database.d \
./Database/DatabaseImpl.d \
./Database/ResultSet.d \
./Database/Transaction.d 


# Each subdirectory must supply rules for building sources it contributes
Database/%.o: ../Database/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



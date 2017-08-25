################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Network/Tcp/Packet.cpp \
../Network/Tcp/Session.cpp \
../Network/Tcp/Tcp.cpp 

OBJS += \
./Network/Tcp/Packet.o \
./Network/Tcp/Session.o \
./Network/Tcp/Tcp.o 

CPP_DEPS += \
./Network/Tcp/Packet.d \
./Network/Tcp/Session.d \
./Network/Tcp/Tcp.d 


# Each subdirectory must supply rules for building sources it contributes
Network/Tcp/%.o: ../Network/Tcp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_DEBUG -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



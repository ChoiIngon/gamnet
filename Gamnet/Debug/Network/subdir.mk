################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Network/HandlerContainer.cpp \
../Network/Listener.cpp \
../Network/Packet.cpp \
../Network/Session.cpp \
../Network/SessionManager.cpp 

OBJS += \
./Network/HandlerContainer.o \
./Network/Listener.o \
./Network/Packet.o \
./Network/Session.o \
./Network/SessionManager.o 

CPP_DEPS += \
./Network/HandlerContainer.d \
./Network/Listener.d \
./Network/Packet.d \
./Network/Session.d \
./Network/SessionManager.d 


# Each subdirectory must supply rules for building sources it contributes
Network/%.o: ../Network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



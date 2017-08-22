################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Network/HandlerContainer.cpp \
../Network/Link.cpp \
../Network/LinkManager.cpp \
../Network/Session.cpp 

OBJS += \
./Network/HandlerContainer.o \
./Network/Link.o \
./Network/LinkManager.o \
./Network/Session.o 

CPP_DEPS += \
./Network/HandlerContainer.d \
./Network/Link.d \
./Network/LinkManager.d \
./Network/Session.d 


# Each subdirectory must supply rules for building sources it contributes
Network/%.o: ../Network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_DEBUG -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



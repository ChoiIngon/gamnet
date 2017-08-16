################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Router/Router.cpp \
../Router/RouterCaster.cpp \
../Router/RouterHandler.cpp \
../Router/RouterListener.cpp \
../Router/Session.cpp 

OBJS += \
./Router/Router.o \
./Router/RouterCaster.o \
./Router/RouterHandler.o \
./Router/RouterListener.o \
./Router/Session.o 

CPP_DEPS += \
./Router/Router.d \
./Router/RouterCaster.d \
./Router/RouterHandler.d \
./Router/RouterListener.d \
./Router/Session.d 


# Each subdirectory must supply rules for building sources it contributes
Router/%.o: ../Router/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



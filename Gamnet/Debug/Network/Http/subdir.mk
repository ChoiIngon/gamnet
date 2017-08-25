################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Network/Http/Dispatcher.cpp \
../Network/Http/HttpClient.cpp \
../Network/Http/HttpServer.cpp \
../Network/Http/LinkManager.cpp \
../Network/Http/Response.cpp \
../Network/Http/Session.cpp 

OBJS += \
./Network/Http/Dispatcher.o \
./Network/Http/HttpClient.o \
./Network/Http/HttpServer.o \
./Network/Http/LinkManager.o \
./Network/Http/Response.o \
./Network/Http/Session.o 

CPP_DEPS += \
./Network/Http/Dispatcher.d \
./Network/Http/HttpClient.d \
./Network/Http/HttpServer.d \
./Network/Http/LinkManager.d \
./Network/Http/Response.d \
./Network/Http/Session.d 


# Each subdirectory must supply rules for building sources it contributes
Network/Http/%.o: ../Network/Http/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_DEBUG -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



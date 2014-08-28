################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Http/Dispatcher.cpp \
../Http/HttpClient.cpp \
../Http/HttpServer.cpp \
../Http/Listener.cpp \
../Http/Response.cpp \
../Http/Session.cpp 

OBJS += \
./Http/Dispatcher.o \
./Http/HttpClient.o \
./Http/HttpServer.o \
./Http/Listener.o \
./Http/Response.o \
./Http/Session.o 

CPP_DEPS += \
./Http/Dispatcher.d \
./Http/HttpClient.d \
./Http/HttpServer.d \
./Http/Listener.d \
./Http/Response.d \
./Http/Session.d 


# Each subdirectory must supply rules for building sources it contributes
Http/%.o: ../Http/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



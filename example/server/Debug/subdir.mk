################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Handler_HeartBeat.cpp \
../Handler_Http_ServerState.cpp \
../Handler_Login.cpp \
../Manager_Session.cpp \
../Session.cpp \
../main.cpp 

OBJS += \
./Handler_HeartBeat.o \
./Handler_Http_ServerState.o \
./Handler_Login.o \
./Manager_Session.o \
./Session.o \
./main.o 

CPP_DEPS += \
./Handler_HeartBeat.d \
./Handler_Http_ServerState.d \
./Handler_Login.d \
./Manager_Session.d \
./Session.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_DEBUG -DDEBUG -I/usr/local/include -I/usr/local/Cellar/openssl/1.0.2l/include -I/usr/local/Cellar/curl/7.55.1/include -I/usr/local/Cellar/mysql-connector-c/6.1.6/include -I/usr/local/mysql-connector-c-6.1.1/include -I/usr/local/Cellar/boost/1.64.0_1/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



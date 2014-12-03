################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/compiler.cpp \
../src/generate_rule_cpp.cpp \
../src/generate_rule_csharp.cpp \
../src/generate_rule_python.cpp \
../src/lexer.cpp \
../src/main.cpp \
../src/parser.cpp 

OBJS += \
./src/compiler.o \
./src/generate_rule_cpp.o \
./src/generate_rule_csharp.o \
./src/generate_rule_python.o \
./src/lexer.o \
./src/main.o \
./src/parser.o 

CPP_DEPS += \
./src/compiler.d \
./src/generate_rule_cpp.d \
./src/generate_rule_csharp.d \
./src/generate_rule_python.d \
./src/lexer.d \
./src/main.d \
./src/parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



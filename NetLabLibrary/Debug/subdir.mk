################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MThread.cpp \
../MultipleTCPSocketsListener.cpp \
../TCPSocket.cpp \
../UDPSocket.cpp 

OBJS += \
./MThread.o \
./MultipleTCPSocketsListener.o \
./TCPSocket.o \
./UDPSocket.o 

CPP_DEPS += \
./MThread.d \
./MultipleTCPSocketsListener.d \
./TCPSocket.d \
./UDPSocket.d 


# Each subdirectory must supply rules for building sources it contributes
MThread.o: ../MThread.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -Ipthread -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"MThread.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Suscripciones.c \
../src/brokerSO.c \
../src/log.c \
../src/memoria.c 

OBJS += \
./src/Suscripciones.o \
./src/brokerSO.o \
./src/log.o \
./src/memoria.o 

C_DEPS += \
./src/Suscripciones.d \
./src/brokerSO.d \
./src/log.d \
./src/memoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-Programadores-en-Fuga/messages_lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Team.c \
../src/deadlock.c \
../src/destroyers.c \
../src/entrenadores.c \
../src/planificador.c \
../src/servidorGameboy.c 

OBJS += \
./src/Team.o \
./src/deadlock.o \
./src/destroyers.o \
./src/entrenadores.o \
./src/planificador.o \
./src/servidorGameboy.o 

C_DEPS += \
./src/Team.d \
./src/deadlock.d \
./src/destroyers.d \
./src/entrenadores.d \
./src/planificador.d \
./src/servidorGameboy.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-Programadores-en-Fuga/messages_lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



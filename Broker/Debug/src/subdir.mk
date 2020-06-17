################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Suscripciones.c \
../src/brokerSO.c \
../src/files.c \
../src/filesColas.c \
../src/memoria.c \
../src/memoriaBuddy.c \
../src/memoriaParticiones.c 

OBJS += \
./src/Suscripciones.o \
./src/brokerSO.o \
./src/files.o \
./src/filesColas.o \
./src/memoria.o \
./src/memoriaBuddy.o \
./src/memoriaParticiones.o 

C_DEPS += \
./src/Suscripciones.d \
./src/brokerSO.d \
./src/files.d \
./src/filesColas.d \
./src/memoria.d \
./src/memoriaBuddy.d \
./src/memoriaParticiones.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-Programadores-en-Fuga/messages_lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



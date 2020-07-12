################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FileSystem.c \
../arbolDirectorio.c \
../archivoHeader.c \
../gamecard.c 

OBJS += \
./FileSystem.o \
./arbolDirectorio.o \
./archivoHeader.o \
./gamecard.o 

C_DEPS += \
./FileSystem.d \
./arbolDirectorio.d \
./archivoHeader.d \
./gamecard.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-Programadores-en-Fuga/messages_lib" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



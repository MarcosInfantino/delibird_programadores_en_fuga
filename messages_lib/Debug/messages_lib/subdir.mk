################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../messages_lib/appearedPokemon.c \
../messages_lib/catchPokemon.c \
../messages_lib/caughtPokemon.c \
../messages_lib/colaMutex.c \
../messages_lib/getPokemon.c \
../messages_lib/listaMutex.c \
../messages_lib/localizedPokemon.c \
../messages_lib/log.c \
../messages_lib/messages_lib.c \
../messages_lib/newPokemon.c \
../messages_lib/paquete.c \
../messages_lib/suscripcion.c \
../messages_lib/suscripcionTiempo.c 

OBJS += \
./messages_lib/appearedPokemon.o \
./messages_lib/catchPokemon.o \
./messages_lib/caughtPokemon.o \
./messages_lib/colaMutex.o \
./messages_lib/getPokemon.o \
./messages_lib/listaMutex.o \
./messages_lib/localizedPokemon.o \
./messages_lib/log.o \
./messages_lib/messages_lib.o \
./messages_lib/newPokemon.o \
./messages_lib/paquete.o \
./messages_lib/suscripcion.o \
./messages_lib/suscripcionTiempo.o 

C_DEPS += \
./messages_lib/appearedPokemon.d \
./messages_lib/catchPokemon.d \
./messages_lib/caughtPokemon.d \
./messages_lib/colaMutex.d \
./messages_lib/getPokemon.d \
./messages_lib/listaMutex.d \
./messages_lib/localizedPokemon.d \
./messages_lib/log.d \
./messages_lib/messages_lib.d \
./messages_lib/newPokemon.d \
./messages_lib/paquete.d \
./messages_lib/suscripcion.d \
./messages_lib/suscripcionTiempo.d 


# Each subdirectory must supply rules for building sources it contributes
messages_lib/%.o: ../messages_lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



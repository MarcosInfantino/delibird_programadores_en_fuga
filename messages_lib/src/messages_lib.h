/*
 * messages_lib.h
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */

#ifndef MESSAGES_LIB_H_
#define MESSAGES_LIB_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
//
typedef struct{
	int size;
	void* stream;
} t_buffer;


typedef enum
{
	CADENA = 1,
}op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct{
	uint32_t modulo;
	uint32_t tipoMensaje;
	uint32_t idProceso;
	uint32_t cola;
	uint32_t socket;
}mensajeSuscripcion;

t_paquete* armarPaquete(char* cadena);

void enviarMensaje(char* cadena, int socket);
void enviarMensajeSuscripcion(mensajeSuscripcion* mensaje, int socket);

void* serializarPaquete(t_paquete* paquete, int bytes);
void* serializarMensajeSuscripcion(mensajeSuscripcion* mensaje,uint32_t bytes);

mensajeSuscripcion* deserializarMensajeSuscripcion(void* stream, uint32_t bytes);



#endif /* MESSAGES_LIB_H_ */

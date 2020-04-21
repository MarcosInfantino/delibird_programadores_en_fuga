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

t_paquete* armarPaquete(char* cadena);

void enviarMensaje(char* cadena, int socket);

void* serializarPaquete(t_paquete* paquete, int bytes);



#endif /* MESSAGES_LIB_H_ */

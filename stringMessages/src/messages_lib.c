/*
 * messages_lib.c
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */
#include "messages_lib.h"

t_paquete* armarPaquete(char* cadena){
	t_buffer* buffer=malloc(sizeof(t_buffer));
	buffer->size=strlen(cadena)+1;
	buffer->stream=malloc(buffer->size);
	memcpy(buffer->stream,cadena,buffer->size);
	t_paquete* paquete=malloc(sizeof(t_paquete));
	paquete->codigo_operacion=CADENA;
	paquete->buffer=buffer;

	return paquete;

}

void* serializarPaquete (t_paquete* paquete, int bytes){
	void* a_enviar=malloc(bytes);
		int offset=0;
		memcpy(a_enviar+offset,&(paquete->codigo_operacion),sizeof(op_code));
		offset+=sizeof(op_code);

		memcpy(a_enviar+offset,&(paquete->buffer->size),sizeof(int));
		offset+=sizeof(int);

		memcpy(a_enviar+offset,(paquete->buffer->stream),paquete->buffer->size);


		return a_enviar;
}

void enviarMensaje(char* cadena,int socket){
	t_paquete* paquete= armarPaquete(cadena);

	int bytes=sizeof(op_code)+sizeof(paquete->buffer->size)+paquete->buffer->size;

	void * a_enviar=serializarPaquete(paquete,bytes);
	send(socket,a_enviar,bytes,0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(a_enviar);
}

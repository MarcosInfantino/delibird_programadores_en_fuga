/*
 * messages_lib.c
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */
#include "messages_lib.h"

t_paquete* armarPaquete(char* cadena){
	t_buffer* buffer          = malloc(sizeof(t_buffer));
	buffer->size	          = strlen(cadena)+1;
	buffer->stream	          = malloc(buffer->size);
	memcpy(buffer->stream,cadena,buffer->size);
	t_paquete* paquete        = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CADENA;
	paquete->buffer           = buffer;

	return paquete;

}

void* serializarPaquete1 (t_paquete* paquete, int bytes){
	void* a_enviar = malloc(bytes);
		int offset = 0;
		memcpy(a_enviar+offset,&(paquete->codigo_operacion),sizeof(op_code));
		offset+= sizeof(op_code);

		memcpy(a_enviar+offset,&(paquete->buffer->size),sizeof(int));
		offset+= sizeof(int);

		memcpy(a_enviar+offset,(paquete->buffer->stream),paquete->buffer->size);
		return a_enviar;
}

void enviarMensaje(char* cadena,int socket){
	t_paquete* paquete= armarPaquete(cadena);

	int bytes = sizeof(op_code)+sizeof(paquete->buffer->size)+paquete->buffer->size;

	void * a_enviar = serializarPaquete1(paquete, bytes);
	send(socket,a_enviar,bytes,0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(a_enviar);
}

//void enviarMensajeSuscripcion(mensajeSuscripcion* mensaje, int socket){
//	uint32_t bytes=sizeof(uint32_t)*4;
//	void* stream=serializarMensajeSuscripcion(mensaje, bytes);
//	send(socket,stream,bytes,0);
//}

void* serializarAppearedBroker(mensajeAppearedBroker* mensaje){
	void* stream    = malloc(sizeof(mensajeAppearedBroker));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	return mensaje;
}

void* serializarAppearedTeam(mensajeAppearedTeam* mensaje){
	void* stream    = malloc(sizeof(mensajeAppearedTeam));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return mensaje;
}

void* serializarNewBroker(mensajeNewBroker* mensaje){
	void* stream 	= malloc(sizeof(mensajeNewBroker));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->cantidad), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return mensaje;
}

void* serializarNewGamecard(mensajeNewGamecard* mensaje){
	void* stream    = malloc(sizeof(mensajeNewGamecard));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->cantidad), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return mensaje;
}

void* serializarCatchBroker(mensajeCatchBroker* mensaje){
	void* stream    = malloc(sizeof(mensajeCatchBroker));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return mensaje;
}

void* serializarCatchGamecard(mensajeCatchGamecard* mensaje){
	void* stream    = malloc(sizeof(mensajeCatchGamecard));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	return mensaje;
}

void* serializarCaught (mensajeCaught* mensaje){
	void* stream    = malloc(sizeof(mensajeCaught));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->resultadoCaught), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	return stream;
}

void* serializarGet (mensajeGet* mensaje){
	void* stream   = malloc(sizeof(mensajeGet));
	uint32_t offset= 0;

	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	return stream;
}

//void* serializarLocalized (mensajeLocalized* mensaje){
//	void* stream = malloc(sizeof(mensajeLocalized));
//	uint32_t offset = 0;
//
//	memcpy(stream+offset, )
//}

void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje){

		void* stream    = malloc(sizeof(mensajeSuscripcionTiempo));
		uint32_t offset = 0;

		memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));
		offset+= sizeof(uint32_t);

		memcpy(stream+offset,&(mensaje->tiempo),sizeof(uint32_t));

		return stream;
}

void* serializarSuscripcion(mensajeSuscripcion* mensaje){

	void* stream    = malloc(sizeof(mensajeSuscripcion));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));

	return stream;
}

paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream){
	paquete* paqueteASerializar       = malloc(sizeof(paquete));
	paqueteASerializar->modulo        = modulo;
	paqueteASerializar->tipoMensaje   = tipoMensaje;
	paqueteASerializar->id            = -1;
	paqueteASerializar->idCorrelativo = -1;
	paqueteASerializar->sizeStream    = sizeStream;
	paqueteASerializar->stream        = stream;

	return paqueteASerializar;
}

void* serializarPaquete(paquete* paqueteASerializar){
	void* paquete   = malloc(sizeof(uint32_t)*5+paqueteASerializar->sizeStream);
	uint32_t offset = 0;

	memcpy(paquete+offset, &(paqueteASerializar->modulo), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->tipoMensaje), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->idCorrelativo), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->sizeStream), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset,paqueteASerializar->stream,paqueteASerializar->sizeStream);

	return paquete;
}

mensajeAppearedBroker* deserializarAppearedBroker(void* streamRecibido){
	mensajeAppearedBroker* mensaje = malloc(sizeof(mensajeAppearedBroker));
	uint32_t offset                = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(&(mensaje->pokemon), streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeAppearedTeam* deserializarAppearedTeam (void* streamRecibido){
	mensajeAppearedTeam* mensaje = malloc(sizeof(mensajeAppearedTeam));
	uint32_t offset              = 0;
	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}
mensajeNewBroker* deserializarNewBroker (void* streamRecibido){
	mensajeNewBroker* mensaje = malloc(sizeof(mensajeNewBroker));
	uint32_t offset 		  = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(&(mensaje->pokemon), streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->cantidad), streamRecibido+offset, sizeof(int32_t));

	return mensaje;
}

mensajeNewGamecard* deserializarNewGamecard (void* streamRecibido){
	mensajeNewGamecard* mensaje = malloc(sizeof(mensajeNewGamecard));
	uint32_t offset				= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(&(mensaje->pokemon), streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->cantidad), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCatchBroker* deserializarCatchBroker (void* streamRecibido){
	mensajeCatchBroker* mensaje = malloc(sizeof(mensajeCatchBroker));
	uint32_t offset 			= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(&(mensaje->pokemon), streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCatchGamecard* deserializarCatchGamecard (void* streamRecibido){
	mensajeCatchGamecard* mensaje = malloc(sizeof(mensajeCatchGamecard));
	uint32_t offset				  = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(&(mensaje->pokemon), streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCaught* deserializarCaught(void* streamRecibido){
	mensajeCaught* mensaje = malloc(sizeof(mensajeCaught));
	uint32_t offset        = 0;

	memcpy(&(mensaje->id), streamRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->resultadoCaught), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeGet* deserializarGet (void* streamRecibido){
	mensajeGet* mensaje = malloc(sizeof(mensajeGet));
	uint32_t offset		= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(&(mensaje->pokemon), streamRecibido+offset, mensaje->sizePokemon);

	return mensaje;
}

//mensajeLocalized* deserializarLocalized (void* streamRecibido){
//
//}

mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido){
	mensajeSuscripcionTiempo* mensaje = malloc(sizeof(mensajeSuscripcionTiempo));
	uint32_t offset 				  = 0;

	memcpy(&(mensaje->cola), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->tiempo), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido){
	mensajeSuscripcion* mensaje = malloc(sizeof(mensajeSuscripcion));

	memcpy(&(mensaje->cola), streamRecibido, sizeof(uint32_t));

	return mensaje;
}

paquete* deserializarPaquete(void* paqueteRecibido){
	paquete* paquete = malloc(sizeof(paquete));
	uint32_t offset  = 0;

	memcpy(&(paquete->modulo), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->tipoMensaje), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->id), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->idCorrelativo), paqueteRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->sizeStream), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	paquete->stream = malloc(paquete->sizeStream);
	memcpy(&(paquete->stream), paqueteRecibido+offset, paquete->sizeStream);

	free(paqueteRecibido);
	return paquete;
}


paquete* recibirPaquete(uint32_t socket){
	paquete* paquete = malloc(sizeof(paquete));
	uint32_t offset  = 0;

	//recv(cliente,&respuesta1,sizeof(uint32_t),0);

	if(recv(socket,&(paquete->modulo),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->tipoMensaje),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->id),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->idCorrelativo),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->sizeStream),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	paquete->stream = malloc(paquete->sizeStream);
	if(recv(socket,(paquete->stream),(paquete->sizeStream),0)==-1)
		return NULL;

	return paquete;
}

uint32_t crearSocketCliente (char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family      = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port        = htons(puerto);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);
	if(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor)) != 0){
		perror("No se pudo conectar");
		return -1;
	}

	return cliente;
}

uint32_t sizeArgumentos (uint32_t colaMensaje, char* nombrePokemon, uint32_t proceso){
	uint32_t size;
	switch(colaMensaje){
	case NEW_POKEMON:
		if(proceso == BROKER){ //size pokemon, pokemon,posx, posy, cantidad
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*4;
		} else if (proceso == GAMECARD){ //size pokemon, pokemon, posX, posY, cantidad, ID
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*5;
		}
		break;

	case APPEARED_POKEMON:
		if(proceso == BROKER){ // sizePokemon, pokemon, posX, posY, ID
			size = strlen(nombrePokemon) +1 + sizeof(uint32_t)*4;
		}else if(proceso == TEAM){ //sizePokemon, pokemon, posX, posY
			size = strlen(nombrePokemon) + 1 +sizeof(uint32_t)*3;
		}
		break;

	case CATCH_POKEMON:
		if(proceso == BROKER){ //sizePokemon, pokemon, posX, posY
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*3;
		}else if (proceso == GAMECARD){ //sizePokemon, pokemon, posX, posY, ID
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*4;
		}
		break;

	case CAUGHT_POKEMON://ID, ok/fail
		size = sizeof(uint32_t)*2;
		break;

	case GET_POKEMON://sizePokemon, pokemon
		size = strlen(nombrePokemon) + 1 + sizeof(uint32_t);
		break;

	default:
		printf("Error: el caso ingresado no esta contemplado \n");
		break;
	}
	return size;
}

uint32_t sizePaquete(paquete* paq){
	return paq->sizeStream+ sizeof(uint32_t)*5;
}

listaMutex inicializarListaMutex(){
	listaMutex list;
	list.lista=list_create();
	list.mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(list.mutex,NULL);
	return list;
}

void addListaMutex(listaMutex list,void* elemento){
	pthread_mutex_lock(list.mutex);
	list_add(list.lista,elemento);
	pthread_mutex_unlock(list.mutex);
}

void* getListaMutex(listaMutex list, uint32_t index){
	pthread_mutex_lock(list.mutex);
	void* value=list_get(list.lista,index);
	pthread_mutex_unlock(list.mutex);
	return value;
}

uint32_t sizeListaMutex(listaMutex list){
	pthread_mutex_lock(list.mutex);
	uint32_t size=list_size(list.lista);
	pthread_mutex_unlock(list.mutex);
	return size;
}

void removeListaMutex(listaMutex list,uint32_t pos){
	pthread_mutex_lock(list.mutex);
	list_remove(list.lista,pos);
	pthread_mutex_unlock(list.mutex);
}

void destruirListaMutex(listaMutex* lista){

}

colaMutex inicializarColaMutex(){
	colaMutex cola;
	cola.cola=queue_create();
	cola.mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(cola.mutex,NULL);
	return cola;
}

void pushColaMutex(colaMutex cola, void* infoEntrenador){
	pthread_mutex_lock(cola.mutex);
	queue_push(cola.cola,infoEntrenador);
	pthread_mutex_unlock(cola.mutex);

}

void* popColaMutex(colaMutex cola){
	pthread_mutex_lock(cola.mutex);
	void* elemento=queue_pop(cola.cola);
	pthread_mutex_unlock(cola.mutex);
	return elemento;

}

void destruirColaMutex(colaMutex* cola){

}


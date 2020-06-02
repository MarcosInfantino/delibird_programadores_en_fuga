/*
 * gamecard.c
 *
 *  Created on: 12 may. 2020
 *      Author: juancito
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include "gamecard.h"

uint32_t puertoBroker;
char* ipBroker;
uint32_t tiempoReconexion;
uint32_t puertoGamecard = 5001;

int main(void){
	t_config * config = config_create("Gamecard.config");
	puertoBroker= config_get_int_value(config,"PUERTO_BROKER");
	ipBroker= config_get_string_value(config,"IP_BROKER");

	FILE *file;
	file = fopen("metadata.bin","w+b");
	fputs("F", file);

	suscribirseColasBroker(config);

	return EXIT_SUCCESS;
}




void* suscribirseColasBroker(void* config){

	t_config* confi=(t_config*) config;
	tiempoReconexion =config_get_int_value(confi, "TIEMPO_DE_REINTENTO_CONEXION");

	mensajeSuscripcion* mensajeNew=malloc(sizeof(mensajeSuscripcion));
	mensajeSuscripcion * mensajeCatch=malloc(sizeof(mensajeSuscripcion));
	mensajeSuscripcion* mensajeGet=malloc(sizeof(mensajeSuscripcion));

	mensajeNew->cola  = NEW_POKEMON;

	mensajeCatch ->cola  = CATCH_POKEMON;

	mensajeGet->cola = GET_POKEMON;

	pthread_t threadSuscripcionNew;
	pthread_create(&threadSuscripcionNew, NULL, suscribirseCola, (void*)mensajeNew);
	pthread_detach(threadSuscripcionNew);

	pthread_t threadSuscripcionCatch;
	pthread_create(&threadSuscripcionCatch, NULL, suscribirseCola, (void*)mensajeCatch);
	pthread_detach(threadSuscripcionCatch);

	pthread_t threadSuscripcionGet;
	pthread_create(&threadSuscripcionGet, NULL, suscribirseCola, (void*)mensajeGet);
	pthread_detach(threadSuscripcionGet);

	while(1);
	free(confi);
	return NULL;
}


void* suscribirseCola(void* msgSuscripcion){
	mensajeSuscripcion* msg=(mensajeSuscripcion*)msgSuscripcion;
	uint32_t sizeStream = sizeof(uint32_t);
	void* streamMsgSuscripcion = serializarSuscripcion(msg);
	printf("Voy a llenar el paquete\n");
	paquete* paq = llenarPaquete(GAMECARD,SUSCRIPCION,sizeStream, streamMsgSuscripcion);

	struct sockaddr_in direccionServidor;
		direccionServidor.sin_family      = AF_INET;
		direccionServidor.sin_addr.s_addr = inet_addr(ipBroker);
		direccionServidor.sin_port        = htons(puertoBroker);

		uint32_t cliente = socket(AF_INET,SOCK_STREAM,0);
		printf("cliente: %d\n",cliente);
		while(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))<0){
			printf("Conexión fallida con el Broker reintentando en %i segundos...\n",tiempoReconexion);
			sleep(tiempoReconexion);
		}



	printf("Comienzo suscripcion a %i \n", paq->tipoMensaje);
	uint32_t bytes = sizeof(uint32_t)*5+paq->sizeStream;

	void* stream   = serializarPaquete(paq);

	//free(streamMsgSuscripcion);

	send(cliente,stream,bytes,0);

	//free(stream);

	uint32_t respuesta;
	printf("Espero respuesta\n");
	recv(cliente,&respuesta,sizeof(uint32_t),0);

		if(respuesta == CORRECTO){
			printf("Mensaje recibido correctamente\n");

		}else{
			printf("Mensaje recibido incorrectamente\n");
			printf("mensaje: %i\n", respuesta);


		}
	while(1){
		paquete* paqueteRespuesta=recibirPaquete(cliente);
		switch(paqueteRespuesta->tipoMensaje){
			case NEW_POKEMON:
				printf("Soy un New Breoo\n");
				//CAMBIAR ACA mensajeAppearedTeam* msgAppeared=deserializarAppearedTeam(paqueteRespuesta->stream);
				break;
			case GET_POKEMON:
				printf("Soy un Get Breoo\n");
				//ATENDER GET
				break;
			case CATCH_POKEMON:
				printf("Soy un Catch Breoo\n");
				//CAMBIAR ACAatenderCaught(paqueteRespuesta);
				break;
			default:
				break; //esto no puede pasar
		}
	}


		return NULL;
}

int crearHiloServidorGameboy(pthread_t* hilo){
	uint32_t err=pthread_create(hilo,NULL,iniciarServidorGameboy,NULL);
					if(err!=0){
						printf("Hubo un problema en la creación del hilo para iniciar el servidor para el Gameboy \n");
						return err;
					}

		pthread_detach(*hilo);
	return 0;
}

void* iniciarServidorGameboy(void* arg){
		struct sockaddr_in direccionServidor;
		direccionServidor.sin_family=AF_INET;
		direccionServidor.sin_addr.s_addr=INADDR_ANY;
		direccionServidor.sin_port=htons(puertoGamecard);

		uint32_t servidor=socket(AF_INET,SOCK_STREAM,0);

		if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))!=0){
			perror("Falló el bind");

		}else{
		printf("Estoy escuchando\n");
		while (1)  								//para recibir n cantidad de conexiones
				esperar_cliente(servidor);
		}
	return NULL;
}

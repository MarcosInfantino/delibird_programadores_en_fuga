/*
 ============================================================================
 Name        : brokerSO.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "broker.h"

pthread_mutex_t mutex;


colaMensajes appearedPokemon,
			 newPokemon,
			 caughtPokemon,
			 catchPokemon,
			 getPokemon,
			 localizedPokemon;

uint32_t idHiloAppearedPokemon, idHiloNewPokemon, idHiloCaughtPokemon,
	idHiloCatchPokemon, idHiloGetPokemon, idHiloLocalizedPokemon;

pthread_t hiloAppearedPokemon, hiloNewPokemon, hiloCaughtPokemon,
		  hiloCatchPokemon, hiloGetPokemon, hiloLocalizedPokemon;


int main(void) {
//	printf("hola");
//	char* pathConfig="/home/utnso/Escritorio/repoSO/tp-2020-1c-Programadores-en-Fuga/Broker/src/broker.config";
//	t_config* config=config_create(pathConfig);
//	printf("hola");
//	puertoBroker=config_get_int_value(config, "PUERTO_BROKER​");
//	printf("hola");
//	ipBroker=config_get_string_value(config, "IP_BROKER");
//	printf("hola");
	iniciarHilos();

	struct sockaddr_in direccionServidor;
		direccionServidor.sin_family      = AF_INET;
		direccionServidor.sin_addr.s_addr = INADDR_ANY;
		direccionServidor.sin_port        = htons(5002);


		int servidor=socket(AF_INET,SOCK_STREAM,0);

		int activado=1;
		setsockopt(servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

		if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))!=0){
			perror("Falló el bind");
			return 1;
		}

		printf("Estoy escuchando\n");
		listen(servidor,100);


//		struct sockaddr_in direccionCliente;
//		unsigned int tamanioDireccion=sizeof(direccionCliente);
//		int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);
//
//		printf("Se ha recibido una conexión en %d.\n", cliente);
//
//
//		void *buffer = malloc(100);
//
//		while(1){
//			int bytesRecibidos = recv(cliente,buffer, 99,0);
//
//			if(bytesRecibidos<=0){
//				perror("Error");
//				return 1;
//			}
//
//
//
//			printf("Me llegaron %d bytes con %s\n", bytesRecibidos,(char*)buffer);
//
//		}
//
//		free(buffer);

		while(1)  //para recibir n cantidad de conexiones
			esperar_cliente(servidor);

	return EXIT_SUCCESS;
}


void* iniciarCola(void* c){
	colaMensajes* cc=(colaMensajes*)c;
	cc->cola = malloc(sizeof(t_queue));
	cc->suscriptores = malloc(sizeof(t_list));
	return NULL;
}

void iniciarHilos(){

		idHiloAppearedPokemon = pthread_create(&hiloAppearedPokemon, NULL, iniciarCola, (void*)&appearedPokemon);
		idHiloNewPokemon = pthread_create(&hiloNewPokemon, NULL, iniciarCola, (void*)&newPokemon);
		idHiloCaughtPokemon = pthread_create(&hiloCaughtPokemon, NULL, iniciarCola, (void*)&caughtPokemon);
		idHiloCatchPokemon = pthread_create(&hiloCatchPokemon, NULL, iniciarCola, (void*)&catchPokemon);
		idHiloGetPokemon = pthread_create(&hiloGetPokemon, NULL, iniciarCola, (void*)&getPokemon);
		idHiloLocalizedPokemon = pthread_create(&hiloLocalizedPokemon, NULL, iniciarCola, (void*)&localizedPokemon);


		pthread_join(hiloAppearedPokemon, NULL);
		pthread_join(hiloNewPokemon, NULL);
		pthread_join(hiloCaughtPokemon, NULL);
		pthread_join(hiloCatchPokemon, NULL);
		pthread_join(hiloGetPokemon, NULL);
		pthread_join(hiloLocalizedPokemon, NULL);
}


void esperar_cliente(uint32_t socket_servidor){

	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion  = sizeof(struct sockaddr_in);
	printf("Espero un nuevo cliente\n");
	uint32_t socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	printf("Gestiono un nuevo cliente\n");
	pthread_create(&thread,NULL,atenderCliente,(void*)(&socket_cliente));
	pthread_detach(thread);
}


void* atenderCliente(void* sock){
	uint32_t* socket=(uint32_t*) sock;
	uint32_t  modulo;
	uint32_t  esSuscripcion;

	if(recv(*socket, &modulo, sizeof(int), MSG_WAITALL) == -1) //si hubo un error al recibir
		{modulo = -1; printf("error recv\n");}
	printf("recibi un mensaje\n");
	recv(*socket, &esSuscripcion, sizeof(int), MSG_WAITALL);

	if (esSuscripcion  == SUSCRIPCION){ //se quiere suscribir a alguna cola
		suscribirSegunCola(modulo, *socket);}
	else{
		manejarTipoDeMensaje(modulo, *socket);}
	return NULL;
}

//me parece que acá va a convenir que sea por cola y no por modulo, segun la cola es el tipo de mensaje
void manejarTipoDeMensaje(uint32_t modulo, uint32_t cliente_fd) {
	/*switch(cola){
		case APPEARED_POKEMON:
			deserializarAppeared(cliente_fd);
			break;
		case NEW_POKEMON:
			deserializarNew(cliente_fd);
			break;
		case CAUGHT_POKEMON:
			deserializarCaught(cliente_fd);
			break;
		case CATCH_POKEMON:
			deserializarCatch(cliente_fd);
			break;
		case GET_POKEMON:
			deserializarGet(cliente_fd);
			break;
		case LOCALIZED_POKEMON:
			deserializarLocalized(cliente_fd);
		}*/
	switch (modulo) {
		case TEAM:
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void suscribirSegunCola( uint32_t modulo, uint32_t socket){
	//uint32_t  id_proceso;
	uint32_t  cola;

	//recv(socket, &id_proceso, sizeof(int), MSG_WAITALL);
	recv(socket, &cola, sizeof(int), MSG_WAITALL);
	printf("Hice el recv de la cola\n");
	switch(cola){
	case APPEARED_POKEMON:
		suscribir(modulo, appearedPokemon, socket, cola);
		break;
	case NEW_POKEMON:
		suscribir(modulo, newPokemon, socket, cola);
		break;
	case CAUGHT_POKEMON:
		suscribir(modulo, caughtPokemon, socket, cola);
		break;
	case CATCH_POKEMON:
		suscribir(modulo, catchPokemon, socket, cola);
		break;
	case GET_POKEMON:
		suscribir(modulo, getPokemon, socket, cola);
		break;
	case LOCALIZED_POKEMON:
		suscribir(modulo, localizedPokemon, socket, cola);
	}
	//enviar todos los mensajes que hubiesen en la cola antes de suscribirse
	//cuando se envien mensajes que no sean suscripción asignarles un numero para posibbles respuestas en otra cola

}

void responderMensaje(uint32_t socketCliente, uint32_t respuesta){
	void* stream=malloc(sizeof(uint32_t));
	memcpy(stream, &respuesta, sizeof(uint32_t));
	send(socketCliente,(void*)(&respuesta),sizeof(uint32_t),0);

}

void suscribir(uint32_t modulo, colaMensajes structCola, uint32_t socketCliente, uint32_t colaEnum){

	if(validarSuscripcionSegunModulo(modulo, colaEnum) && !validarPertenencia(structCola, socketCliente)){ //si se puede suscribir y aun no esta en la cola
		list_add(structCola.suscriptores , (void*) (&socketCliente));
		responderMensaje(socketCliente, CORRECTO);
		printf("respondi mensaje correcto\n");
	}else{
		responderMensaje(socketCliente, INCORRECTO);
		printf("respondi mensaje incorrecto\n");
	}

}

bool validarSuscripcionSegunModulo(uint32_t modulo, uint32_t cola){

	switch(modulo){
	case TEAM:
		if(cola == APPEARED_POKEMON || cola == CAUGHT_POKEMON || cola == LOCALIZED_POKEMON)
			{printf("Soy team\n"); return true;}
		break;
	case GAMECARD:
		if(cola == NEW_POKEMON || cola == GET_POKEMON || cola == CATCH_POKEMON)
			return true;
		break;
	case GAMEBOY: //acepta cualquier cola
		return true;
	default:
		return false;
	}

	return false; //tiraba error de sin retorno, no sé si está bien
}

bool validarPertenencia(colaMensajes cola, uint32_t socket){
	uint32_t i;
	void* socketLista;
	for(i = 0; i< list_size(cola.suscriptores); i++){
		socketLista = list_get(cola.suscriptores, i);
		if(*((uint32_t*)socketLista) == socket)
			{printf("Me aprobo validar pertenencia\n");return true;}
	}
	printf("Me rechazo validar pertenencia\n");
	return false;

}

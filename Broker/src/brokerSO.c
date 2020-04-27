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
//
colaMensajes appearedPokemon,
			 newPokemon,
			 caughtPokemon,
			 catchPokemon,
			 getPokemon,
			 localizedPokemon;

int idHiloAppearedPokemon, idHiloNewPokemon, idHiloCaughtPokemon,
	idHiloCatchPokemon, idHiloGetPokemon, idHiloLocalizedPokemon;

pthread_t hiloAppearedPokemon, hiloNewPokemon, hiloCaughtPokemon,
		  hiloCatchPokemon, hiloGetPokemon, hiloLocalizedPokemon;


int main(void) {

	iniciarHilos();

	struct sockaddr_in direccionServidor;
		direccionServidor.sin_family      = AF_INET;
		direccionServidor.sin_addr.s_addr = INADDR_ANY;
		direccionServidor.sin_port        = htons(8987);


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


void esperar_cliente(int socket_servidor){
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion  = sizeof(struct sockaddr_in);

	uint32_t socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)atenderCliente,&socket_cliente);
	pthread_detach(thread);
}


void atenderCliente(int* socket)
{
	uint32_t  modulo;
	uint32_t  tipoMensaje;

	if(recv(*socket, &modulo, sizeof(int), MSG_WAITALL) == -1) //si hubo un error al recibir
		modulo = -1;

	recv(*socket, &tipoMensaje, sizeof(int), MSG_WAITALL)

	if (tipoMensaje  == 1){ //se quiere suscribir a alguna cola
		suscribirCola(modulo, socket);}
	else{
		manejarTipoDeModulo(modulo, *socket);}
}
//me parece que acá va a convenir que sea por cola y no por modulo, segun la cola es el tipo de mensaje
void manejarTipoDeModulo(int modulo, int cliente_fd) {

		switch (modulo) {
		case TEAM:
//			tratarTeam
//			responderMensajeOK(cliente_fd);
			break;
		/*case GAMECARD:
			break;
		  case GAMEBOY:
		  	break;*/
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void suscribirCola( uint32_t modulo, uint32_t socket){
	uint32_t  id_proceso;
	uint32_t  cola;

	recv(*socket, &id_proceso, sizeof(int), MSG_WAITALL)
	recv(*socket, &cola, sizeof(int), MSG_WAITALL)

	//agregar validacion de si el que se quiere suscribir a cierta cola pueda hacerlo
	//agregar validación de si ya esta en la lista

	switch(cola){
	case APPEARED_POKEMON:
		list_add(appearedPokemon.suscriptores , (void*)* id_proceso);
		break;
	case NEW_POKEMON:
		list_add(newPokemon.suscriptores , (void*)* id_proceso);
		break;
	case CAUGHT_POKEMON:
		list_add(caughtPokemon.suscriptores , (void*)* id_proceso);
		break;
	case CATCH_POKEMON:
		list_add(catchPokemon.suscriptores , (void*)* id_proceso);
		break;
	case GET_POKEMON:
		list_add(getPokemon.suscriptores , (void*)* id_proceso);
		break;
	case LOCALIZED_POKEMON:
		list_add(localizedPokemon.suscriptores , (void*)* id_proceso);
	}

	responderMensajeOK(socket);
	//enviar todos los mensajes que hubiesen en la cola antes de suscribirse
	//agregar_a_cola(nombre de cola, id proceso)


	//cuando se envien mensajes que no sean suscripción asignarles un numero para posibbles respuestas en otra cola
}

void responderMensajeOK(uint32_t socketCliente){

	t_paquete* paquete      = malloc(sizeof(t_paquete));
	paquete->respuesta      = RESPUESTAOK;
	paquete->buffer->size   = strlen("ok")+1;
	paquete->buffer->stream = memcpy(buffer->stream,"ok",buffer->size);

	void* a_enviar = serializar_paquete(paquete,sizeof(respuesta_broker)+buffer->size+sizeof(buffer->size))
	send(socketCliente,a_enviar,sizeof(respuesta_broker)+buffer->size+sizeof(buffer->size),0);
	contadorMensajes + 1;

}

//void agregar_a_cola(uint32_t cola, uint32_t id_proceso){
//
//
//	list_add(cola.suscriptores , (void*)* id_proceso);
//	chequearYenviarMensajes()
//}



/*---------------protocolo suscripcion---------------*/
/*modulo   | tipo_mensaje | cola | id_proceso
        int(enum)
   *1 si es subscripcion 0 si no lo es*/


/*---------------protocolo get pokemon---------------*/
/*modulo  | tipo_mensaje | cola | nombre_pokemon */


/*---------------protocolo Caught Pokemon---------------*/
/*modulo   | tipo_mensaje | cola | int(0 o 1) */
/*int         int              string           int   int*/



/*---------------protocolo Catch Pokemon---------------*/
/*modulo   | tipo_mensaje | cola | nombre_pokemon  | posx | posy */
/*int         int              string           int   int*/



/*---------------protocolo Appeared Pokemon---------------*/
/*modulo   | tipo_mensaje | cola | nombre_pokemon  | posx | posy */
/*int         int              string           int   int*/


/*---------------protocolo Localized Pokemon---------------*/
/*modulo   | tipo_mensaje | cola | cant_pokemon | posx | pos y*/
/*int         int            int            int   int*/

/* van a haber n cantidad de pares de x e y siendo n = cant_pokemon*/



/*---------------protocolo New Pokemon---------------*/
/*modulo | tipo_mensaje |  cola| pos x | pos y | cant_pokemon */
/*int         int     int       int*/






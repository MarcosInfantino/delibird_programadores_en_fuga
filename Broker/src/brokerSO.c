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
		direccionServidor.sin_family=AF_INET;
		direccionServidor.sin_addr.s_addr=INADDR_ANY;
		direccionServidor.sin_port=htons(8987);


		int servidor=socket(AF_INET,SOCK_STREAM,0);

		int activado=1;
		setsockopt(servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

		if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))!=0){
			perror("Falló el bind");
			return 1;
		}
		printf("Estoy escuchando\n");
		listen(servidor,100);


		struct sockaddr_in direccionCliente;
		unsigned int tamanioDireccion=sizeof(direccionCliente);
		int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

		printf("Se ha recibido una conexión en %d.\n", cliente);


		void *buffer=malloc(100);

		while(1){
			int bytesRecibidos = recv(cliente,buffer, 99,0);

			if(bytesRecibidos<=0){
				perror("Error");
				return 1;
			}



			printf("Me llegaron %d bytes con %s\n", bytesRecibidos,(char*)buffer);

		}

		free(buffer);

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



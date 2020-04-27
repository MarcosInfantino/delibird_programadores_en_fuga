/*
 ============================================================================
 Name        : Team.c
 Author      : Programadores en fuga
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Team.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

//fecha ult modificacion 27 de abril

//t_list* idsHilos=list_create();//son ints
//t_list* hilos=list_create();//son pthread_t

int socketColasBroker=8987;
int socketGameboy;
int socketGamecard;


//
//int main(void) {
////	char* str="[Pikachu|Squirtle|Pidgey, Squirtle|Charmander, Bulbasaur]";
////
////	t_list* lst=obtenerListaDeListas(str);
////	t_list* primerLista=arrayStringALista(((char**)list_get(lst,0)));
////
////	printf("%s\n", (char*) (list_get(primerLista,2)));
//	char* pathConfig="Team2.config";
//	t_config* config=config_create(pathConfig);
//	dataTeam* t=inicializarTeam(config);
//
//	printf("No se colgo\n");
//
////	t_list* prueba=list_create();
//
//	int pos=1;
//	printf("Objetivo : %s\n",((objetivo*)list_get(t->objetivoGlobal,pos))->pokemon);
//	printf("Cantidad : %i\n",((objetivo*)list_get(t->objetivoGlobal,pos))->cantidad);
//
//	return EXIT_SUCCESS;
//}

//int main(void){
//	int i=0;
//	char* aux="Pikachu|Squirtle|Pidgey";
//	char** lst=string_split(aux, "|");
//	printf("%s",lst[i]);
//	return 0;
//}

//
int main(int argc , char* argv[]){
	//char pathConfig= argv;
	char* pathConfig="Team2.config";
	t_config* config=config_create(pathConfig);

	dataTeam* t=inicializarTeam(config);
	uint32_t cantEntrenadores=list_size(t->entrenadores);
	pthread_t arrayIdHilos[cantEntrenadores];
	inicializarEntrenadores(t->entrenadores,arrayIdHilos);
	pthread_t hiloConexionInicialBroker;
	crearHiloConexionColasBroker((void*)config,&hiloConexionInicialBroker);
	pthread_t hiloServidorGameboy;
	crearHiloServidorGameboy(&hiloServidorGameboy);

	printf("hola\n");


	while(1);





	return 0;
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
		direccionServidor.sin_port=htons(8080);

		int servidor=socket(AF_INET,SOCK_STREAM,0);

		int activado=1;
		setsockopt(servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

		if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))!=0){
			perror("Falló el bind");

		}else{
		printf("Estoy escuchando\n");
		listen(servidor,100);
		}

		struct sockaddr_in direccionCliente;
		unsigned int tamanioDireccion=sizeof(direccionCliente);
		int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

		printf("Se ha recibido una conexión en %d.\n", cliente);

	return NULL;
}

int crearHiloConexionColasBroker(void* config, pthread_t* hilo){
	uint32_t err=pthread_create(hilo,NULL,suscribirseColasBroker,(void*)config);
				if(err!=0){
					printf("Hubo un problema en la creación del hilo para conectarse al broker \n");
					return err;
				}

	pthread_detach(*hilo);
	return 0;
}

void* suscribirseColasBroker(void* conf){

	t_config* config=(t_config*) conf;
	uint32_t tiempoReconexion =config_get_int_value(config, "TIEMPO_RECONEXION");
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family=AF_INET;
	direccionServidor.sin_addr.s_addr=inet_addr("127.0.0.1");
	direccionServidor.sin_port=htons(socketColasBroker);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);

	while(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))!=0){
		printf("Conexión fallida con el Broker reintentando en %i segundos...\n",tiempoReconexion);
		sleep(tiempoReconexion);
	}
	uint32_t modulo= TEAM;
	uint32_t tipoMensaje=MENSAJE_SUSCRIPCION;
	uint32_t idProceso=getpid();

	cola colaAppeared=APPEARED_POKEMON;
	cola colaNew=NEW_POKEMON;
	cola colaCaught=CAUGHT_POKEMON;
	cola colaCatch=CATCH_POKEMON;
	cola colaGet=GET_POKEMON;
	cola colaLocalized=LOCALIZED_POKEMON;

	suscribirseCola(&modulo,&tipoMensaje,&idProceso,&colaAppeared,cliente);
	suscribirseCola(&modulo,&tipoMensaje,&idProceso,&colaNew,cliente);
	suscribirseCola(&modulo,&tipoMensaje,&idProceso,&colaCaught,cliente);
	suscribirseCola(&modulo,&tipoMensaje,&idProceso,&colaCatch,cliente);
	suscribirseCola(&modulo,&tipoMensaje,&idProceso,&colaGet,cliente);
	suscribirseCola(&modulo,&tipoMensaje,&idProceso,&colaLocalized,cliente);

	return NULL;
}

void suscribirseCola(uint32_t* modulo,uint32_t* tipoMensaje,uint32_t* idProceso, uint32_t* cola, uint32_t socket){
	uint32_t bytes=sizeof(uint32_t)*4;

	void* stream=malloc(bytes);
	uint32_t offset=0;

	memcpy(stream+offset,modulo,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(stream+offset,tipoMensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(stream+offset,idProceso,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(stream+offset,cola,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	send(socket,stream,bytes,0);

	free(stream);


}

int inicializarEntrenadores(t_list* entrenadores, pthread_t arrayIdHilos[]){
	uint32_t i;
	for(i=0;i<list_size(entrenadores);i++){
		void* entrenadorActual=list_get(entrenadores,i);
		uint32_t err=pthread_create(&(arrayIdHilos[i]),NULL,ejecucionHiloEntrenador,entrenadorActual);
		if(err!=0){
			printf("Hubo un problema en la creación del hilo del entrenador \n");
			return err;
		}
//		}else{
//			printf("Todo bien\n");
//		}
		pthread_join(arrayIdHilos[i],NULL);
	}
	return 0;
}


void* ejecucionHiloEntrenador(void* arg){



	return NULL;
}



dataTeam* inicializarTeam(t_config* config){

	dataTeam* dataTeam=malloc(sizeof(dataTeam));
	dataTeam->entrenadores=list_create();
	t_list* especiesObjetivo=list_create();

	dataTeam->objetivosCumplidos=list_create();

	char** arrayPosicionesEntrenadores=config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** arrayPokemonesEntrenadores=config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** arrayObjetivosEntrenadores=config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	t_list* posicionesEntrenadores=obtenerListaDeListas(arrayPosicionesEntrenadores);
	t_list* pokemonesEntrenadores=obtenerListaDeListas(arrayPokemonesEntrenadores);
	t_list* objetivosEntrenadores=obtenerListaDeListas(arrayObjetivosEntrenadores);
	uint32_t cantEntrenadores=list_size(posicionesEntrenadores);

	uint32_t i;

	for(i=0;i<cantEntrenadores;i++){
		dataEntrenador* dataEntrenador=malloc(sizeof(dataEntrenador));
		char** pos=list_get(posicionesEntrenadores,i);
		char** pokemones=list_get(pokemonesEntrenadores,i);
		char** objetivos=list_get(objetivosEntrenadores,i);

		(dataEntrenador->posicion).x=atoi(pos[0]);
		(dataEntrenador->posicion).y=atoi(pos[1]);

		dataEntrenador->pokemones=arrayStringALista(pokemones);
		dataEntrenador->objetivoPersonal=arrayStringALista(objetivos);

		t_list* pokemonesEntrenadorAux=arrayStringALista(pokemones);
		t_list* objetivoPersonalEntrenadorAux=arrayStringALista(objetivos);
		uint32_t i;
		//for(i=0;i<list_size(pokemonesEntrenadorAux);i++){
		for(i=0;i<list_size(pokemonesEntrenadorAux);i++){
			char *pokemonAComparar=(char*) list_get(pokemonesEntrenadorAux,i);
			uint32_t encontrado=buscarMismoPokemon(objetivoPersonalEntrenadorAux,pokemonAComparar);

			if(encontrado!=-1){
				list_remove(objetivoPersonalEntrenadorAux,encontrado);
			}
		}

		for(i=0;i<list_size(objetivoPersonalEntrenadorAux);i++){

						list_add(especiesObjetivo,list_get(objetivoPersonalEntrenadorAux,i));

				}
		dataEntrenador->estado=NEW;

		list_add(dataTeam->entrenadores,dataEntrenador);


	}
	dataTeam->objetivoGlobal=obtenerObjetivos(especiesObjetivo);
	return dataTeam;

}

uint32_t buscarMismoPokemon(t_list* lst, char* pokemon){//devuelve la posicion en la que encontro el pokemon

	uint32_t i;
	for(i=0;i<list_size(lst);i++){
		char* pokemonAComparar=(char*)list_get(lst,i);
		if(strcmp(pokemonAComparar,pokemon)==0){
			return i;
		}
	}

	return -1;
}


t_list* arrayStringALista(char** arr){
	uint32_t i;
	t_list* lst=list_create();
	for(i=0;arr[i]!=NULL;i++){
		list_add(lst,(void*) arr[i]);
	}
	return lst;
}


//t_list* obtenerObjetivos(t_list* especies){
//	t_list* objetivos=list_create();
//
//
//	for(uint32_t i=0;i<list_size(especies);i++){
//		void* especie=list_get(especies,i);
//
//		especieAComparar=especie;
//
//		void* encontrado=list_find(objetivos,objetivoMismaEspecie);
//		if(encontrado==NULL){
//			objetivo* objetivo=malloc(sizeof(objetivo));
//			objetivo->cantidad=1;
//			objetivo->pokemon= (char*) especie;
//			//printf("%s\n",(char*)(objetivo->pokemon));
//			list_add(objetivos,(void*)objetivo);
//		}else{
//			(((objetivo*)encontrado)->cantidad)++;
//		}
//	}
//	return objetivos;
//
//}

t_list* obtenerObjetivos(t_list* especies){
	t_list* objetivos=list_create();


	for(uint32_t i=0;i<list_size(especies);i++){
		char* especie=(char*)list_get(especies,i);

		//especieAComparar=especie;

		uint32_t encontrado=buscarObjetivoPorEspecie(objetivos,especie);
		if(encontrado==-1){
			objetivo* objetivo=malloc(sizeof(objetivo));
			objetivo->cantidad=1;
			objetivo->pokemon= (char*) especie;
			//printf("%s\n",(char*)(objetivo->pokemon));
			list_add(objetivos,(void*)objetivo);
		}else{
			(((objetivo*)list_get(objetivos,encontrado))->cantidad)++;
		}
	}
	return objetivos;

}

uint32_t buscarObjetivoPorEspecie(t_list* listaObjetivos, char* especie){
	uint32_t i;
	for(i=0;i<list_size(listaObjetivos);i++){
		objetivo* obj=(objetivo*)list_get(listaObjetivos,i);
		if(strcmp(obj->pokemon,especie)==0){
			return i;
		}
	}
	return -1;
}

//bool objetivoMismaEspecie(void* obj){
//	objetivo* objAux=(objetivo*) obj;
//	return strcmp(objAux->pokemon,(char*)especieAComparar)==0;
//}


t_list* obtenerListaDeListas(char** lst){
	t_list* lstDeLst=list_create();
	uint32_t i=0;
	char* aux=lst[i];
	while(aux!=NULL){
		char** str=string_split(aux, "|");
		list_add(lstDeLst,(void**)str);
		i++;
		aux=lst[i];
	}
	return lstDeLst;
}

//t_list* obtenerListaDeListas(char* str){//me devuelve una lista en la cual cada elemento es un array de strings (char**)
//
//			int length_value = strlen(str) - 2;
//			char* cadena = string_substring(str, 1, length_value);
//			t_list* lst=list_create();
//			int i;
//			int j=0;
//			bool sublistaTerminada=false;
//
//			char* buffer=malloc(200);
//			for(i=0;i<strlen(cadena);i++){
//
//				if(sublistaTerminada){
//					sublistaTerminada=false;
//				}
//				else if(cadena[i]!=']'){
//					buffer[j]=cadena[i];
//					j++;
//				}else{
//					buffer[j]=cadena[i];
//					buffer[j+1]='\0';
//					j=0;
//					string_trim(&buffer);
//					char** arr= string_get_string_as_array(buffer);
//					list_add(lst,(void**)arr);
//					buffer=malloc(200);
//					sublistaTerminada=true;
//
//				}
//			}
//
//	return lst;
//}

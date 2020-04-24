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
#include <commons/config.h>
#include <pthread.h>
#include <stdbool.h>

//t_list* idsHilos=list_create();//son ints
//t_list* hilos=list_create();//son pthread_t
bool esMayorQue1(void* i){
	return *((int*)i)>1;
}
int main(void) {
	char* str="[[Pikachu, Squirtle, Pidgey], [Squirtle, Charmander], [Bulbasaur]]";

	t_list* lst=obtenerListaDeListas(str);
	t_list* primerLista=arrayStringALista(((char**)list_get(lst,0)));

	printf("%s\n", (char*) (list_get(primerLista,2)));

	dataTeam* t=inicializarTeam("Team1.config");

	printf("No se colgo\n");

	t_list* prueba=list_create();

	int pos=4;
	printf("Objetivo : %s\n",((objetivo*)list_get(t->objetivoGlobal,pos))->pokemon);
	printf("Cantidad : %i\n",((objetivo*)list_get(t->objetivoGlobal,pos))->cantidad);

	return EXIT_SUCCESS;
}



dataTeam* inicializarTeam(char* path){
	t_config* config=config_create(path);
	dataTeam* dataTeam=malloc(sizeof(dataTeam));
	dataTeam->entrenadores=list_create();
	t_list* especiesObjetivo=list_create();

	dataTeam->objetivosCumplidos=list_create();

	char* stringPosicionesEntrenadores=config_get_string_value(config,"POSICIONES_ENTRENADORES");
	char* stringPokemonesEntrenadores=config_get_string_value(config,"POKEMON_ENTRENADORES");
	char* stringObjetivosEntrenadores=config_get_string_value(config,"OBJETIVOS_ENTRENADORES");

	t_list* posicionesEntrenadores=obtenerListaDeListas(stringPosicionesEntrenadores);
	t_list* pokemonesEntrenadores=obtenerListaDeListas(stringPokemonesEntrenadores);
	t_list* objetivosEntrenadores=obtenerListaDeListas(stringObjetivosEntrenadores);
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

		uint32_t i;
		for(i=0;i<list_size(dataEntrenador->objetivoPersonal);i++){

			list_add(especiesObjetivo,list_get(dataEntrenador->objetivoPersonal,i));
		}

		dataEntrenador->estado=NEW;

		list_add(dataTeam->entrenadores,dataEntrenador);


	}
	dataTeam->objetivoGlobal=obtenerObjetivos(especiesObjetivo);
	return dataTeam;

}

void* inicializarEntrenador(void* arg){
	dataEntrenador* data= (dataEntrenador*) arg;

	return NULL;
}

t_list* arrayStringALista(char** arr){
	uint32_t i;
	t_list* lst=list_create();
	for(i=0;arr[i]!=NULL;i++){
		list_add(lst,(void*) arr[i]);
	}
	return lst;
}

//int** mostrarPosicionesEntrenadores(char* path){
//	t_config* config=config_create(path);
//			char** posicionesEntrenadores=config_get_array_value(config,"POSICIONES_ENTRENADORES");
//			int**list=malloc();
//			for (int i = 0 ; i < sizeof(posicionesEntrenadores)/2; i ++ ){
//
//				printf("x %c\n", posicionesEntrenadores[i*2][1]);
//		        printf("y %c\n" ,posicionesEntrenadores[i*2+1][0]);
//			}
//}


//void inicializarHilosEntrenadores(uint32_t limite){
//	uint32_t i=0;
//	while(i<limite){
//		uint32_t* idHilo=malloc(sizeof(uint32_t));
//		pthread_t* hilo=malloc(sizeof(pthread_t));
//		idHilo = pthread_create(hilo, NULL, inicializarSiguienteEntrenador, NULL);
//		pthread_join(*hilo, NULL);
//		list_add(idsHilos,(void*)idHilo);
//		list_add(hilos,(void*)hilo);
//	}
//}

//t_list* especiesSinRepeticion(t_list* especies){
//
//}
//
t_list* obtenerObjetivos(t_list* especies){
	t_list* objetivos=list_create();


	for(uint32_t i=0;i<list_size(especies);i++){
		void* especie=list_get(especies,i);

		especieAComparar=especie;

		void* encontrado=list_find(objetivos,objetivoMismaEspecie);
		if(encontrado==NULL){
			objetivo* objetivo=malloc(sizeof(objetivo));
			objetivo->cantidad=1;
			objetivo->pokemon= (char*) especie;
			//printf("%s\n",(char*)(objetivo->pokemon));
			list_add(objetivos,(void*)objetivo);
		}else{
			(((objetivo*)encontrado)->cantidad)++;
		}
	}
	return objetivos;

}

bool objetivoMismaEspecie(void* obj){
	objetivo* objAux=(objetivo*) obj;
	return strcmp(objAux->pokemon,(char*)especieAComparar)==0;
}




t_list* obtenerListaDeListas(char* str){//me devuelve una lista en la cual cada elemento es un array de strings (char**)

			int length_value = strlen(str) - 2;
			char* cadena = string_substring(str, 1, length_value);
			t_list* lst=list_create();
			int i;
			int j=0;
			bool sublistaTerminada=false;

			char* buffer=malloc(200);
			for(i=0;i<strlen(cadena);i++){

				if(sublistaTerminada){
					sublistaTerminada=false;
				}
				else if(cadena[i]!=']'){
					buffer[j]=cadena[i];
					j++;
				}else{
					buffer[j]=cadena[i];
					buffer[j+1]='\0';
					j=0;
					string_trim(&buffer);
					char** arr= string_get_string_as_array(buffer);
					list_add(lst,(void**)arr);
					buffer=malloc(200);
					sublistaTerminada=true;

				}
			}

	return lst;
}

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
#include "Team.h"
#include <commons/config.h>
#include <pthread.h>

t_list* idsHilos=list_create();//son ints
t_list* hilos=list_create();//son pthread_t
int main(void) {


	return EXIT_SUCCESS;
}

dataTeam* inicializarTeam(char* path){
	t_config* config=config_create(path);
	dataTeam datosTeam;
	datosTeam.entrenadores=list_create();
	datosTeam.objetivoGlobal=list_create();
	//datosTeam.mapa=malloc(sizeof(uint32_t)*20*20);

	char** posicionesEntrenadores=config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** pokemonesEntrenadores=config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** objetivosEntrenadores=config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	int i=0;
	char* x;
	while(){

	}

}


int** mostrarPosicionesEntrenadores(char* path){
	t_config* config=config_create(path);
			char** posicionesEntrenadores=config_get_array_value(config,"POSICIONES_ENTRENADORES");
			int**list=malloc();
			for (int i = 0 ; i < sizeof(posicionesEntrenadores)/2; i ++ ){

				printf("x %c\n", posicionesEntrenadores[i*2][1]);
		        printf("y %c\n" ,posicionesEntrenadores[i*2+1][0]);
			}
}


void inicializarHilosEntrenadores(uint32_t limite){
	uint32_t i=0;
	while(i<limite){
		uint32_t* idHilo=malloc(sizeof(uint32_t));
		pthread_t* hilo=malloc(sizeof(pthread_t));
		idHilo = pthread_create(hilo, NULL, inicializarSiguienteEntrenador, NULL);
		pthread_join(*hilo, NULL);
		list_add(idsHilos,(void*)idHilo);
		list_add(hilos,(void*)hilo);
	}
}



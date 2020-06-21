/*
 * destroyers.c
 *
 *  Created on: 21 jun. 2020
 *      Author: utnso
 */
#include "Team.h"

void destruirObjetivo(void* arg){
	objetivo* obj= (objetivo*) arg;
	free(obj->pokemon);
	free(obj);
}

void destruirPokemonPosicion(void* arg){
	pokemonPosicion* poke=(pokemonPosicion*) arg;
	free(poke->pokemon);
	free(poke);
}

void destruirContadorRafagas(void* arg){
	contadorRafagas* contador=(contadorRafagas*) arg;
	free(contador->mutex);
	free(contador);
}

void destruirDataEntrenador(void* arg){
	dataEntrenador* entrenador= (dataEntrenador*) arg;
	list_destroy_and_destroy_elements(entrenador->pokemones, free);
	list_destroy_and_destroy_elements(entrenador->objetivoPersonal, free);
	free(entrenador->semaforo);
	free(entrenador->semaforoContinuarEjecucion);
	free(entrenador->semaforoPedidoCiclo);
	destruirContadorRafagas((void*) entrenador->contadorCpu);
}

void destruirPokemonSobrante(void* arg){
	pokemonSobrante* poke= (pokemonSobrante*) arg;
	free(poke->pokemon);
	free(poke);
}

void destruirDataTeam(void* arg){
	dataTeam* team= (dataTeam*) arg;
	destruirListaMutexYElementos(team->objetivoGlobal, destruirObjetivo);
	list_destroy_and_destroy_elements(team->entrenadores,destruirDataEntrenador);

}

void liberarMemoria(){
	destruirDataTeam((void*) team);
	terminarPlanificador();
	destruirListaMutexYElementos(especiesLocalizadas, free);
	destruirListaMutexYElementos(listaIdsRespuestasGet, free);
	destruirListaMutexYElementos(listaIdsEntrenadorMensaje, free);
	destruirListaMutex(entrenadoresLibres);
	destruirColaMutexYElementos(pokemonesPendientes, destruirPokemonPosicion);
	destruirListaMutex(entrenadoresExit);
	destruirListaMutex(entrenadoresDeadlock);
	free(entrenadorEnCola);
	free(iniciarResolucionDeadlock);
	free(semaforoObjetivoCumplido);
	free(finalizacionCicloCpu);

}

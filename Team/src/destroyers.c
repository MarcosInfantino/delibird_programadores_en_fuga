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
	pthread_mutex_destroy(contador->mutex);
	free(contador);
}

void destruirDataEntrenador(void* arg){
	dataEntrenador* entrenador= (dataEntrenador*) arg;
	list_destroy_and_destroy_elements(entrenador->pokemones, free);
	list_destroy_and_destroy_elements(entrenador->objetivoPersonal, free);
	sem_destroy(entrenador->semaforo);
	sem_destroy(entrenador->semaforoContinuarEjecucion);
	sem_destroy(entrenador->semaforoPedidoCiclo);
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
	sem_destroy(entrenadorEnCola);
	sem_destroy(iniciarResolucionDeadlock);
	sem_destroy(semaforoObjetivoCumplido);
	sem_destroy(finalizacionCicloCpu);
//	pthread_cancel(threadSuscripcionAppeared);
//	pthread_cancel(threadSuscripcionCaught);
//	pthread_cancel(threadSuscripcionLocalized);
//	pthread_cancel(hiloConexionInicialBroker);
//	pthread_cancel(hiloServidorGameboy);
//	pthread_cancel(hiloPlanificador);

	destruirHilosEntrenadores();

}

void destruirHilosEntrenadores(){
//	for(uint32_t i=0; i<cantEntrenadores;i++){
//		pthread_cancel(arrayIdHilosEntrenadores[i]);
//	}
	free(arrayIdHilosEntrenadores);
}

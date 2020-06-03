/*
 * deadlock.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "Team.h"

//Dado que el proceso Team conoce cuantos Pokemon de cada especie necesita globalmente,
//cuantos de cada uno ha atrapado y planifica al entrenador más cercano libre, puede darse
//el caso que un entrenador que no requiere una especie de Pokémon termine capturandolo,
//impidiendo a otro del mismo equipo que si lo necesita, obtenga el mismo.
//En estos casos se producirá un caso de Deadlock, en el cual el proceso Team no podrá finalizar
//debido a que varios de sus entrenadores están en un estado de Interbloqueo.
//Cuando se detecte dichos casos, se deberá bloquear uno de los entrenadores y planificar al/los otro/s a
//la posición del primero para generar un “intercambio” (cada intercambio implica que cada entrenador
//entregue un Pokémon al otro uno de ellos).

bool mismaListaPokemones(t_list* listaPokemones1, t_list* listaPokemones2){
	uint32_t i;

	if(list_size(listaPokemones1) == list_size(listaPokemones2)){
	t_list* copiaLista1 = list_duplicate(listaPokemones1);
	for(i=0;i<list_size(listaPokemones2);i++){
		char *pokemonAComparar = (char*) list_get(listaPokemones2,i);
		uint32_t encontrado    = buscarMismoPokemon(copiaLista1,pokemonAComparar);

		if(encontrado != -1){
			list_remove(copiaLista1,encontrado);
		}else{
			return false;
		}
	}
	}else{
		return false;
	}
	return true;
}

bool entrenadorEnDeadlock(dataEntrenador* entrenador){ //para saber si un entrenador esta en deadlock
	return(entrenador->estado == BLOCKED && !cumplioObjetivo(entrenador));
}

//t_list* pokemonQueSobra (dataEntrenador* entrenador){
//
//	return listaPoke;
//}
//
//t_list* pokemonQueFalta (dataEntrenador* entrenador){
//
//	return listaPoke;
//}

void realizarIntercambio(){
	//simularCicloCpu(5,entrenador1);
	//simularCicloCpu(5,entrenador2);
}

void entrarEnEjecucionParaDeadlock(dataEntrenador* infoEntrenador){
	sem_wait(&(infoEntrenador->semaforo));
	infoEntrenador->estado = EXEC;
	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
	sem_post(&semaforoEjecucionCpu);
}













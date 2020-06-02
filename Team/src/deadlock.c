/*
 * deadlock.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "deadlock.h"
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

//bool existeDeadlock (dataTeam* dataDelTeam){
//	return (!objetivoCumplido(dataDelTeam) && sizeListaMutex(dataDelTeam->objetivoGlobal) == list_size(dataDelTeam->objetivosCumplidos));
//}
//
//bool estaBloqueado(void* entrenador){
//	dataEntrenador* entrenadorB = (dataEntrenador*) entrenador;
//	return (entrenadorB->estado == BLOCKED);
//}
//
//char* pokemonQueSobra (dataEntrenador* entrenador){
//	saco el pokemon que no le pertenece
//}
//
//char* pokemonQueFalta (dataEntrenador* entrenador){
//	el pokemon que le falta
//}

void resolverDeadlock (dataTeam* dataDelTeam){
//	t_list* entrenadoresEnDeadlock = list_create();
//	entrenadoresEnDeadlock = list_filter(dataDelTeam->entrenadores, estaBloqueado()); filtro por entrenadores bloqueados.
//	dataEntrenador* primerEntrenador = list_get(entrenadoresEnDeadlock, 0);
//	me fijo el pokemon que no le pertenece, recorro la lista para ver a quien le falta ese pokemon, muevo al
//	entrenador a esa posicion, intercambio pokemones.
//	pongo a ese pokemon en exit. Me fijo si el primer pokemon puede estar en exit, si no, repito procedimiento.
//	Todo esto en un while existeDeadlock

}




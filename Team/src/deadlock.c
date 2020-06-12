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
			list_destroy(copiaLista1);
			return false;
		}
	}
	}else{

		return false;
	}
	return true;
}

bool entrenadorEnDeadlock(dataEntrenador* entrenador){ //para saber si un entrenador esta en deadlock
	return(entrenador->estado == BLOCKED && !cumplioObjetivo(entrenador) && !leFaltaCantidadDePokemones(entrenador));
}



void realizarIntercambio(dataEntrenador* entrenadorQueSeMueve){
	simularCicloCpu(5,entrenadorQueSeMueve);

	t_list* pokemonesSobrantesEntrenadorBloqueado=obtenerPokemonesSobrantes(entrenadorBloqueadoParaDeadlock);
	pokemonSobrante* pokemonSobranteInteresante=
			obtenerPokemonInteresante(entrenadorQueSeMueve,pokemonesSobrantesEntrenadorBloqueado);
	char* pokemonAPedir;

	if(pokemonSobranteInteresante==NULL){
		char* primerPokemon=(char*)list_get(pokemonesSobrantesEntrenadorBloqueado,0);
		pokemonAPedir=malloc(strlen(primerPokemon)+1);
		strcpy(pokemonAPedir,primerPokemon);
	}else{
		pokemonAPedir=malloc(strlen(pokemonSobranteInteresante->pokemon)+1);
		strcpy(pokemonAPedir,pokemonSobranteInteresante->pokemon);
	}
	darPokemon(entrenadorQueSeMueve,entrenadorBloqueadoParaDeadlock,entrenadorQueSeMueve->pokemonAAtrapar->pokemon);
	darPokemon(entrenadorBloqueadoParaDeadlock,entrenadorQueSeMueve,pokemonAPedir);



	log_info(teamLogger, "Operación de intercambio realizada entre entrenadores %i y %i",entrenadorQueSeMueve->id, entrenadorBloqueadoParaDeadlock->id);

	sem_post(&intercambioFinalizado);

}

void entrarEnEjecucionParaDeadlock(dataEntrenador* infoEntrenador){
	log_info(teamLogger, "El entrenador %i entra en ejecución para deadlock.");
	sem_wait((infoEntrenador->semaforo));//espera al planificador
	infoEntrenador->estado = EXEC;
	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
	realizarIntercambio(infoEntrenador);
	infoEntrenador->estado=BLOCKED;
	sem_post(&semaforoEjecucionCpu);
}

void resolverDeadlock(){


	while(hayEntrenadoresEnDeadlock()){
		log_info(teamLogger2, "Siguen habiendo entrenadores en deadlock.");

		entrenadorBloqueadoParaDeadlock=(dataEntrenador*)getListaMutex(entrenadoresDeadlock,0);

		while(entrenadorEnDeadlock(entrenadorBloqueadoParaDeadlock)){

			t_list* listaPokemonesSobrantes=obtenerPokemonesSobrantesTeam(entrenadoresDeadlock);

			pokemonSobrante* pokeSobrante=obtenerPokemonInteresante(entrenadorBloqueadoParaDeadlock,listaPokemonesSobrantes );

			dataEntrenador* entrenadorAMover=pokeSobrante->entrenador;

			entrenadorAMover->pokemonAAtrapar->posicion=entrenadorBloqueadoParaDeadlock->posicion;
			entrenadorAMover->pokemonAAtrapar->pokemon=pokeSobrante->pokemon;//OJO, ACA ESTOY ABUSANDO DE LA VARIABLE PARA GUARDAR EL POKEMON QUE DEBE DARLE AEL ENTRENADOR EN MOVIMIENTO AL QUE ESTA QUIETO

			sem_post(entrenadorAMover->semaforo);
			sem_post(entrenadorAMover->semaforo);
			log_info(teamLogger2, "Entrenador bloqueado: %i. Entrenador a mover: %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);

			sem_wait(&intercambioFinalizado);

			list_destroy(listaPokemonesSobrantes);
			free(pokeSobrante);


		}


		actualizarEntrenadoresEnDeadlock();
	}

}

//void resolverDeadlock(){
//
//
//	while(hayEntrenadoresEnDeadlock()){
//
//		entrenadorBloqueadoParaDeadlock=encontrarEntrenadorParaIntercambioMutuo(entrenadoresDeadlock );
//
//		if(entrenadorBloqueadoParaDeadlock==NULL)
//			entrenadorBloqueadoParaDeadlock=(dataEntrenador*)getListaMutex(entrenadoresDeadlock,0);
//
//		while(entrenadorEnDeadlock(entrenadorBloqueadoParaDeadlock)){
//
//			t_list* listaPokemonesSobrantes=obtenerPokemonesSobrantesTeam(entrenadoresDeadlock);
//
//			pokemonSobrante* pokeSobrante=obtenerPokemonInteresante(entrenadorBloqueadoParaDeadlock,listaPokemonesSobrantes );
//
//			dataEntrenador* entrenadorAMover=pokeSobrante->entrenador;
//
//			entrenadorAMover->pokemonAAtrapar->posicion=entrenadorBloqueadoParaDeadlock->posicion;
//			entrenadorAMover->pokemonAAtrapar->pokemon=pokeSobrante->pokemon;//OJO, ACA ESTOY ABUSANDO DE LA VARIABLE PARA GUARDAR EL POKEMON QUE DEBE DARLE AEL ENTRENADOR EN MOVIMIENTO AL QUE ESTA QUIETO
//
//			habilitarHiloEntrenador(entrenadorAMover->id);
//
//			sem_wait(&intercambioFinalizado);
//
//			list_destroy(listaPokemonesSobrantes);
//			free(pokeSobrante);
//
//
//		}
//
//
//		actualizarEntrenadoresEnDeadlock();
//	}
//
//}

bool hayEntrenadoresEnDeadlock(){
	return sizeListaMutex(entrenadoresDeadlock)>0;
}


void actualizarEntrenadoresEnDeadlock(){

	for(uint32_t i=0;i<sizeListaMutex(entrenadoresDeadlock);i++){
		dataEntrenador* entrenadorActual=(dataEntrenador*) getListaMutex(entrenadoresDeadlock,i);
		if(cumplioObjetivo(entrenadorActual)){
			entrenadorActual->estado=EXIT;
			removeListaMutex(entrenadoresDeadlock,i);
			log_info(teamLogger2, "El entrenador %i salio del deadlock.");
		}
	}
}

t_list* obtenerPokemonesSobrantesTeam(listaMutex* listaEntrenadores){
	t_list* listaGlobal=list_create();
	for(uint32_t i=0;i<sizeListaMutex(listaEntrenadores);i++){
		dataEntrenador* entrenadorActual=getListaMutex(listaEntrenadores,i);
		t_list* listaActual=obtenerPokemonesSobrantes(entrenadorActual);//ESTA DESPUES HAY QUE DESTRUIRLA
		list_add_all(listaGlobal,listaActual);
		list_destroy(listaActual);

	}
	return listaGlobal;
}

pokemonSobrante* obtenerPokemonInteresante(dataEntrenador* entrenador,t_list* pokemonesSobrantes){
	for(uint32_t i=0;i<list_size(pokemonesSobrantes);i++){
		pokemonSobrante* pokemonSobranteActual=(pokemonSobrante*)list_get(pokemonesSobrantes,i);
		if(pokemonLeInteresa(entrenador,pokemonSobranteActual->pokemon)){
			list_remove(pokemonesSobrantes,i);
			return pokemonSobranteActual;
		}
	}
	return NULL;
}







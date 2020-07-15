/*
 * deadlock.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "Team.h"

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
	//return(entrenador->estado == BLOCKED && !cumplioObjetivo(entrenador) && !leFaltaCantidadDePokemones(entrenador));
	return(!cumplioObjetivo(entrenador) && !leFaltaCantidadDePokemones(entrenador));
}



void realizarIntercambio(dataEntrenador* entrenadorQueSeMueve){

	log_info(teamLogger2, "El entrenador %i comienza con el intercambio con el entrenador %i.", entrenadorQueSeMueve->id, entrenadorBloqueadoParaDeadlock->id);
	simularCicloCpu(5,entrenadorQueSeMueve);
	t_list* pokemonesSobrantesEntrenadorBloqueado=obtenerPokemonesSobrantes(entrenadorBloqueadoParaDeadlock);

	//char* pueba=(char*)list_get(pokemonesSobrantesEntrenadorBloqueado,0);
	pokemonSobrante* pokemonSobranteInteresante=
			obtenerPokemonInteresante(entrenadorQueSeMueve,pokemonesSobrantesEntrenadorBloqueado);
	char* pokemonAPedir;

	if(pokemonSobranteInteresante==NULL){
		//char* primerPokemon=(char*)list_get(entrenadorBloqueadoParaDeadlock->pokemones,0);

		//char* primerPokemon=(char*)list_get(pokemonesSobrantesEntrenadorBloqueado,0);
		char* primerPokemon=((pokemonSobrante*)list_get(pokemonesSobrantesEntrenadorBloqueado,0))->pokemon;
		//log_info(teamLogger2, "pokemon %s", primerPokemon);
		pokemonAPedir=malloc(strlen(primerPokemon)+1);
		strcpy(pokemonAPedir,primerPokemon);

	}else{
		pokemonAPedir=malloc(strlen(pokemonSobranteInteresante->pokemon)+1);
		strcpy(pokemonAPedir,pokemonSobranteInteresante->pokemon);
		//log_info(teamLogger2, "pokeAPedir  %s", pokemonAPedir);
	}
	//log_info(teamLogger2, "pokeADar  %s", entrenadorQueSeMueve->pokemonAAtrapar->pokemon);
	darPokemon(entrenadorQueSeMueve,entrenadorBloqueadoParaDeadlock,entrenadorQueSeMueve->pokemonAAtrapar->pokemon);
	darPokemon(entrenadorBloqueadoParaDeadlock,entrenadorQueSeMueve,pokemonAPedir);

	free(entrenadorQueSeMueve->pokemonAAtrapar->pokemon);
	free(pokemonAPedir);
	log_info(teamLogger2, "Operación de intercambio realizada entre entrenadores %i y %i",entrenadorQueSeMueve->id, entrenadorBloqueadoParaDeadlock->id);
	log_info(teamLogger, "Operación de intercambio realizada entre entrenadores %i y %i",entrenadorQueSeMueve->id, entrenadorBloqueadoParaDeadlock->id);

	sem_post(&intercambioFinalizado); //OK5

}

//void entrarEnEjecucionParaDeadlock(dataEntrenador* infoEntrenador){
//	log_info(teamLogger, "El entrenador %i se mueve a la posición del entrenador %i.", infoEntrenador->id, entrenadorBloqueadoParaDeadlock->id);
//
//	sem_wait((infoEntrenador->semaforo));//espera al planificador //OK4
//	//infoEntrenador->estado = EXEC;
//	ponerEnEjecucion(infoEntrenador);
//	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
//	realizarIntercambio(infoEntrenador);
//	poneteEnBlocked(infoEntrenador);
//	//infoEntrenador->estado=BLOCKED;
//	sem_post(&semaforoEjecucionCpu);
//}

//void* resolverDeadlock(void* arg){
//
//	sem_wait(iniciarResolucionDeadlock);
//	while(hayEntrenadoresEnDeadlock()){
//		//log_info(teamLogger2, "Siguen habiendo entrenadores en deadlock.");
//
//		entrenadorBloqueadoParaDeadlock=(dataEntrenador*)getListaMutex(entrenadoresDeadlock,0);
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
//			sem_post(entrenadorAMover->semaforo);
//			log_info (teamLogger, "El entrenador %i, esta en DEADLOCK con el entrenador %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
//			log_info (teamLogger2, "El entrenador %i, esta en DEADLOCK con el entrenador %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
//			//log_info(teamLogger2, "Entrenador bloqueado: %i. Entrenador a mover: %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
//
//			(team->cantidadDeadlocksEncontrados)++;
//
//			sem_wait(&intercambioFinalizado);
//
//			(team->cantidadDeadlocksResueltos)++;
//
//			list_destroy(listaPokemonesSobrantes);
//			free(pokeSobrante);
//		}
//
//		actualizarEntrenadoresEnDeadlock();
//	}
//
//	if(objetivoCumplido()){
//		sem_post(semaforoObjetivoCumplido);
//	}
//	return NULL;
//}


void* resolverDeadlock(void* arg){

	sem_wait(iniciarResolucionDeadlock);

	while(hayEntrenadoresEnDeadlock()){

		entrenadorBloqueadoParaDeadlock=encontrarEntrenadorParaIntercambioMutuo(entrenadoresDeadlock);
		t_list* entrenadoresEsperaCircular;

		if(entrenadorBloqueadoParaDeadlock==NULL){
			//log_info(teamLogger2,"Entre al if.");
			entrenadoresEsperaCircular=list_create();
			entrenadoresEsperaCircular=encontrarEsperaCircular(entrenadoresDeadlock,entrenadoresEsperaCircular,NULL);
			loggearEsperaCircular(entrenadoresEsperaCircular);
			resolverEsperaCircular(entrenadoresEsperaCircular);
			list_destroy(entrenadoresEsperaCircular);
		}else{
			resolverIntercambioMutuo();
		}
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


		actualizarEntrenadoresEnDeadlock();
	}

	if(objetivoCumplido()){
			sem_post(semaforoObjetivoCumplido);
	}

	return NULL;
}

void loggearEsperaCircular(t_list* listaEspera){
	log_info(teamLogger2,"Encontre una espera circular. Entrenadores involucrados: ");
	log_info(teamLogger,"Encontre una espera circular. Entrenadores involucrados: ");
	for(uint32_t i=0; i<list_size(listaEspera);i++){
		dataEntrenador* entrenadorActual= (dataEntrenador*) list_get(listaEspera,i);
		log_info(teamLogger2,"Entrenador %i.", entrenadorActual->id);
		log_info(teamLogger,"Entrenador %i.", entrenadorActual->id);
	}
}

void resolverIntercambioMutuo(){
	dataEntrenador* companiero=encontrarCompanieroIntercambioMutuo(entrenadorBloqueadoParaDeadlock, entrenadoresDeadlock);
	t_list* listaPokemonesSobrantes=obtenerPokemonesSobrantes(companiero);

	pokemonSobrante* pokeSobrante=obtenerPokemonInteresante(entrenadorBloqueadoParaDeadlock,listaPokemonesSobrantes );

	dataEntrenador* entrenadorAMover=pokeSobrante->entrenador;
	entrenadorAMover->pokemonAAtrapar=malloc(sizeof(pokemonPosicion));
	entrenadorAMover->pokemonAAtrapar->posicion=entrenadorBloqueadoParaDeadlock->posicion;
	entrenadorAMover->pokemonAAtrapar->pokemon=pokeSobrante->pokemon;//OJO, ACA ESTOY ABUSANDO DE LA VARIABLE PARA GUARDAR EL POKEMON QUE DEBE DARLE AEL ENTRENADOR EN MOVIMIENTO AL QUE ESTA QUIETO

	sem_post(entrenadorAMover->semaforo);
	log_info (teamLogger, "El entrenador %i, esta en DEADLOCK con el entrenador %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
	log_info (teamLogger2, "El entrenador %i, esta en DEADLOCK con el entrenador %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
	//log_info(teamLogger2, "Entrenador bloqueado: %i. Entrenador a mover: %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);

	(team->cantidadDeadlocksEncontrados)++;

	sem_wait(&intercambioFinalizado);

	(team->cantidadDeadlocksResueltos)++;



	list_destroy_and_destroy_elements(listaPokemonesSobrantes,destruirPokemonSobrante);
	//free(pokeSobrante);




}

void resolverEsperaCircular(t_list* entrenadoresEsperaCircular){
	for(uint32_t i=0; i<list_size(entrenadoresEsperaCircular)-1;i++){

		entrenadorBloqueadoParaDeadlock=(dataEntrenador*) list_get(entrenadoresEsperaCircular,i);
		log_info(teamLogger2, "Espera circular. Entrenador bloqueado: %i.", entrenadorBloqueadoParaDeadlock->id);
		listaMutex* listaMutexEntrenadoresEsperaCircular= convertirAListaMutex(entrenadoresEsperaCircular);
		t_list* listaPokemonesSobrantes=obtenerPokemonesSobrantesTeam(listaMutexEntrenadoresEsperaCircular);

		//DESTRUIR listaMutexEntrenadoresEsperaCircular

		pokemonSobrante* pokeSobrante=obtenerPokemonInteresante(entrenadorBloqueadoParaDeadlock,listaPokemonesSobrantes );
		log_info(teamLogger2, "entrenador bloqueado para deadlock : %i", entrenadorBloqueadoParaDeadlock->id);
		dataEntrenador* entrenadorAMover=pokeSobrante->entrenador;
		entrenadorAMover->pokemonAAtrapar=malloc(sizeof(pokemonPosicion));
		entrenadorAMover->pokemonAAtrapar->posicion=entrenadorBloqueadoParaDeadlock->posicion;
		log_info(teamLogger2, "hola.");
		entrenadorAMover->pokemonAAtrapar->pokemon=pokeSobrante->pokemon;//OJO, ACA ESTOY ABUSANDO DE LA VARIABLE PARA GUARDAR EL POKEMON QUE DEBE DARLE AEL ENTRENADOR EN MOVIMIENTO AL QUE ESTA QUIETO
		log_info(teamLogger2, "Entrenador bloqueado: %i. Entrenador a mover: %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
		sem_post(entrenadorAMover->semaforo);
		log_info (teamLogger, "El entrenador %i, esta en DEADLOCK con el entrenador %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
		log_info (teamLogger2, "El entrenador %i, esta en DEADLOCK con el entrenador %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);
						//log_info(teamLogger2, "Entrenador bloqueado: %i. Entrenador a mover: %i.", entrenadorBloqueadoParaDeadlock->id, entrenadorAMover->id);

		(team->cantidadDeadlocksEncontrados)++;

		sem_wait(&intercambioFinalizado);

		(team->cantidadDeadlocksResueltos)++;

		list_destroy_and_destroy_elements(listaPokemonesSobrantes,destruirPokemonSobrante);



		//free(pokeSobrante);


//
//		log_info(teamLogger2, "---------------------------------------------------------------");
//		for(uint32_t z=0; z<list_size(entrenadoresEsperaCircular);z++){
//			dataEntrenador* e=(dataEntrenador*) list_get(entrenadoresEsperaCircular,z);
//			log_info(teamLogger2, "Entrenador %i: %s", e->id, (char*)list_get(e->pokemones,0));
//		}

	}
	log_info (teamLogger2, "hice todos los intercambios.");
}

//bool esperaCircularResuelta(t_list* esperaCircular){
//	for(uint32_t i=0; i<list_size(esperaCircular);i++){
//		dataEntrenador* entrenadorActual= (dataEntrenador*) list_get(esperaCircular, i);
//
//		if()
//	}
//	return true;
//}

bool hayEntrenadoresEnDeadlock(){
	return sizeListaMutex(entrenadoresDeadlock)>0;
}


void actualizarEntrenadoresEnDeadlock(){

	for(uint32_t i=0;i<sizeListaMutex(entrenadoresDeadlock);i++){
		dataEntrenador* entrenadorActual=(dataEntrenador*) getListaMutex(entrenadoresDeadlock,i);
		//log_info(teamLogger2, "El entrenador %i entra al for.", entrenadorActual->id);
		if(cumplioObjetivo(entrenadorActual)){
			//log_info(teamLogger2, "El entrenador %i entra al if.", entrenadorActual->id);
			removeListaMutex(entrenadoresDeadlock,i);
			i--;
			poneteEnExit(entrenadorActual);
			log_info(teamLogger2, "El entrenador %i salio del deadlock.", entrenadorActual->id);
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

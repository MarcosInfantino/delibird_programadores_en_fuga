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
#include <stdbool.h>
#include "broker.h"
#include "memoria.h"


/*Procedimiento para almacenamiento de datos
Se buscará una partición libre que tenga suficiente memoria continua como para contener el valor.
 En caso de no encontrarla, se pasará al paso siguiente (si corresponde, en caso contrario se pasará al paso 3
  directamente).
Se compactará la memoria y se realizará una nueva búsqueda. En caso de no encontrarla, se pasará al paso siguiente.
Se procederá a eliminar una partición de datos. Luego, si no se pudo encontrar una partición con suficiente memoria
como para contener el valor, se volverá al paso 2 o al 3 según corresponda.*/


void registrarEnMemoriaPARTICIONES(msgMemoriaBroker* mensajeNuevo){
	particionLibre* particionLibre = malloc(sizeof(particionLibre));
	particionLibre = obtenerParticionLibrePARTICIONES(mensajeNuevo->sizeStream);

	if(particionLibre == NULL){
		//COMPACTAR y volver a llamar a obtenerpart... si vuelve a devolver nulo, eliminamos una particion segun
		//los algoritmos y vuelvo a ejecutar obtenerPArt... si devuelve null vuelvo a compactar y así sucesivamente
	}

	asignarPuntero(particionLibre->offset, mensajeNuevo->stream, mensajeNuevo->sizeStream);
	free(particionLibre);
}

particionLibre* obtenerParticionLibrePARTICIONES(uint32_t tamStream){

	if(sizeListaMutex(particionesLibres) <= 0)
		return NULL;

	if (algoritmoParticionLibre == FIRST_FIT){ //acá hay qué agregar mutex?

		list_sort(particionesLibres->lista, menorAmayorSegunOffset);
		//return removeListaMutex(particionesLibres, 1); //lo saca de los libres y lo retorna
		return list_remove(particionesLibres->lista, 1);

	}else if(algoritmoParticionLibre == BEST_FIT){
		list_sort(particionesLibres->lista, menorAmayorSegunSize);
		return list_remove_by_condition(particionesLibres->lista, esSuficientementeGrandeParaElMSG );
		//no se como pasar por parametro el tamStream, deberia ser global, la condicion solo acepta 1 parametro
		//y es el elemento de la lista que varía.
	}

	return NULL;
}

bool menorAmayorSegunOffset(void* primero, void* segundo){
	return ((particionLibre*)primero)->offset < ((particionLibre*)segundo)->offset;}

bool menorAmayorSegunSize(void* primero, void* segundo){
	return ((particionLibre*)primero)->sizeParticion < ((particionLibre*)segundo)->sizeParticion;}

bool esSuficientementeGrandeParaElMSG(void* elemento){
	uint32_t tamStream = 0; //lo puse para q no rompa pero debería ser global para poder comparar.
	return ((particionLibre*)elemento)->sizeParticion >= tamStream;
}

/*msgMemoriaBroker*  buscarMensajeEnMemoriaParticiones(idMensajeBuscado){

}*/


listaMutex* iniciarMemoriaPARTICIONES(){
	return inicializarListaMutex();
}

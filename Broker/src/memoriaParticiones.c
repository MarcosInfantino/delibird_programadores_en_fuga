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
	if(algoritmoParticionLibre == FIRST_FIT){


		for(int i = 0; i < sizeListaMutex(particionesLibres); i++){


		}
		particionLibre* particionLIbre = getListaMutex(particionesLibres, 1);
	}else{ //BEST_FIT

	}

}


/*msgMemoriaBroker*  buscarMensajeEnMemoriaParticiones(idMensajeBuscado){

}*/


listaMutex* iniciarMemoriaPARTICIONES(){
	return inicializarListaMutex();
}

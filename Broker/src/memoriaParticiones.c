
#include "broker.h"
#include "memoria.h"
#include "memoriaParticiones.h"

bool menorAMayorSegunLru (void* part1, void* part2){
	particion* parti1 = (particion*) part1;
	particion* parti2 = (particion*) part2;

	return menorTiempo(parti1->lru, parti2->lru);
}

bool hayParticionLibreALaIzquierda(particion* particionLibre){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		if(partiActual->offset + partiActual->sizeParticion == particionLibre->offset){
			return true;
		}
	}
	return false;
}

particion* particionLibreALaIzquierda(particion* particionLibreNueva){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		if(partiActual->offset + partiActual->sizeParticion == particionLibreNueva->offset){
			removeListaMutex(particionesLibres, i);
			return partiActual;
		}
	}
	return NULL;
}

bool hayParticionLibreALaDerecha(particion* particionLibreNueva){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		if(partiActual->offset == (particionLibreNueva->offset + particionLibreNueva->sizeParticion)){
			return true;
		}
	}
	return false;
}

particion* particionLibreALaDerecha(particion* particionLibreNueva){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		if(partiActual->offset == (particionLibreNueva->offset + particionLibreNueva->sizeParticion)){
			removeListaMutex(particionesLibres, i);
			return partiActual;
		}
	}
	return NULL;
}

void consolidarSiSePuede(particion* particionLibre){
	 if(hayParticionLibreALaDerecha(particionLibre)){
		 particion* partAConsolidar = particionLibreALaDerecha(particionLibre);
		 log_info(loggerBroker, "Se consolidan las particiones que comienzan en: %i y %i", particionLibre->offset, partAConsolidar->offset);
		 particionLibre->sizeParticion = particionLibre->sizeParticion + partAConsolidar->sizeParticion;
		 destroyParticionLibre(partAConsolidar);
	 }else if(hayParticionLibreALaIzquierda(particionLibre)){
		 particion* partAConsolidar = particionLibreALaIzquierda(particionLibre);
		 log_info(loggerBroker, "Se consolidan las particiones que comienzan en: %i y %i", partAConsolidar->offset, particionLibre->offset);
		 particionLibre->offset = partAConsolidar->offset;
		 particionLibre->sizeParticion = particionLibre->sizeParticion + partAConsolidar->sizeParticion;
		 destroyParticionLibre(partAConsolidar);
	 }
}

void eliminarParticion (particion* part){
	particion* partiNueva = malloc(sizeof(particion));
	partiNueva->offset = part->offset;
	partiNueva->sizeParticion = part->sizeParticion;
	partiNueva->estadoParticion = PARTICION_LIBRE;
	addListaMutex(particionesLibres, (void*)partiNueva);
	log_info(loggerBroker, "Elimino particion que comienza en: %p", memoria + partiNueva->offset );
	removeAndDestroyElementListaMutex(particionesOcupadas, 0, destroyParticionOcupada);
	consolidarSiSePuede(partiNueva);
}

bool menorAMayorSegunTiempoCarga(void* part1, void* part2){
	particion* parti1 = (particion*) part1;
	particion* parti2 = (particion*) part2;
	return parti1->tiempoDeCargaPart<parti2->tiempoDeCargaPart;
}

void elegirParticionVictimaYEliminarla(){
	particion* particionVictima;
	switch(algoritmoReemplazo){
	case FIFO:
		list_sort_Mutex(particionesOcupadas, menorAMayorSegunTiempoCarga);
		particionVictima = (particion*)getListaMutex(particionesOcupadas, 0);
		break;
	case LRU:
		list_sort_Mutex(particionesOcupadas, menorAMayorSegunLru);
		particionVictima = (particion*)getListaMutex(particionesOcupadas, 0);
		break;
	}
	eliminarParticion(particionVictima);
}

bool sePuedeCompactar(){
	if(frecuenciaCompactacion==-1){
		return (sizeListaMutex(particionesOcupadas)==0);
	}else if(sizeListaMutex(particionesLibres)>1 && frecuenciaCompactacion>=0){
		return cantidadBusquedasFallidas >= frecuenciaCompactacion;
	}
	return NULL;
}

void compactar(){
	particion* elemento;
	uint32_t base = 0;
	list_sort_Mutex(particionesOcupadas, menorAmayorSegunOffset);
	for(int i=0; i<sizeListaMutex(particionesOcupadas); i++){
		elemento = (particion*)getListaMutex(particionesOcupadas, i);

		void* aux = malloc(elemento->sizeParticion);
		memcpy(aux, memoria + elemento->offset, elemento->sizeParticion);

		memcpy(memoria + base, aux, elemento->sizeParticion);

		elemento->mensaje->stream = memoria + base;//IMPORTANTISIMO
		free(aux);

		elemento->offset = base;
		base += elemento->sizeParticion;
	}
	log_info(loggerBroker, "Se compacta la memoria - particiones dinámicas.");
	generarParticionLibre(base);
	cantidadBusquedasFallidas = 0;
}

void registrarEnParticiones(msgMemoriaBroker* mensajeNuevo){
	particion* particionLibre;
	particionLibre = obtenerParticionLibrePARTICIONES(mensajeNuevo->sizeStream);
	if(particionLibre == NULL){
		cantidadBusquedasFallidas++;
		if(sePuedeCompactar()){
			compactar();
			particionLibre = obtenerParticionLibrePARTICIONES(mensajeNuevo->sizeStream);
			if(particionLibre == NULL){
				cantidadBusquedasFallidas++;
				elegirParticionVictimaYEliminarla();
				registrarEnParticiones (mensajeNuevo);
			}
		}else{
			elegirParticionVictimaYEliminarla();
			registrarEnParticiones(mensajeNuevo);
		}
	}

	if(particionLibre != NULL){
		asignarMensajeAParticion(particionLibre, mensajeNuevo);
	}
}

void asignarMensajeAParticion(particion* partiLibre, msgMemoriaBroker* mensaje){
	particion* partiOcupada = malloc(sizeof(particion));
	partiOcupada->offset = partiLibre->offset;
	partiOcupada->mensaje = mensaje;
	if(mensaje->sizeStream > particionMinima){
		partiOcupada->sizeParticion = mensaje->sizeStream;
	}else{
		partiOcupada->sizeParticion = particionMinima;
	}


	partiOcupada->estadoParticion = PARTICION_OCUPADA;

	partiOcupada->lru = temporal_get_string_time();
	partiOcupada->tiempoDeCargaPart=TiempoCarga;
	TiempoCarga++;
	memcpy(memoria + partiOcupada->offset, mensaje->stream, mensaje->sizeStream);

	mensaje->stream = memoria + partiOcupada->offset;

	log_info(brokerLogger2,"ASIGNE: size de partición: %i. Id mensaje: %i. Size del mensaje: %i.", partiOcupada->sizeParticion, mensaje->idMensaje, mensaje->sizeStream);


	addListaMutex(particionesOcupadas, (void*)partiOcupada);
	log_info(loggerBroker, "Almaceno mensaje en partición que comienza en: %i", partiOcupada->offset);
	if(partiLibre->sizeParticion > partiOcupada->sizeParticion){
		partiLibre->offset += partiOcupada->sizeParticion;
		partiLibre->sizeParticion -= partiOcupada->sizeParticion;
		partiLibre->estadoParticion = PARTICION_LIBRE;
		addListaMutex(particionesLibres, (void*)partiLibre);
	}else{
		destroyParticionLibre (partiLibre);
	}
}

particion* crearPrimeraParticionLibre(void){
	particion* particionADevolver = malloc(sizeof(particion));
	particionADevolver->offset = 0;
	particionADevolver->sizeParticion   = tamMemoria;
	particionADevolver->estadoParticion = PARTICION_LIBRE;
	return particionADevolver;
}

uint32_t cantidadMemoriaLibre (){
	uint32_t memoriaLibre=0;
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* parti = (particion*)getListaMutex(particionesLibres,i);
		memoriaLibre += parti->sizeParticion;
	}
	return memoriaLibre;
}

bool noHayNingunaSuficientementeGrande(uint32_t tamStream){

	for(int i =0; i<sizeListaMutex(particionesLibres); i++){
		particion* parti = (particion*)getListaMutex(particionesLibres, i);
		if(parti->sizeParticion>=tamStream){
			return false;
		}
	}
	return true;
}

particion* obtenerParticionLibrePARTICIONES(uint32_t tamStream){
	if(noHayNingunaSuficientementeGrande(tamStream) || cantidadMemoriaLibre()<tamStream){

		return NULL;
	}
	if (algoritmoParticionLibre == FIRST_FIT){

		if(sizeListaMutex(particionesLibres)>1)
			list_sort_Mutex(particionesLibres, menorAmayorSegunOffset);

		particion* pSeleccionadaFIRST = (particion*)list_remove_by_condition_Mutex(particionesLibres, esSuficientementeGrandeParaElMSG);
		return pSeleccionadaFIRST;
	}else if(algoritmoParticionLibre == BEST_FIT){

		if(sizeListaMutex(particionesLibres)>1)
			list_sort_Mutex(particionesLibres, menorAmayorSegunSize);

		particion* pSeleccionadaBEST = (particion*)list_remove_by_condition_Mutex(particionesLibres, esSuficientementeGrandeParaElMSG);
		return pSeleccionadaBEST;
	}
	return NULL;
}



void generarParticionLibre(uint32_t base){
	particion* nuevaParticion = malloc(sizeof(particion));
	nuevaParticion->offset = base;
	nuevaParticion->sizeParticion   = tamMemoria - base;
	nuevaParticion->estadoParticion = PARTICION_LIBRE;
	uint32_t size = sizeListaMutex(particionesLibres);
	for(int j=0; j<size; j++){
		removeAndDestroyElementListaMutex(particionesLibres, 0, destroyParticionLibre);
	}
	addListaMutex(particionesLibres,(void*) nuevaParticion);
}

bool menorAmayorSegunOffset(void* primero, void* segundo){
	return ((particion*)primero)->offset < ((particion*)segundo)->offset;
}

bool menorAmayorSegunSize(void* primero, void* segundo){
	return ((particion*)primero)->sizeParticion < ((particion*)segundo)->sizeParticion;
}

bool esSuficientementeGrandeParaElMSG(void* elemento){
	particion* partLibre = (particion*)elemento;
	return partLibre->sizeParticion >= auxTamanioStreamGlobal;
}

t_list* buscarMensajesDeColaEnParticiones (uint32_t cola){
	particion* particion1;
	t_list* msjsDeCola = list_create();
	for(int i = 0; i<sizeListaMutex(particionesOcupadas); i++){
		particion1 = (particion*) getListaMutex(particionesOcupadas, i);
		if(particion1->mensaje->cola == cola){
			char* aux=particion1->lru;
			particion1->lru = temporal_get_string_time();
			free(aux);
			list_add(msjsDeCola, (void*)particion1->mensaje);
		}
	}
	return msjsDeCola;
}

void enviarMsjsASuscriptorNuevoParticiones (uint32_t cola, uint32_t socket, uint32_t idProceso){
	t_list* listMsjs = buscarMensajesDeColaEnParticiones(cola);
	msgMemoriaBroker* msg;
	paquete* paqueteASerializar;
	void* paqueteSerializado;
	for(int i = 0; i<list_size(listMsjs); i++){
		msg = (msgMemoriaBroker*) list_get(listMsjs, i);
		paqueteASerializar = llenarPaquete(msg->modulo, msg->cola, msg->sizeStream, msg->stream);
		insertarIdPaquete(paqueteASerializar, msg->idMensaje);
		insertarIdCorrelativoPaquete(paqueteASerializar, msg->idCorrelativo);
		paqueteSerializado = serializarPaquete(paqueteASerializar);
		send(socket, paqueteSerializado, sizePaquete(paqueteASerializar),0);

		free(paqueteSerializado);
		//memory leak
		guardarYaEnviados(paqueteASerializar, idProceso);
	}
	list_destroy(listMsjs);
}

msgMemoriaBroker*  buscarMensajeEnMemoriaParticiones(uint32_t idMensajeBuscado){
	particion* particion1;
	for(int i = 0; i<sizeListaMutex(particionesOcupadas); i++){
		particion1 = (particion*) getListaMutex (particionesOcupadas, i);
		if(particion1->mensaje->idMensaje == idMensajeBuscado){
			return particion1->mensaje;
		}
	}
	return NULL;
}

void destroyParticionLibre(void* parti){
	particion* part = (particion*)parti;
	free(part);
}

void destroyParticionOcupada (void* part){
	particion* parti = (particion*) part;
	free(parti->mensaje); //ver si destruir las listas o no
	free(parti);
}

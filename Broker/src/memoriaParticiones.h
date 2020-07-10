/*
 * memoriaParticiones.h
 *
 *  Created on: 9 jul. 2020
 *      Author: utnso
 */

#ifndef MEMORIAPARTICIONES_H_
#define MEMORIAPARTICIONES_H_

#include "broker.h"
#include "memoria.h"

uint32_t TC;

typedef enum{
	PARTICION_LIBRE,
	PARTICION_OCUPADA
}estadoParticion;

typedef struct{
	uint32_t offset;
	uint32_t sizeParticion;
	struct tm lru;
	//struct tm tiempoDeCargaPart;
	uint32_t tiempoDeCargaPart;
	msgMemoriaBroker* mensaje;
	uint32_t estadoParticion;
}particion;

particion* crearPrimeraParticionLibre();
particion* inicializarParticion();
void registrarEnParticiones(msgMemoriaBroker*);
particion* obtenerParticionLibrePARTICIONES(uint32_t tamStream);
bool menorAmayorSegunSize(void* primero, void* segundo);
bool menorAmayorSegunOffset(void* primero, void* segundo);
bool esSuficientementeGrandeParaElMSG(void* elemento);
void generarParticionLibre(uint32_t base);
void compactar();
void asignarMensajeAParticion(particion* partiLibre, msgMemoriaBroker* mensaje);
void eliminarParticion (particion* particion);
void elegirParticionVictimaYEliminarla();
void destroyParticionOcupada (void* particion);
void destroyParticionLibre (void* particion);
bool menorAMayorSegunTiempoCarga (void* part1, void* part2);
bool menorAMayorSegunLru (void* part1, void* part2);
bool sePuedeCompactar();
t_list* buscarMensajesDeColaEnParticiones (uint32_t cola);
void enviarMsjsASuscriptorNuevoParticiones (uint32_t cola, uint32_t* socket);
msgMemoriaBroker*  buscarMensajeEnMemoriaParticiones(uint32_t idMensajeBuscado);
void consolidarSiSePuede(particion* particionLibre);
particion* particionLibreALaIzquierda(particion* particionLibreNueva);
particion* particionLibreALaDerecha(particion* particionLibreNueva);

#endif /* MEMORIAPARTICIONES_H_ */

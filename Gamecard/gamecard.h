/*
 * gamecard.h
 *
 *  Created on: 12 may. 2020
 *      Author: juancito
 */
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <messages_lib/messages_lib.h>

#ifndef GAMECARD_H_
#define GAMECARD_H_

void* suscribirseCola(void* msgSuscripcion);
void* suscribirseColasBroker(void* config);

#endif /* GAMECARD_H_ */

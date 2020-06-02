/*
 * deadlock.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef DEADLOCK_H_
#define DEADLOCK_H_
#include "Team.h"

bool estaBloqueado(void* entrenador);
bool existeDeadlock (dataTeam* dataDelTeam);
void resolverDeadlock (dataTeam* dataDelTeam);



#endif /* DEADLOCK_H_ */

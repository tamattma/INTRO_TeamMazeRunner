/**
 * \file
 * \brief Sumo strategy implementation
 * \author Remo Mattmann, Caroline Brunner
 *
 * This module implements the strategy for the sumo battels.
 */

#ifndef SUMO_H_
#define SUMO_H_

#include "Platform.h"


typedef enum {
	SUMO_IDLE,
	SUMO_START,
	SUMO_WAIT_5s,
	SUMO_VOLLGAS,
	SUMO_VOLLGAS_LINE,
	SUMO_VOLLGAS_SEARCH,
	SUMO_TRAP_SEARCH,
	SUMO_TRAP_WAIT,
	SUMO_TRAP_BACK,
	SUMO_TRAP_FORWARD,
} SUMO_States;

typedef enum {
	SUMO_VOLLGAS,
	SUMO_TRAP,
	SUMO_MIXED,
} SUMO_Strategy;

/*!
 * \brief De-Initializes the module.
 */
void SUMO_Deinit(void);

/*!
 * \brief Initializes the module.
 */
void SUMO_Init(void);




#endif /* SUMO_H_ */

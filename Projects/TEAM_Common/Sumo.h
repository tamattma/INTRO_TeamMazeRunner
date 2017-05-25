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
	SUMO_DUMMY_DRIVE,
	SUMO_DUMMY_TURN,
	SUMO_VOLLGAS_FORWARD,
	SUMO_VOLLGAS_LINE,
	SUMO_VOLLGAS_SEARCH,
	SUMO_TRAP_SEARCH,
	SUMO_TRAP_WAIT,
	SUMO_TRAP_BACK,
	SUMO_TRAP_FORWARD,
} SUMO_States;

typedef enum {
	SUMO_DUMMY,
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

/*!
 * brief Called for Start Signal
 * param SUMO_Strategy to be used
 */
void SUMO_Start(SUMO_Strategy strat);

/*!
 * brief Called for Stop Signal
 * param SUMO_Strategy to be used
 */
void SUMO_Stop(void);



#endif /* SUMO_H_ */

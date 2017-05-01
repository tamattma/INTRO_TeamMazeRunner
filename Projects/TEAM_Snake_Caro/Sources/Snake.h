/**
 * \file
 * \brief Snake game
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a classic Nokia phone game: the Snake game.
 */

#ifndef SNAKE_H_
#define SNAKE_H_

/*! \todo Extend interface as needed */

/*
 * status
 */
typedef enum {
	Snake_NotRunning,
	Snake_StartUp,
	Snake_Running,
	Snake_Pause
}Snake_Status;

Snake_Status Snake_GetStatus();	/* get the satus of the snake game */

void Snake_SetLeftEVNT();	/* left button pressed */

void Snake_SetRightEVNT();	/* right button pressed */

void Snake_SetUpEVNT();		/* up button pressed */

void Snake_SetDownEVNT();	/* down button pressed */

void Snake_SetPauseEVNT();	/* center button pressed */

void Snake_AnyButton();		/* any button pressed */

/*!
 * \brief Driver de-initialization.
 */
void SNAKE_Deinit(void);

/*!
 * \brief Driver initialization.
 */
void SNAKE_Init(void);

#endif /* SNAKE_H_ */

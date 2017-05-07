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
bool SNAKE_Running;

void SNAKE_ButtonLeft(void);
void SNAKE_ButtonRight(void);
void SNAKE_ButtonUp(void);
void SNAKE_ButtonDown(void);
void SNAKE_ButtonCenter(void);

/*!
 * \brief Driver de-initialization.
 */
void SNAKE_Deinit(void);

/*!
 * \brief Driver initialization.
 */
void SNAKE_Init(void);

#endif /* SNAKE_H_ */

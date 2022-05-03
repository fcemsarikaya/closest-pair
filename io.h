/**     
*@file io.h
*@author Fidel Cem Sarikaya <fcemsarikaya@gmail.com>
*@date 06.12.2021
*
*@brief Provides the essential functions to the program 'cpair'.
*
* The io module. Contains functions to handle input/output operations of the program.
**/

#ifndef IO_H   /* #IncludeGuards that prevent the double      */
#define IO_H   /* declaration of the functions. */

#include "cpair.h"

int stdinToArray(point** points, int *numberofPoints);
int arrayDivider(point* points, int pointCount, point** lows, point** highs, int *numberofLows, int *numberofHighs);
void toStdout(point* points, int pointCount);

#endif         //* IO_H
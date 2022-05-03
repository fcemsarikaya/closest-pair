/**     
*@file cpair.h
*@author Fidel Cem Sarikaya <fcemsarikaya@gmail.com>
*@date 03.12.2021
*
*@brief Provides the struct 'point' to the program.
*
* The cpair module. Contains a struct that enables to store a point on a two dimensional coordinate system.
**/

#ifndef CPAIR_H   /* #IncludeGuards that prevent the double      */
#define CPAIR_H   /* definition of the struct 'point'. */

typedef struct point {
    float X, Y;
} point;

#endif           //* CPAIR_H
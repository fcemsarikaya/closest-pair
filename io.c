/**     
*@file io.c
*@author Fidel Cem Sarikaya <fcemsarikaya@gmail.com>
*@date 06.11.2021
*
*@brief Implementation of the io module.
*
*Definitions of the input/output functions to be used in 'cpair'.
**/

#include "cpair.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Main input function.
 * @brief This function gets an input of strings with pairs of floats and puts them into a 'point' array.
 * @details Lines that are received using getline() are parsed with strtok() to catch float pairs. Then they are stored in a dynamic 'point' array to be given
 * to outside of the function through parameter 'points', while number of points is also given using 'numberofPoints'.
 * @param points Pointer to a 'point' array.
 * @param numberofPoints Pointer to an integer.
*/
int stdinToArray(point** points, int *numberofPoints) {
    *points = (point*) malloc(0); //'points' to be freed later
    if (*points == NULL) {return 1;}

    char *line = NULL;
    ssize_t read;
    size_t size = 0;
 
    int pointCount = 0;
    while (1) {
        read = getline(&line, &size, stdin);
        if (read == -1) {break;}

        char *token = strtok(line, " ");
        if (token == NULL) {
            return 2;}

        point* tmp = realloc(*points, ((pointCount + 1) * sizeof(point)));
        if (tmp == NULL) {return 1;}
        *points = tmp;

        int tokenCount = 0;
        while (token != NULL){
            float num;
            char ch;
            if (sscanf(token, "%f%c", &num, &ch) == 0) {
                return 2;}
            
            if (tokenCount == 0) {
                (*points)[pointCount].X = num;
                }
            else if (tokenCount == 1) {
                (*points)[pointCount].Y = num;
                }

            token = strtok(NULL, " ");
            if (tokenCount > 1) {
                return 2;}
            tokenCount++;
            }
        tokenCount = 0;
        pointCount++;
    }
    free(line);
    *numberofPoints = pointCount;
    return 0;
} 

/**
 * A function that divides a 'point' array to two part.
 * @brief This function splits an array of 'point' objects to two separate arrays according to the mean of the X attribute of the elements in the array.
 * @details Mean of the X attribute of the elements in array 'points' is calculated, and two dynamic arrays 'lows' and 'highs' are created. Then every element in
 * 'points' is either placed in 'lows' (if X attribute of the element is lower than or equal to mean) or in 'highs'. In the end, new arrays and their counters are given back
 * to outside of the function scope through parameters.
 * @param points A 'point' array.
 * @param pointCount Number of elements in 'points'.
 * @param lows A pointer to a 'point' array.
 * @param highs A pointer to a 'point' array.
 * @param numberofLows Pointer to an integer.
 * @param numberofHighs Pointer to an integer.
*/
int arrayDivider(point* points, int pointCount, point** lows, point** highs, int *numberofLows, int *numberofHighs) {
    float mean = 0;
    for (int i = 0; i < pointCount; i++) {
        mean += points[i].X;
    }
    mean /= pointCount;

    int lower = 0;
    for (int i = 0; i < pointCount; i++) {
        if (points[i].X <= mean) {lower++;}
    }
    *lows = malloc(lower * sizeof(point));                 //'lows' to be freed later
    if (*lows == NULL) {return 1;}

    *highs = malloc((pointCount - lower) * sizeof(point)); //'highs' to be freed later
    if (*highs == NULL) {return 1;}

    int lowCount = 0;
    int highCount = 0;
    for (int i = 0; i < pointCount; i++) {
        if (points[i].X <= mean) {
            (*lows)[lowCount].X = points[i].X;
            (*lows)[lowCount].Y = points[i].Y;
            lowCount++;
        }
        else {
            (*highs)[highCount].X = points[i].X;
            (*highs)[highCount].Y = points[i].Y;
            highCount++;
        }
    }
    *numberofLows = lowCount;
    *numberofHighs = highCount;
    return 0;
}

/**
 * Print function for 'point' arrays.
 * @brief This writes the contents of a 'point' array to the stdout.
 * @details Elements of the array is written to the stdout using fprintf() in a for-loop. Stdout gets flushed after every print operation to guarantee immediate print.
 * @param points A 'point' array.
 * @param numberofPoints Number of elements in 'points'.
*/
void toStdout(point* points, int pointCount) {
    for (int i = 0; i < pointCount; i++) {
        fprintf(stdout, "%f %f\n", points[i].X, points[i].Y);
        fflush(stdout);
    }
}
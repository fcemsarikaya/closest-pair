/**     
*@file cpair.c
*@author Fidel Cem Sarikaya <fcemsarikaya@gmail.com>
*@date 11.11.2021
*
*@brief Main program module.
*
* This program receives pairs of float values from stdin, which represents points in a two-dimensional coordinate system and finds the closest pair of points among them
* using a divide-and-conquer algorithm.
**/

#include "cpair.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <unistd.h>
#include <sys/wait.h>

static void usage(void);
static double distanceCalculator(point a, point b);
static int closestPairFinder(point *points, int pointCount, point *pair);

/** 
 * Program entry point.
 * @brief The program starts here. The program receives lines that contain float pairs that represent two dimensional coordinates of points and by splitting the array of
 * given points to two subarrays and calling itself for each part, finds and prints the closest pair of points recursively.
 * @details At first, lines are accepted from stdin and an array containing all the points is set. If array contains one element, program terminates without printing. With two
 * elements, program prints these points directly and terminates. In a case with more elements, program sets three pipes. Two for each child to write and one for the parent to 
 * read from its children. Then the program forks twice for each child and the pipes are altered to enable interprocess communication between relevant processes, 
 * according to the return value of fork. In the parent, input array is split to two arrays according to the mean value of X attributes of all elements in the array and if 
 * mean is not equal to every element in the array (both subarrays contain at least one element) each sub array is written to stdout for children to read. After the program
 * waits for its children to terminate and write their output to stdin for the parent process, program reads stdin again to parse points and finds the closest pair of points
 * within new points and prints this pair. 
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS or EXIT_FAILURE.
*/
int main(int argc, char const *argv[]) {
    if (argc != 1) {
            fprintf(stderr,"Usage: \t%s\n", argv[0]);
            return EXIT_FAILURE;}
     

    point* points;
    int pointCount = 0;
    int tmp;
    if ((tmp = stdinToArray(&points, &pointCount)) != 0) {
        if (tmp == 2) {usage();}
        else {return EXIT_FAILURE;}}

    if (pointCount < 2) {
        free(points);
        return EXIT_SUCCESS;
        }
    else if (pointCount == 2) {
        toStdout(points, pointCount);

        free(points);
        return EXIT_SUCCESS;
    }

    int fd_ch1[2];                         //
    int fd_ch2[2];                         // file descriptors for pipes: 0 being read end, 1 being write end
    int fd_parent[2];                      //
    
    if (pipe(fd_ch1) == -1) {
        fprintf(stderr, "Pipe failed");
        free(points);
        return EXIT_FAILURE;}

    if (pipe(fd_ch2) == -1) {
        fprintf(stderr, "Pipe failed");
        free(points);
        return EXIT_FAILURE;}

    if (pipe(fd_parent) == -1) {
        fprintf(stderr, "Pipe failed");
        free(points);
        return EXIT_FAILURE;}       

    int ch1_id = fork();
    int ch2_id = fork();

    if (ch1_id == 0) {
        close(fd_ch1[1]);                   //
        dup2(fd_ch1[0], STDIN_FILENO);      // read end is duplicated to stdin
        close(fd_ch1[0]);                   //

        close(fd_parent[0]);                //
        dup2(fd_parent[1], STDOUT_FILENO);  // write end is duplicated to stdout
        close(fd_parent[1]);                //

        close(fd_ch2[0]);                   // both pipe ends of child 2 closed
        close(fd_ch2[1]);                   //

        execlp("./cpair", "cpair", NULL);
    }
    else if (ch2_id == 0) {
        close(fd_ch2[1]);                   //
        dup2(fd_ch2[0], STDIN_FILENO);      // read end is duplicated to stdin
        close(fd_ch2[0]);                   //

        close(fd_parent[0]);                //
        dup2(fd_parent[1], STDOUT_FILENO);  // write end is duplicated to stdout
        close(fd_parent[1]);                //

        close(fd_ch1[0]);                   // both pipe ends of child 1 closed
        close(fd_ch1[1]);                   //
    
        execlp("./cpair", "cpair", NULL);
    }
    else {
        close(fd_parent[1]);                //
        dup2(fd_parent[0], STDIN_FILENO);   // read end is duplicated to stdin
        close(fd_parent[0]);                //

        point* lows;
        int lowCount = 0;
        point* highs;
        int highCount = 0;
        if (arrayDivider(points, pointCount, &lows, &highs, &lowCount, &highCount) != 0) {return EXIT_FAILURE;}
        
        int so_backup = dup(STDOUT_FILENO); // stdout is backed up for later

        point closestPair[2];
        if (lowCount == 0 || highCount == 0) {
        
            closestPair[0] = points[0];
            closestPair[1] = points[0];

        } else {
            close(fd_ch1[0]);                   //
            dup2(fd_ch1[1], STDOUT_FILENO);     // write end is duplicated to stdout
            close(fd_ch1[1]);                   //

            toStdout(lows, lowCount);
            close(STDOUT_FILENO);

            close(fd_ch2[0]);                   //
            dup2(fd_ch2[1], STDOUT_FILENO);     // write end is duplicated to stdout
            close(fd_ch2[1]);                   //

            toStdout(highs, highCount);
            close(STDOUT_FILENO);

            free(lows);
            free(highs);

            int exitStatus1;
            int exitStatus2;
            waitpid(ch1_id, &exitStatus1, 0);   // wait for the childs to end
            waitpid(ch2_id, &exitStatus2, 0);   //
            if (exitStatus1 != 0 || exitStatus2 != 0) {
                free(points);
                free(lows);
                free(highs);
                return EXIT_FAILURE;
            }

            point* newPoints;
            int newPointCount = 0;
            if (stdinToArray(&newPoints, &newPointCount) != 0) {
                free(points);
                return EXIT_FAILURE;}

            if (closestPairFinder(newPoints, newPointCount, closestPair) != 0) {
                free(points);
                return EXIT_FAILURE;} 
                
            free(newPoints);
        }
        dup2(so_backup, STDOUT_FILENO);     // stdout is restored from backup
        close(so_backup);                   //

        toStdout(closestPair, 2);
        close(STDOUT_FILENO);
    }
    free(points);
    return EXIT_SUCCESS;
}

/**
 * Usage function.
 * @brief This function writes helpful usage information to stderr about how program should be called.
 */
static void usage(void) {
    fprintf(stderr, "\nOnly pairs of float types are accepted in input lines! \nUsage: FLOAT FLOAT\n");
    exit(EXIT_FAILURE);
    }

/**
 * Distance calculator.
 * @brief This function calculates the distance between two points.
 * @details This function uses the pythagorean distance formula to calculate the distance between two objects of 'point'.
 * @param a A 'point'.
 * @param b A 'point'.
*/
static double distanceCalculator(point a, point b) {
    return sqrt(pow(((double)b.X - (double)a.X), 2) + pow(((double)b.Y - (double)a.Y), 2));
}

/**
 * Closest pair finder for 'point' arrays.
 * @brief This function finds the two points that are closest to each other than other points in input arrays.
 * @details shortestDistance variable has the maximum double value at first. Then for every combination of points, the distance between these points is calculated and saved to 
 * shortestDistance variable, if it's less then what's already in the variable. These pair of points are also saved to paramater variable 'pair'. In the end, 'pair' is altered
 * to hold the closest pair in the array 'points'. 
 * @param points A 'point' array.
 * @param pointCount Number of elements in 'points'.
 * @param pair A 'point' array.
*/
static int closestPairFinder(point *points, int pointCount, point *pair) {
    double shortestDistance = DBL_MAX;

    if (pointCount < 1) {return 1;}
    else if (pointCount == 1) {
        pair[0] = points[0];
        pair[1] = points[0];
    }
    else {
        for (int i = 0; i < pointCount; i++) {
            for (int j = 0; j < pointCount; j++) {
                if (i == j) {continue;}
            
                double tmp = distanceCalculator(points[i], points[j]);
                if (tmp < shortestDistance) {
                    shortestDistance = tmp;
                    pair[0] = points[i];
                    pair[1] = points[j];
                    }
                }
            }
    }
    return 0;
}
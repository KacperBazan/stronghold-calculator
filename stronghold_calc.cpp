/*
 * Program Name: stronghold_calc.cpp
 * Program Description: C++ Program that generates average distance to a Minecraft stronghold depending on distance from origin.
 * Date Started: Mar/10/2021
 * Author: Kacper Bazan
 * Author Link: https://github.com/KacperBazan
 */

/*
 * References: https://minecraft.gamepedia.com/Stronghold
 */

#include <iostream> //cout
#include <fstream>  //ofstream
#include <math.h>   //cos
#include <random>   //default_random_engine...

#define PI 3.14159265

std::default_random_engine ranGen;
std::uniform_real_distribution<double> ranTheta(0.0, 2 * PI);
std::uniform_real_distribution<double> ranRadius(1280.0, 2816.0);

const int NUM_SH = 1000;           //Number of stronghold batches.
double strongholds[NUM_SH][3][2]; //Batches, strongholds, theta + radius

const double MAXDIST = 3000;   //Distance you could build a portal. Realistically between 0 and 2816.
const int RES_WIDTH = 3000;       //Increments of distance between 0 and MAXDIST.
const int RES_THETA = 100;       //Resolution of angle rotated along circle.
double distances[RES_WIDTH][2]; //Radius increments, radius value + average distance.

void generateStrongholds()
{
    for (int i = 0; i < NUM_SH; i++)
    {
        double theta = ranTheta(ranGen); //generates initial theta for the set of 3 strongholds.
        for (int j = 0; j < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); j++)
        {
            theta = theta + ((2 * PI) / 3);                                     //adjusts each subsequent stronghold to be 120 degrees apart.
            strongholds[i][j][0] = (theta > 2 * PI) ? theta - (2 * PI) : theta; //clamps all angles to be between 0 and 2PI.
            strongholds[i][j][1] = ranRadius(ranGen);
        }
    }
}

void generateDistances()
{
    double theta;
    double radius;
    double distance1, distance2, distance3;
    double distRunning = 0;

    for (int i = 0; i <= RES_WIDTH; i++)
    {
        radius = i * MAXDIST / RES_WIDTH;
        for (int j = 0; j < NUM_SH; j++)
        {
            for (int k = 0; k < RES_THETA; k++)
            {
                theta = k * (2 * PI) / RES_THETA;
                distance1 = pow(radius, 2) + pow(strongholds[j][0][1], 2) - (2 * radius * strongholds[j][0][1] * cos(strongholds[j][0][0] - theta));
                distance2 = pow(radius, 2) + pow(strongholds[j][1][1], 2) - (2 * radius * strongholds[j][1][1] * cos(strongholds[j][1][0] - theta));
                distance3 = pow(radius, 2) + pow(strongholds[j][2][1], 2) - (2 * radius * strongholds[j][2][1] * cos(strongholds[j][2][0] - theta));
                distRunning = distRunning + sqrt(std::min(std::min(distance1, distance2), distance3)); //Running total of all distances
            }
        }
        distances[i][0] = radius;
        distances[i][1] = distRunning / (RES_THETA * NUM_SH);
        distRunning = 0;
    }
}

void writeToFile(std::string fn)
{
    std::ofstream outfile; //cout replacement

    //attempt to open file...
    outfile.open(fn);

    //verify that the file was opened...
    if (outfile.is_open())
    {
        //process the file...

        /* Prints the finalized distances list. */
        for (int i = 0; i <= RES_WIDTH; i++)
        {
            outfile << "Radius: " << distances[i][0] << " | Average Distance: " << distances[i][1] <<std::endl;
        }
        outfile << std::endl;

        //close it
        outfile.close();
    }
    else
        std::cout << "ERROR!\n";
}

int main()
{
    generateStrongholds();
    generateDistances();
    writeToFile("stronghold_output.txt");
}
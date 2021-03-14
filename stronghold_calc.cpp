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
std::uniform_real_distribution<double> r1(1280.0, 2816.0);   //3
std::uniform_real_distribution<double> r2(4352.0, 5888.0);   //6
std::uniform_real_distribution<double> r3(7424.0, 8960.0);   //10
std::uniform_real_distribution<double> r4(10496.0, 12032.0); //15
std::uniform_real_distribution<double> r5(13568.0, 15104.0); //21
std::uniform_real_distribution<double> r6(16640.0, 18176.0); //28
std::uniform_real_distribution<double> r7(19712.0, 21248.0); //36
std::uniform_real_distribution<double> r8(22784.0, 24320.0); //9

std::uniform_real_distribution<double> ranRadius[8] = {r1, r2, r3, r4, r5, r6, r7, r8};

const int COUNT_SH[8] = {3, 6, 10, 15, 21, 28, 36, 9};
const int NUM_SH = 100;               //Number of stronghold batches.
double strongholds[NUM_SH][128][2]; //Batches, strongholds, theta + radius
const double MAXDIST = 25000;       //Distance you could build a portal. Realistically between 0 and 24320.
const int RES_WIDTH = 5000;          //Increments of distance between 0 and MAXDIST.
const int RES_THETA = 10;           //Resolution of angle rotated along circle.
double distances[RES_WIDTH][2];     //Radius increments, radius value + average distance.

void generateStrongholds()
{
    for (int i = 0; i < NUM_SH; i++)
    {
        int index = 0, count = 0;
        double theta = ranTheta(ranGen); //generates initial theta for the set of 3 strongholds.
        for (int j = 0; j < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); j++)
        {
            count++;
            if (count > COUNT_SH[index])
            {
                double theta = ranTheta(ranGen);
                count = 1;
                index++;
            }
            theta = theta + ((2 * PI) / COUNT_SH[index]) - (2 * PI * ((theta + ((2 * PI) / COUNT_SH[index])) > 2 * PI));
            strongholds[i][j][0] = theta;
            strongholds[i][j][1] = ranRadius[index](ranGen);
        }
    }
}

void generateDistances()
{
    double theta;
    double radius;
    double d = INFINITY;
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

                for (int l = 0; l < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); l++)
                {
                    d = std::min(d, (pow(radius, 2) + pow(strongholds[j][l][1], 2) - (2 * radius * strongholds[j][l][1] * cos(strongholds[j][l][0] - theta))));
                }
                distRunning = distRunning + sqrt(d); //Running total of all distances
                d = INFINITY;
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
            outfile << "Radius: " << distances[i][0] << " | Average Distance: " << distances[i][1] << std::endl;
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

    // for (size_t i = 0; i < NUM_SH; i++)
    // {
    //     std::cout << "BATCH NUMBER: " << i << std::endl;
    //     for (size_t j = 0; j < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); j++)
    //     {
    //         std::cout << "--> S #: " << j << " | Theta: " << strongholds[i][j][0] << " , Radius: " << strongholds[i][j][1] << std::endl;
    //         //std::cout << strongholds[i][j][0] << std::endl;
    //     }
    // }

    generateDistances();
    writeToFile("stronghold_output.txt");
}
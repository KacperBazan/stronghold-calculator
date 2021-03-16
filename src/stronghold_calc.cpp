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
std::uniform_real_distribution<double> r1(1280.0, 2816.0);
std::uniform_real_distribution<double> r2(4352.0, 5888.0);
std::uniform_real_distribution<double> r3(7424.0, 8960.0);
std::uniform_real_distribution<double> r4(10496.0, 12032.0);
std::uniform_real_distribution<double> r5(13568.0, 15104.0);
std::uniform_real_distribution<double> r6(16640.0, 18176.0);
std::uniform_real_distribution<double> r7(19712.0, 21248.0);
std::uniform_real_distribution<double> r8(22784.0, 24320.0);

/* Array holding random generators to be easily refered to later. */
std::uniform_real_distribution<double> ranRadius[8] = {r1, r2, r3, r4, r5, r6, r7, r8};

const int COUNT_SH[8] = {3, 6, 10, 15, 21, 28, 36, 9}; //Array holding number of strongholds in each respective ring.
const int NUM_SH = 100;                                //Number of stronghold batches.
const double STARTDIST = 0;                            //Minimum distance one would build a portal.
const double MAXDIST = 25000;                          //Maximum distance one would build a portal.
const int RES_WIDTH = 5000;                            //Increments of distance between STARTDIST and MAXDIST.
const int RES_THETA = 10;                              //Increments of angle between 0 and 2PI.

double strongholds[NUM_SH][128][2]; //Batches, strongholds, theta + radius of each stronghold
double distances[RES_WIDTH][2];     //Radius increments, radius value + average distance.

void printToConsole(int count, int max)
{
    /* Since the load count never reaches -1, the clear function is defined by using -1. */
    if (count == -1)
    {
        /* Prints 50 lines to clear the terminal. */
        for (int c = 0; c < 50; c++)
        {
            std::cout << "" << std::endl;
        }
        std::cout << "Loading :";
        for (int c = 0; c < max; c++)
        {
            std::cout << "-";
        }
        std::cout << " 0 / " << max << std::endl;
    }
    else
    {
        /* Loading bar using '-' and '#'. */
        std::cout << "Loading :";
        for (int c = 0; c < count; c++)
        {
            std::cout << "#";
        }
        for (int c = 0; c < max - count; c++)
        {
            std::cout << "-";
        }
        std::cout << " " << count << " / " << max << std::endl;
    }
}


void generateStrongholds()
{
    for (int i = 0; i < NUM_SH; i++)
    {
        /* Index and Count are used to refer to COUNT_SH, or the number of
        strongholds in each respective ring. */
        int index = 0, count = 0;
        double theta = ranTheta(ranGen); 
        for (int j = 0; j < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); j++)
        {
            count++;
            /* Since each ring has a different number of equally spaced strongholds,
            count is incremented once we exhaust all the strongholds in a ring, and
            we generate a new theta to begin spacing out strongholds. We know we exhaust
            all the strongholds in a ring when the index value matches the count of
            generated strongholds */
            if (count > COUNT_SH[index])
            {
                double theta = ranTheta(ranGen);
                count = 1;
                index++;
            }
            /* Subtracts 2PI from the angle if it exceeds 2PI, effectively clamping the value between 0 and 2PI. */
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
    double distRunning = 0;

    /* Variables to determine loading bar */
    int loadMax = 100;
    double loadCount = 1;

    printToConsole(-1, loadMax);

    for (int i = 0; i <= RES_WIDTH; i++)
    {
        /* Updates loading bar */
        if (i >= ((RES_WIDTH / loadMax) * loadCount))
        {
            printToConsole(loadCount, loadMax);
            loadCount++;
        }

        radius = i * MAXDIST / RES_WIDTH;
        for (int j = 0; j < NUM_SH; j++)
        {
            for (int k = 0; k < RES_THETA; k++)
            {
                theta = k * (2 * PI) / RES_THETA;

                for (int l = 0; l < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); l++)
                {
                    /* Checks each stronghold and saves minimum distance to d*/
                    d = std::min(d, (pow(radius, 2) + pow(strongholds[j][l][1], 2) - (2 * radius * strongholds[j][l][1] * cos(strongholds[j][l][0] - theta))));
                }
                /*Saves a running total of the minimum distances */
                distRunning = distRunning + sqrt(d); 
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
    generateDistances();
    writeToFile("stronghold_output.txt");
}
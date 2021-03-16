/*
 * Program Name: stronghold_highroll_calc.cpp
 * Program Description: C++ Program that generates best chances at blinding at a particular distance.
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

/*
The wiki states that strongholds in the first ring generate between
1280 and 2816 blocks. However, the strongholds initial RNG call is between
1408 and 2688 blocls, 128 blocks less in each direction. This is because
the generated stronghold snaps due to biome placement somewhere in the 128
block range. More research and data collection needs to be made regarding
this, but for now, the adjusted range covers the vast majority of stronghold
generation.
*/
std::uniform_real_distribution<double> ranRadius(1408.0, 2688.0);

const int NUM_SH = 4000;       //Number of stronghold batches.
const double MAXDIST = 2720;   //Maximum distance one would build a portal.
const double STARTDIST = 1360; //Minimum distance one would build a portal.
const int THRESHOLD = 20;      //Distance stronghold needs to be to deem blind travel a success.
const int RES_WIDTH = 5000;    //Increments of distance between STARTDIST and MAXDIST.
const int RES_THETA = 2000;    //Increments of angle between 0 and 2PI.

double strongholds[NUM_SH][3][2]; //Batches, strongholds, theta + radius of each stronghold
double distances[RES_WIDTH][2];   //Radius increments, radius value + average distance.

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
        double theta = ranTheta(ranGen);
        for (int j = 0; j < sizeof(strongholds[0]) / sizeof(strongholds[0][0]); j++)
        {
            /* Subtracts 2PI from the angle if it exceeds 2PI, effectively clamping the value between 0 and 2PI. */
            theta = theta + ((2 * PI) / 3) - (2 * PI * ((theta + ((2 * PI) / 3)) > 2 * PI));
            strongholds[i][j][0] = theta;
            strongholds[i][j][1] = ranRadius(ranGen);
        }
    }
}

void generateOdds(int t)
{
    double theta;
    double radius;
    double d = INFINITY;

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

        int highRoll = 0;
        radius = (i * (MAXDIST - STARTDIST) / RES_WIDTH) + STARTDIST;
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

                /* Checks if stronghold is within threshold distance */
                if (d <= pow(t, 2))
                {
                    highRoll++;
                }
                d = INFINITY;
            }
        }
        distances[i][0] = radius;
        distances[i][1] = highRoll;
        highRoll = 0;
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
            //outfile << "Radius: " << distances[i][0] << " | Average Distance: " << distances[i][1] << std::endl;
            outfile << distances[i][0] << ", " << distances[i][1] << std::endl;
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
    generateOdds(THRESHOLD);
    writeToFile("stronghold_highroll_output.txt");
}
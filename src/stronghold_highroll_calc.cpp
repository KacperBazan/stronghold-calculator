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
#include <vector>

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

/* Variables to determine loading bar */
const int LOADMAX = 100;
int loadCount = 1;

int BATCHES = 250;       //Number of stronghold batches.
int RES_WIDTH = 250;     //Increments of distance between STARTDIST and MAXDIST.
int RES_THETA = 10;      //Increments of angle between 0 and 2PI.
double THRESHOLD = 1000; //Distance stronghold needs to be to deem blind travel a success.
double STARTDIST = 0;    //Minimum distance one would build a portal.
double MAXDIST = 3000;   //Maximum distance one would build a portal.

std::vector<std::vector<std::vector<double>>> batches; //Batches of strongholds that will be generated [BATCHES][DESIRED_SH][2]
std::vector<std::vector<double>> highRollDistances;    //Vector holding every distance increment along with its high roll distance [RES_WIDTH][2] distances[RES_WIDTH][2];   //Radius increments, radius value + average distance.

void userInput()
{
    std::string u;
    std::cout << "Would you like to run with default settings? [y/n]: ";
    std::cout << std::endl;
    std::cin >> u;
    if (u == "n" || u == "N" || u == "no" || u == "NO")
    {
        std::cout << "Block threshold for successful blind travel (THRESHOLD):  ";
        std::cin >> THRESHOLD;
        std::cout << "No. of stronghold batches (BATCHES):  ";
        std::cin >> BATCHES;
        std::cout << "Minimum portal distance (STARTDIST):  ";
        std::cin >> STARTDIST;
        std::cout << "Maximum portal distance (MAXDIST):    ";
        std::cin >> MAXDIST;
        std::cout << "No. of width increments (RES_WIDTH):  ";
        std::cin >> RES_WIDTH;
        std::cout << "No. of angle increments (RES_THETA):  ";
        std::cin >> RES_THETA;
        std::cout << std::endl;
    }
}

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
    /* Initialize multidimensional vector */
    for (int i = 0; i < BATCHES; i++)
    {
        std::vector<std::vector<double>> stronghold;
        for (int j = 0; j < 3; j++)
        {
            std::vector<double> data;
            for (int k = 0; k < 2; k++)
            {
                data.push_back(0);
            }
            stronghold.push_back(data);
        }
        batches.push_back(stronghold);
    }

    for (int i = 0; i < BATCHES; i++)
    {
        double theta = ranTheta(ranGen);
        for (int j = 0; j < 3; j++)
        {
            /* Subtracts 2PI from the angle if it exceeds 2PI, effectively clamping the value between 0 and 2PI. */
            theta = theta + ((2 * PI) / 3) - (2 * PI * ((theta + ((2 * PI) / 3)) > 2 * PI));
            batches[i][j][0] = theta;
            batches[i][j][1] = ranRadius(ranGen);
        }
    }
}

void generateOdds(int t)
{
    /* Initialize multidimensional vector */
    for (int i = 0; i < RES_WIDTH; i++)
    {
        std::vector<double> data;
        for (int j = 0; j < 2; j++)
        {
            data.push_back(0);
        }
        highRollDistances.push_back(data);
    }

    double theta;
    double radius;
    double d = INFINITY;
    int highRoll = 0;

    double loadInc = double(RES_WIDTH) / double(LOADMAX);
    printToConsole(-1, LOADMAX);

    for (int i = 0; i < RES_WIDTH; i++)
    {
        /* Updates loading bar */
        if (i + 1 >= loadInc * loadCount)
        {
            printToConsole(loadCount, LOADMAX);
            loadCount++;
        }

        radius = (i * (MAXDIST - STARTDIST) / double(RES_WIDTH)) + STARTDIST;
        for (int j = 0; j < BATCHES; j++)
        {
            for (int k = 0; k < RES_THETA; k++)
            {
                theta = k * (2 * PI) / RES_THETA;

                for (int l = 0; l < 3; l++)
                {
                    /* Checks each stronghold and saves minimum distance to d*/
                    d = std::min(d, (radius * radius + batches[j][l][1] * batches[j][l][1] - (2 * radius * batches[j][l][1] * cos(batches[j][l][0] - theta))));
                }

                /* Checks if stronghold is within threshold distance */
                if (d <= (t * t))
                {
                    highRoll++;
                }
                d = INFINITY;
            }
        }
        highRollDistances[i][0] = radius;
        highRollDistances[i][1] = highRoll; //Divide by RES_THETA * BATCHES
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
        for (int i = 0; i < RES_WIDTH; i++)
        {
            outfile << highRollDistances[i][0] << ", " << highRollDistances[i][1] << std::endl;
        }
        std::cout << "Calculations Complete. Please check your output file." << std::endl;

        //close it
        outfile.close();
    }
    else
        std::cout << "ERROR!\n";
}

int main()
{
    userInput();
    generateStrongholds();
    generateOdds(THRESHOLD);
    writeToFile("highroll_chance.txt");
}
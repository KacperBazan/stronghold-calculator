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
#include <vector>

#define PI 3.14159265

std::default_random_engine ranGen;
std::uniform_real_distribution<double> ranTheta(0.0, 2 * PI);
std::uniform_real_distribution<double> r1(1408, 2688);
std::uniform_real_distribution<double> r2(4480, 5760);
std::uniform_real_distribution<double> r3(7552, 8832);
std::uniform_real_distribution<double> r4(10624, 11904);
std::uniform_real_distribution<double> r5(13696, 14976);
std::uniform_real_distribution<double> r6(16768, 18048);
std::uniform_real_distribution<double> r7(19840, 21120);
std::uniform_real_distribution<double> r8(22912, 24192);

/* Array holding random generators to be easily refered to later. */
std::uniform_real_distribution<double> ranRadius[8] = {r1, r2, r3, r4, r5, r6, r7, r8};

const int COUNT_SH[8] = {3, 6, 10, 15, 21, 28, 36, 9}; //Array holding number of strongholds in each respective ring.

/* Variables to determine loading bar */
const int LOADMAX = 100;
double loadCount = 1;

int BATCHES = 50;      //Number of stronghold batches.
int DESIRED_SH = 128;  //Value of strongholds simulated world has. Limited between 3 (first ring) and 128 (all rings).
int RES_WIDTH = 250;   //Increments of distance between STARTDIST and MAXDIST.
int RES_THETA = 25;    //Increments of angle between 0 and 2PI.
double STARTDIST = 0;  //Minimum distance one would build a portal.
double MAXDIST = 3000; //Maximum distance one would build a portal.

std::vector<std::vector<std::vector<double>>> batches; //Batches of strongholds that will be generated [BATCHES][DESIRED_SH][2]
std::vector<std::vector<double>> avgDistances;         //Vector holding every distance increment along with its average distance [RES_WIDTH][2]

void userInput()
{
    std::string u;
    std::cout << "Would you like to run with default settings? [y/n]: ";
    std::cout << std::endl;
    std::cin >> u;
    if (u == "n" || u == "N" || u == "no" || u == "NO")
    {
        DESIRED_SH = 0;
        while (DESIRED_SH == 0)
        {
            std::cout << "Number of desired strongholds in world. Always between 3 and 128. [0 for help] (DESIRED_SH): ";
            std::cin >> DESIRED_SH;
            if (DESIRED_SH == 0)
            {
                std::cout << std::endl;
                std::cout << "Strongholds generate in eight different rings starting around 1408 blocks away from the origin." << std::endl;
                std::cout << "There are 3 strongholds in the first ring, followed by 6, 10, 15, 21, 28, 36, and finally 9. 128 in total." << std::endl;
                std::cout << "Speedrunners only really care about the first ring, and therefore the first three strongholds." << std::endl;
                std::cout << "This program calculates the average distance graph for all 128 strongholds." << std::endl;
                std::cout << "However, if you'd like to change that, change the DESIRED_SH value." << std::endl;
                std::cout << "If you want to calculate up to the 'nth' ring, make sure to add the strongholds from previous rings." << std::endl;
                std::cout << std::endl;
            }
        }
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
        for (int j = 0; j < DESIRED_SH; j++)
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
        /* Index and Count are used to refer to COUNT_SH, or the number of
        strongholds in each respective ring. */
        int index = 0, count = 0;
        double theta = ranTheta(ranGen);
        for (int j = 0; j < DESIRED_SH; j++)
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
            batches[i][j][0] = theta;
            batches[i][j][1] = ranRadius[index](ranGen);
        }
    }
}

void generateDistances()
{
    /* Initialize multidimensional vector */
    for (int i = 0; i < RES_WIDTH; i++)
    {
        std::vector<double> data;
        for (int j = 0; j < 2; j++)
        {
            data.push_back(0);
        }
        avgDistances.push_back(data);
    }

    double theta;
    double radius;
    double d = INFINITY;
    double distRunning = 0;

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

                for (int l = 0; l < DESIRED_SH; l++)
                {
                    /* Checks each stronghold and saves minimum distance to d*/
                    d = std::min(d, (radius * radius + batches[j][l][1] * batches[j][l][1] - (2 * radius * batches[j][l][1] * cos(batches[j][l][0] - theta))));
                }
                /*Saves a running total of the minimum distances */
                distRunning = distRunning + sqrt(d);
                d = INFINITY;
            }
        }
        avgDistances[i][0] = radius;
        avgDistances[i][1] = distRunning / (RES_THETA * BATCHES);
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
        for (int i = 0; i < RES_WIDTH; i++)
        {
            outfile << avgDistances[i][0] << ", " << avgDistances[i][1] << std::endl;
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
    generateDistances();
    writeToFile("avg_distance.txt");
}
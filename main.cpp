#include "FileLoader.h"
#include "RobotState.h"
#include "History.h"
#include "DynamicOccupancyGridMap.h"
#include <tuple>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <cmath>

using namespace std;

// Some constant values here that can be played with
const int NUM_PARTICLES = 3;
//Positional uncertainty
const double sigma_pos[3] = {0.3, 0.3, 0.01};
const int X_MIN = 0;
const int X_MAX = 100;
const int Y_MIN = 0;
const int Y_MAX = 100;
const int T_MIN = 0;
const int T_MAX = 359;
bool is_initialized = false;

typedef struct particle
{
    int id;
    double x;
    double y;
    double theta;
    double weight;
} particle;

typedef struct MAP
{
    //NEED TO SETUP??????
} MAP;

typedef struct cell
{
    int x;
    int y;
} cell;

bool operator==(const cell lhs, const cell rhs)
{
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
}

particle particleArray[NUM_PARTICLES];

void init()
{
    default_random_engine gen;

    for (int index = 0; index < NUM_PARTICLES; index++)
    {

        uniform_real_distribution<double> dist_x(X_MIN, X_MAX);
        uniform_real_distribution<double> dist_y(Y_MIN, Y_MAX);
        uniform_real_distribution<double> dist_theta(T_MIN, T_MAX);

        double sample_x, sample_y, sample_theta;

        sample_x = dist_x(gen);
        sample_y = dist_y(gen);
        sample_theta = dist_theta(gen);

        particle new_particle;
        new_particle.id = index;
        new_particle.x = sample_x;
        new_particle.y = sample_y;
        new_particle.theta = sample_theta;
        new_particle.weight = 1.0;
        particleArray[index] = new_particle;
    }
    is_initialized = true;
}

void TestInit()
{
    for (int index = 0; index < NUM_PARTICLES; index++)
    {
        particle new_particle;
        new_particle.id = index;
        new_particle.x = index;
        new_particle.y = index;
        new_particle.theta = index;
        new_particle.weight = 1.0;
        particleArray[index] = new_particle;
    }
    is_initialized = true;
}

void printParticle(int index)
{
    cout << "ID: " + to_string(particleArray[index].id) << endl
         << "X: " + to_string(particleArray[index].x) << endl
         << "Y: " + to_string(particleArray[index].y) << endl
         << "Theta: " + to_string(particleArray[index].theta) << endl
         << "Weight: " + to_string(particleArray[index].weight) << endl;
}

void printAllParticles()
{
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        printParticle(i);
    }
}

double toRadians(int degree)
{
    return degree * M_PI / 180;
}

double constrainAngle(double x)
{
    x = fmod(x + 180, 360);
    if (x < 0)
        x += 360;
    return x - 180;
}

void motionModel(double u[])
{
    //change u to have just one distance and angle

    default_random_engine gen;
    uniform_real_distribution<double> distXY(0, 0.04);
    uniform_real_distribution<double> distTheta(0, 0.01);

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        double hypot = u[0];
        double deltaAngle = u[1];
        //double angle = toRadians(particleArray[i].theta);
        double angle = constrainAngle(particleArray[i].theta + u[1]);
        double angleRad = toRadians(angle);

        double tempX = (u[0] * cos(angleRad) + particleArray[i].x); // + distXY(gen); Noise is already in u, don't need to add
        double tempY = (u[0] * sin(angleRad) + particleArray[i].y); // + distXY(gen);

        if (tempX > X_MAX)
            tempX = X_MAX;
        if (tempX < X_MIN)
            tempX = X_MIN;
        if (tempY > Y_MAX)
            tempY = Y_MAX;
        if (tempY < Y_MIN)
            tempY = Y_MIN;

        particleArray[i].x = tempX;
        particleArray[i].y = tempY;
        particleArray[i].theta = angle;
    }
}

void measModel(LaserZ z, OccupancyGrid *ogrid)
{
    /**
     * Proposed way to update weights:
     * 1) Get path of the laser
     * 2) At the end of the path, check to see if the laser reported occupied, or free (reached max range), and get the end cell
     * 3) Check same path off of each particle, see if end cells match
     * 4) If end cells DON'T match, automatic fail, decrement weight in some way
     * 5) If end cells DO match, need to check entire path to make sure they match,
     *      If they do match, best result, highly likely candidate
     *      If they don't match, assign weight based on how close: Very early mis-match along path = decrement weight
     *                                                             Mis-match close to the end, possible assign tiny amount/don't decrement, could be from error
     * */

    //For each particle:
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particle currParticle = particleArray[i];

        //for each laser to be checked as if it came from each particle
        for (int r = 0; r < z.getLaserCount(); r++)
        {
            double currLaserDist = z.getMeasurement(r);
            double currLaserAngle = z.getLaserAngle(r);

            //add the relative anlge of the laser to the direction the particle is facing to get the actual angle
            double currWorkingAngle = constrainAngle(currParticle.theta + currLaserAngle);

            //break the measred distance down into x and y parts
            double xComponent = currLaserDist * cos(currWorkingAngle);
            double yComponent = currLaserDist * sin(currWorkingAngle);
            
            //add the components to the current x and y position to get the endpoint
            double xEnd = currParticle.x + xComponent;
            double yEnd = currParticle.y + yComponent;

        }
    }
}

/**
void measModel(double laserData[], double angles[], MAP priorMap) //TODO This needs to get the map figured out
{
    double allWeights[NUM_PARTICLES] = {};

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        double particleX = particleArray[i].x;
        double particleY = particleArray[i].y;
        double particleT = particleArray[y].theta;
        double pos[3] = {particleX, particleY, particleT};

        vector<double> particleDistances; //This will hold the weird weighting system I use????

        for (int u = 0; u < sizeof(laserData); u++)
        {
            double xEnd = laserData[u] * cos(toRadians(pos[2] + angles[u])) + pos[0];
            double yEnd = laserData[u] * sin(toRadians(pos[2] + angles[u])) + pos[1];

            double rayAngle = angles[u] + pos[2];
            //how big of steps along the data to make
            double step = 1;
            double numSteps = int(laserData[u] / step);
            vector<cell> cellIndexList;

            for (int r = 0; r < numSteps; r++)
            {
                double lan = r / numSteps;
                double x = pos[0] + (xEnd - pos[0]) * lan;
                double y = pos[1] + (yEnd - pos[1]) * lan;

                if (x > X_MAX)
                    x = X_MAX;
                if (x < X_MIN)
                    x = X_MIN;
                if (y > Y_MAX)
                    y = Y_MAX;
                if (y < Y_MIN)
                    y = Y_MIN;

                cell index; //FIX THIS WITH AN ACTUAL GET CELL INDEX FUNCTION??????????
                index.x = floor(x);
                index.y = floor(y);
                bool found = false;
                for (int m = 0; m < cellIndexList.size(); m++)
                {
                    if (cellIndexList[m] == index)
                    {
                        found = true;
                    }
                }
                if (!found)
                    cellIndexList.push_back(index);
            }

            //NEED TO ASSIGNE PARTICLES WEIGHTS
        }
    }
}
*/

void resample()
{
    //copy the existing list of particles
    vector<particle> particleArrayCopy;
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particleArrayCopy.push_back(particleArray[i]);
    }

    //vector of weights of the particles
    vector<int> weights;
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        weights.push_back(particleArrayCopy[i].weight);
    }

    //Object of random number engine class that generate pseudo-random numbers
    //NOTE: http://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
    mt19937 gen;
    //Object for generating discrete disturbutions based on the weights vecotr
    discrete_distribution<int> weight_dist(weights.begin(), weights.end());

    //With the discrete distribution pick out particles according to their
    //weights. The higher the weight of the particle, the higher are the chances
    //of the particle being included multiple times.
    //Discrete_distribution is used here to pick particles with the appropriate
    //weights(i.e. which meet a threshold)
    //http://www.cplusplus.com/reference/random/discrete_distribution/
    //NOTE: Here is an example which helps with the understanding
    //      http://coliru.stacked-crooked.com/a/3c9005a4cc0ed9d6
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        //Append the particle to the new list
        //NOTE: Calling weights_dist with the generator returns the index of one
        //      of weights in the vector which was used to generate the distribution.
        particleArray[i] = particleArrayCopy[weight_dist(gen)];
    }
}

int main(int argc, char *argv[])
{
    // Load Prior Occupancy Grid into memory
    History history;
    FileLoader loader;
    vector<vector<double>> rawgrid = loader.loadGridMap("occupancy_grid.omap"); // This is the prior map before boxes were moved and the current experiment was ran
    std::tuple<double,double> xlim((double)0.0, (double)rawgrid[0].size());
    std::tuple<double,double> ylim((double)0.0, (double) rawgrid.size());
    OccupancyGrid ogrid = OccupancyGrid(xlim, ylim, rawgrid);
    // Load Controls and Measurements from experiment into memory
    loader.loadNumSteps("number_of_steps.data", &history);
    loader.loadSensorAngles("Angles.data");
    loader.loadMeasurements("Measurements (1).data", &history);
    loader.loadNoisyMeasurements("Measurements_Noisy (1).data", &history);
    loader.loadControls("Controls (1).data", &history);
    loader.loadNoisyControls("Controls_Noisy (1).data", &history);
    loader.loadState("State (1).data", &history);
    // Initialize Particle Filter -Dylan made this
    init();
    // Setup Plotting - ?
    // Loop through the data stream
    for (std::uint32_t t = 0; t < history.getNumSteps(); t++)
    {
        // Extract data (get measurement z and control u at this time step t)
        LaserZ z = history.getNoisyMeasurement(t);
        ControlU u = history.getNoisyControl(t);
        RobotState x_true = history.getState(t);   // This is the true value of the state. We can use the initial value, but nothing else, unless it's for testing.
        LaserZ z_true = history.getMeasurement(t); // Do not use in PF Algorithm - This is the true value of the measurements. Just for testing.
        ControlU u_true = history.getControl(t);   // Do not use in PF Algorithm - This is the true value of the controls. Just for testing.
        // Operate on data to run particle filter algorithm -
        double uarg[2] = {u.getDDist(), u.getDTheta()};
        motionModel(uarg);
        // measModel(z, dgrid);
        //MAIN LOOP
        /**
         * for loop through each step of the robot
         * {
         *  motionModel //Pass in movement, apply movement to every partile. Needs u[]: distance moved and change in angle
         *  weigtingFunction****NEED TO FIGURE OUT //Figures out whiche particles are garbage and weights them.  Needs laser data[], angles[], and map
         *  resample //Resmaples based on weight, higher = more likely to move on/propegate
         * }
         * /
         **/

        // Use extended sensor model for mapping problem
        // Plot
    }
    return 0;
}
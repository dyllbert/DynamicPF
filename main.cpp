#include "DynamicOccupancyGridMap.h"
#include "FileManager.h"
#include "History.h"
#include "RobotState.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <math.h>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Some constant values here that can be played with
const int NUM_PARTICLES = 100;
// Positional uncertainty
const double sigma_pos[3] = {0.3, 0.3, 0.01};
const int X_MIN = 0;
const int X_MAX = 100;
const int Y_MIN = 0;
const int Y_MAX = 100;
const int T_MIN = 0;
const int T_MAX = 359;
const double MAX_LASER_RANGE = 128;
bool is_initialized = false;

typedef struct particle
{
    int id;
    double x;
    double y;
    double theta;
    double weight;
    DynamicOccupancyGridMap map;
} particle;

typedef struct cell
{
    int x;
    int y;
} cell;

bool operator==(const cell lhs, const cell rhs) { return ((lhs.x == rhs.x) && (lhs.y == rhs.y)); }

particle particleArray[NUM_PARTICLES];
vector<RobotState> particleCapture; // Used to save the state of the particles every so often to be
                                    // displayed in the simulation

void init(tuple<double, double> xlim, tuple<double, double> ylim, vector<vector<double>> init_grid,
          vector<vector<int>> st_mtx, double p_occ_from_free, double p_free_from_occ)
{
    default_random_engine gen;
    particleCapture.clear();
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
        new_particle.weight = 0;
        new_particle.map = DynamicOccupancyGridMap(xlim, ylim, init_grid, st_mtx, p_occ_from_free,
                                                   p_free_from_occ);
        particleArray[index] = new_particle;
        RobotState rs(new_particle.x, new_particle.y, new_particle.theta);
        particleCapture.push_back(rs);
    }
    is_initialized = true;
}

void TestInit()
{
    particleCapture.clear();
    for (int index = 0; index < NUM_PARTICLES; index++)
    {
        particle new_particle;
        new_particle.id = index;
        new_particle.x = index;
        new_particle.y = index;
        new_particle.theta = index;
        new_particle.weight = 1.0;
        particleArray[index] = new_particle;
        RobotState rs(new_particle.x, new_particle.y, new_particle.theta);
        particleCapture.push_back(rs);
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

double toRadians(int degree) { return degree * M_PI / 180; }

double constrainAngle(double x)
{
    x = fmod(x + 180, 360);
    if (x < 0)
        x += 360;
    return x - 180;
}

void motionModel(double u[])
{
    // change u to have just one distance and angle

    default_random_engine gen;
    uniform_real_distribution<double> distXY(0, 0.04);
    uniform_real_distribution<double> distTheta(0, 0.01);

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        double angle = constrainAngle(particleArray[i].theta + u[1]);
        double angleRad = toRadians(angle);

        double tempX =
            (u[0] * cos(angleRad) +
             particleArray[i].x);                                   // + distXY(gen); Noise is already in u, don't need to add
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
        particleCapture[i].x = particleArray[i].x;
        particleCapture[i].y = particleArray[i].y;
        particleCapture[i].theta = particleArray[i].theta;
    }
}

cell tupleToCell(tuple<double, double> convert)
{
    cell toReturn;
    toReturn.x = get<0>(convert);
    toReturn.y = get<1>(convert);
    return toReturn;
}

void measModel(LaserZ z)
{
    // For each particle:
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particle currParticle = particleArray[i];

        // for each laser to be checked as if it came from each particle
        for (int r = 0; r < z.getLaserCount(); r++)
        {
            double currLaserDist = z.getMeasurement(r);
            double currLaserAngle = z.getLaserAngle(r);

            // Make a robot (particle) state to pass in
            RobotState passInParticle;
            passInParticle.theta = currParticle.theta;
            passInParticle.x = currParticle.x;
            passInParticle.y = currParticle.y;

            // update particles own map here?
            particleArray[i].map.integrateLaserRangeRay(passInParticle, currLaserAngle,
                                                        currLaserDist, MAX_LASER_RANGE);

            double expectedRange =
                currParticle.map.findExpectedRange(passInParticle, currLaserAngle, MAX_LASER_RANGE);

            currParticle.weight += (LaserZ::laserRangeModel(currLaserDist, expectedRange) /
                                    21.0); // Possible way to update weight. Probably Ok to just use
                                           // addition, and not multiplication because we normalize.

            /**
            // SD of sensor is 3
            if (abs(expectedRange - currLaserDist) < 1) {
                // Good match, add weight
                // add 3
                currParticle.weight += 3;
            } else if (abs(expectedRange - currLaserDist) < 3) {
                // maybe match, add tiny weight
                // add 1
                currParticle.weight += 1;
            } else if (abs(expectedRange - currLaserDist) < 5) {
                // neutral match, do not change
            } else if (abs(expectedRange - currLaserDist) < 8) {
                // tiny bad match, take away tiny weight
                //-1
                currParticle.weight += -1;
            } else {
                // big bad match, take away big weight
                //-3
                currParticle.weight += -3;
            }
            */

        } // End of for each laser

    } // End of for each particle

    // Find Extremes
    double maxWeight = -1000000, minWeight = 100000;
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        // Find the new max weight
        if (particleArray[i].weight > maxWeight)
            maxWeight = particleArray[i].weight;
        // Find the mind weight
        if (particleArray[i].weight < minWeight)
            minWeight = particleArray[i].weight;
    }
    // Normalize
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        double temp = particleArray[i].weight - minWeight;
        temp = temp / (maxWeight - minWeight);
        particleArray[i].weight = temp;
    }
}

void resample()
{
    // copy the existing list of particles
    vector<particle> particleArrayCopy;
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particleArrayCopy.push_back(particleArray[i]);
    }

    // vector of weights of the particles
    vector<int> weights;
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        weights.push_back(particleArrayCopy[i].weight);
    }

    // Object of random number engine class that generate pseudo-random numbers
    // NOTE: http://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
    mt19937 gen;
    // Object for generating discrete disturbutions based on the weights vecotr
    discrete_distribution<int> weight_dist(weights.begin(), weights.end());

    // With the discrete distribution pick out particles according to their
    // weights. The higher the weight of the particle, the higher are the chances
    // of the particle being included multiple times.
    // Discrete_distribution is used here to pick particles with the appropriate
    // weights(i.e. which meet a threshold)
    // http://www.cplusplus.com/reference/random/discrete_distribution/
    // NOTE: Here is an example which helps with the understanding
    //      http://coliru.stacked-crooked.com/a/3c9005a4cc0ed9d6
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        // Append the particle to the new list
        // NOTE: Calling weights_dist with the generator returns the index of one
        //      of weights in the vector which was used to generate the distribution.
        particleArray[i] = particleArrayCopy[weight_dist(gen)];
        particleArray[i].id = i;
    }
}

int main()
{
    // Load Prior Occupancy Grid into memory
    History history;
    FileManager loader;
    vector<vector<double>> rawgrid =
        loader.loadGridMap("occupancy_grid.omap"); // This is the prior map before boxes were moved
                                                   // and the current experiment was ran
    vector<vector<int>> raw_static_grid = loader.loadStaticMap("permanence.pmap");
    std::tuple<double, double> xlim((double)0.0, (double)rawgrid[0].size());
    std::tuple<double, double> ylim((double)0.0, (double)rawgrid.size());
    // DynamicOccupancyGridMap ogrid = DynamicOccupancyGridMap(xlim, ylim, rawgrid, raw_static_grid,
    // 0.01, 0.05);
    // Load Controls and Measurements from experiment into memory
    loader.loadNumSteps("number_of_steps.data", &history);
    loader.loadSensorAngles("Angles.data");
    loader.loadMeasurements("Measurements (1).data", &history);
    loader.loadNoisyMeasurements("Measurements_Noisy (1).data", &history);
    loader.loadControls("Controls (1).data", &history);
    loader.loadNoisyControls("Controls_Noisy (1).data", &history);
    loader.loadState("State (1).data", &history);
    // Initialize Particle Filter -Dylan made this
    init(xlim, ylim, rawgrid, raw_static_grid, 0.01, 0.05);
    // Setup Plotting
    uint32_t capture_period = history.getNumSteps() / 16;
    // Loop through the data stream
    for (std::uint32_t t = 0; t < history.getNumSteps(); t++)
    {
        // Print particles
        if (t % capture_period == 0)
        {
            stringstream ss_fname;
            ss_fname << "Particles_step" << t << "_" << t / capture_period << ".part";
            loader.saveState(particleCapture, ss_fname.str());
        }
        // Extract data (get measurement z and control u at this time step t)
        LaserZ z = history.getNoisyMeasurement(t);
        ControlU u = history.getNoisyControl(t);
        // RobotState x_true = history.getState(t);   // This is the true value of the state. We can use the initial
                                                   // value, but nothing else, unless it's for testing.
        // LaserZ z_true = history.getMeasurement(t); // Do not use in PF Algorithm - This is the true
                                                   // value of the measurements. Just for testing.
        // ControlU u_true = history.getControl(t);   // Do not use in PF Algorithm - This is the true
                                                   // value of the controls. Just for testing.
        // Operate on data to run particle filter algorithm -
        double uarg[2] = {u.getDDist(), u.getDTheta()};
        motionModel(uarg);
        measModel(z);
        resample();
    }
    return 0;
}
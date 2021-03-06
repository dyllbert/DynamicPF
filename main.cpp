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

#define PRINT_OUT_OGRID 0
#define PRINT_OUT_NOISY_MEASUREMENTS 0

using namespace std;

// Some constant values here that can be played with
const int NUM_PARTICLES = 200;
// Positional uncertainty
const double sigma_pos[3] = {0.3, 0.3, 0.01};
const int X_MIN = 0;
const int X_MAX = 1024;
const int Y_MIN = 0;
const int Y_MAX = 768;
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

    //Add a bit of noise to each particle after resampleing
    default_random_engine noise;
    uniform_real_distribution<double> distXY(0, 0.04);
    uniform_real_distribution<double> distTheta(0, 0.01);
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particleArray[i].x += distXY(noise);
        particleArray[i].y += distXY(noise);
        particleArray[i].theta += distTheta(noise);
    }
}

int main()
{
    // Load Prior Occupancy Grid into memory
    std::cout << "Loading Occupancy Grid\n";
    History history;
    FileManager loader;
    vector<vector<double>> rawgrid =
        loader.loadGridMap("occupancy_grid.omap"); // This is the prior map before boxes were moved
                                                   // and the current experiment was ran
#if PRINT_OUT_OGRID
    int print_to_file = 0;
    stringstream ss_omap;
    ofstream ogrid_ascii("ogrid_ascii.txt", ios::out);
    if (ogrid_ascii)
    {
        print_to_file = 1;
    }
    for (uint32_t j = 0; j < rawgrid.size(); j++)
    {
        ss_omap << "|";
        if (print_to_file)
        {
            ogrid_ascii << "|";
        }
        for (uint32_t i = 0; i < rawgrid[0].size(); i++)
        {
            if (rawgrid[j][i] <= 0.5)
            {
                ss_omap << " ";
                if (print_to_file)
                {
                    ogrid_ascii << " ";
                }
            }
            else
            {
                ss_omap << ".";
                if (print_to_file)
                {
                    ogrid_ascii << ".";
                }
            }
        }
        ss_omap << "|\r\n";
        if (print_to_file)
        {
            ogrid_ascii << "|\r\n";
        }
    }
    ogrid_ascii.close();
    std::cout << ss_omap.str();
#endif
    std::cout << "Loading Static/Dynamic Map\n";
    vector<vector<int>> raw_static_grid = loader.loadStaticMap("permanence.pmap");
    std::cout << "Loading Number of steps\n";
    std::tuple<double, double> xlim((double)0.0, 1024.0); //(double)rawgrid[0].size());
    std::tuple<double, double> ylim((double)0.0, 768.0);  //(double)rawgrid.size());
    // DynamicOccupancyGridMap ogrid = DynamicOccupancyGridMap(xlim, ylim, rawgrid, raw_static_grid,
    // 0.01, 0.05);
    // Load Controls and Measurements from experiment into memory
    loader.loadNumSteps("number_of_steps.data", &history);
    std::cout << "Hard Coded Sensor Angles\n";
    //loader.loadSensorAngles("Angles.data");
    std::cout << "Loading Z\n";
    loader.loadMeasurements("Measurements (1).data", &history);
    std::cout << "Loading Z noisy\n";
    loader.loadNoisyMeasurements("Measurements_Noisy (1).data", &history);
#if PRINT_OUT_NOISY_MEASUREMENTS
    ofstream f_znoisy("noisy_z_ascii.txt", ios::out);
    if (history.getNumSteps() != history.getNoisyMeasurementHistory().size())
    {
        cout << "Wrong number of measurements! (" << history.getNoisyMeasurementHistory().size() << "). Should be " << history.getNumSteps() << "\n";
    }
    if (f_znoisy)
    {
        cout << "About to loop through " << history.getNumSteps() << " steps.\n";
        for (uint32_t i = 0; i < history.getNumSteps(); i++)
        {
            cout << i << endl;
            LaserZ print_z = history.getNoisyMeasurement(i);
            if (print_z.getMeasurements().size() != 21)
            {
                cout << "ERROR: Z only has " << print_z.getMeasurements().size() << " lasers!\n";
            }
            cout << "[" << i << "] About to loop through " << LaserZ::getLaserCount() << " lasers.\n";
            for (uint32_t j = 0; j < LaserZ::getLaserCount(); j++)
            {
                f_znoisy << "[" << j << "]" << print_z.getMeasurement(j) << " ";
                if (print_z.getMeasurement(j) < 0.0)
                {
                    cout << "Warning - Measurement of value " << print_z.getMeasurement(j) << "\n";
                }
            }
            f_znoisy << "\n";
        }
    }
    f_znoisy.close();
#endif
    std::cout << "Loading U\n";
    loader.loadControls("Controls (1).data", &history);
    std::cout << "Loading U Noisy\n";
    loader.loadNoisyControls("Controls_Noisy (1).data", &history);
    std::cout << "Loading State\n";
    loader.loadState("State (1).data", &history);
    // Initialize Particle Filter -Dylan made this
    std::cout << "Initializing Particle Filter\n";
    init(xlim, ylim, rawgrid, raw_static_grid, 0.01, 0.05);
    particleArray[0].x = history.getState(0).x; // Because we have limited compute power, we set 1 particle to the correct position.
    particleArray[0].y = history.getState(0).y;
    particleArray[0].theta = history.getState(0).theta;
    particleCapture[0].x = particleArray[0].x;
    particleCapture[0].y = particleArray[0].y;
    particleCapture[0].theta = particleArray[0].theta;
    std::cout << "Putting 1 particle at (" << particleArray[0].x << "," << particleArray[0].y << "," << particleArray[0].theta << ")\n";
    // Setup Plotting
    std::cout << "Setup Plotting\n";
    uint32_t capture_period = 1; // history.getNumSteps() / 16;
    // Loop through the data stream
    std::cout << "Beginning Loop\n";
    for (std::uint32_t t = 0; t < history.getNumSteps(); t++)
    {
        cout << "T is: " + to_string(t) << endl;
        // Print particles
        if (t % capture_period == 0)
        {
            stringstream ss_fname;
            ss_fname << "Particles_step" << t << "_" << t / capture_period << ".part";
            cout << "Saving " << particleCapture.size() << " current particles to " << ss_fname.str() << "\n";
            loader.saveState(particleCapture, ss_fname.str());
        }
        // Extract data (get measurement z and control u at this time step t)
        cout << "Getting z" << endl;
        LaserZ z = history.getNoisyMeasurement(t);
        cout << "Getting u" << endl;
        ControlU u = history.getNoisyControl(t);
        // RobotState x_true = history.getState(t);   // This is the true value of the state. We can use the initial
        // value, but nothing else, unless it's for testing.
        // LaserZ z_true = history.getMeasurement(t); // Do not use in PF Algorithm - This is the true
        // value of the measurements. Just for testing.
        // ControlU u_true = history.getControl(t);   // Do not use in PF Algorithm - This is the true
        // value of the controls. Just for testing.
        // Operate on data to run particle filter algorithm -
        double uarg[2] = {u.getDDist(), u.getDTheta()};
        cout << "Entering motion model" << endl;
        motionModel(uarg);
        cout << "Entering meas model" << endl;
        measModel(z);
        cout << "Entering reseample" << endl;
        resample();
    }
    return 0;
}
#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;

// Some constant values here that can be played with
const int NUM_PARTICLES = 20;
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

particle particleArray[NUM_PARTICLES];

void init(double x, double y, double theta, const double std[])
{
    default_random_engine gen;

    double std_x, std_y, std_theta;

    std_x = std[0];
    std_y = std[1];
    std_theta = std[2];

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

void printParticle(int index)
{
    cout << "ID: " + to_string(particleArray[index].id) << endl
         << "X: " + to_string(particleArray[index].x) << endl
         << "Y: " + to_string(particleArray[index].y) << endl
         << "Theta: " + to_string(particleArray[index].theta) << endl
         << "Weight: " + to_string(particleArray[index].weight) << endl;
}

int main()
{
    cout << "Test" << endl;
    init(1, 2, 30, sigma_pos);
    for(int i=0;i<NUM_PARTICLES;i++)
    {
        printParticle(i);
    }
}

/**
 * 
 * void init(double x, double y, double theta, const double std[])
{
    default_random_engine gen;

    double std_x, std_y, std_theta;

    std_x = std[0];
    std_y = std[1];
    std_theta = std[2];

    for (int index = 0; index < NUM_PARTICLES; index++)
    {

        normal_distribution<double> dist_x(x, std_x);
        normal_distribution<double> dist_y(y, std_y);
        normal_distribution<double> dist_theta(theta, std_theta);

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

void printParticle(int index)
{
    cout << "ID: " + to_string(particleArray[index].id) << endl
         << "X: " + to_string(particleArray[index].x) << endl
         << "Y: " + to_string(particleArray[index].y) << endl
         << "Theta: " + to_string(particleArray[index].theta) << endl
         << "Weight: " + to_string(particleArray[index].weight) << endl;
}

void prediction(double delta_t, double sigma_pos[], double velocity)
{
    default_random_engine gen;

    double std_x, std_y, std_theta;

    std_x = std_pos[0];
    std_y = std_pos[1];
    std_theta = std_pos[2];

    normal_distribution<double> noise_dist_x(0, std_x);
    normal_distribution<double> noise_dist_y(0, std_y);
    normal_distribution<double> noise_dist_theta(0, std_theta);
}
*/

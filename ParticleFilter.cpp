#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

using namespace std;

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

typedef struct MAP
{
    //NEED TO SETUP??????
} MAP;

typedef struct cell
{
    int x;
    int y;
} cell;

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

void motionModel(double u[])
{
    default_random_engine gen;
    uniform_real_distribution<double> distXY(0, 0.04);
    uniform_real_distribution<double> distTheta(0, 0.01);

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        double angle = toRadians(particleArray[i].theta);
        double tempX = (u[0] * cos(angle) - u[1] * sin(angle) + particleArray[i].x) + distXY(gen);
        double tempY = (u[0] * sin(angle) + u[1] * cos(angle) + particleArray[i].y) + distXY(gen);

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
        particleArray[i].theta = fmod(particleArray[i].theta + u[2] + distTheta(gen), 360.0);
    }
}

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

                

            }
        }
    }
}

int main()
{
    init(1, 2, 30, sigma_pos);
    cout << "Original Particles" << endl
         << "*******************" << endl;
    printAllParticles();
    double u[3] = {5.5, 5.5, -10};
    motionModel(u);
    cout << "Particles after one motion model" << endl
         << "***************" << endl;
    printAllParticles();
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

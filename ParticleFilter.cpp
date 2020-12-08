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
const int NUM_PARTICLES = 10;
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

void TestInit(double x, double y, double theta, const double std[])
{
    default_random_engine gen;

    double std_x, std_y, std_theta;

    std_x = std[0];
    std_y = std[1];
    std_theta = std[2];

    for (int index = 0; index < NUM_PARTICLES; index++)
    {
        particle new_particle;
        new_particle.id = index;
        new_particle.x = index;
        new_particle.y = index;
        new_particle.theta = index;
        new_particle.weight = rand()%10 - 5;
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
        particleArray[i].id = i;
    }
}

//Testing only
int main()
{
    TestInit(1, 2, 30, sigma_pos);
    cout << "Original Particles" << endl
         << "*******************" << endl;
    printAllParticles();
    double u[3] = {7, -10};
    motionModel(u);
    cout << "Particles after one motion model" << endl
         << "***************" << endl;
    printAllParticles();
    resample();
    cout << "Particles after resampling" << endl
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

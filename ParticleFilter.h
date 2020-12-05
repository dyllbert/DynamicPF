/*
* ParticleFilter.h
*/

#ifndef PARTICLEFILTER_H_
#define PARTICLEFILTER_H_

#include "pfHelpers.h"
#include <math.h>
#include <float.h>
#include <stdio.h>

using namespace std;

struct Particle {
    int id;
    double x;
    double y;
    double theta;
    double weight;
    vector<int> associations;
    vector<double> sense_x;
    vecotr<double> sense_y;
}

class ParticleFilter {
    // of particle to draw
    int numParticles;

    //init flag
    bool isInit;

    //vector of all the weights
    vecotr<double> weights;

    public:
    //current particles
    vector<Particle> currentParticles;

    //Constructor
    ParticleFilter() : numParticles(0), isInit(false) {}

    //Destructor
    ParticleFilter(){}

    //Init
    //Contains initial x,y,theat positions and an array of 3 dimensions for standard deviations
    void init(double x, double y, double theta, double std[]);
}
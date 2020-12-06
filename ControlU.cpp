#include "ControlU.h"

ControlU::ControlU(double d_dist, double d_theta) {
    this->d_dist = d_dist;
    this->d_theta = d_theta;
}

double ControlU::getDDist() {
    return d_dist;
}

double ControlU::getDTheta() {
    return d_theta;
}

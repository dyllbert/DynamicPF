#include "LaserZ.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double LaserZ::laser_angles[21] = {270,279,288,297,306,315,324,333,342,351,0,9,18,27,36,45,54,63,72,81,90};

LaserZ::LaserZ() {}

LaserZ::LaserZ(std::vector<double> z) {
    this->z = z;
}

void LaserZ::setMeasurements(std::vector<double> z) {
    this->z = z;
}

double LaserZ::getMeasurement(int laser_index) {
    return z[laser_index];
}

std::vector<double> LaserZ::getMeasurements() {
    return z;
}

double LaserZ::getLaserAngle(int laser_index) {
    return laser_angles[laser_index];
}

double LaserZ::getLaserCount() {
    return 21;//laser_angles.size();
}

void LaserZ::setLaserAngles(std::vector<double> angles) {
    // LaserZ::laser_angles = angles;
    if (angles.size() != 21) {
        std::cout << "ERROR: There should be 21 laser angles. " << angles.size() << " were provided.\n";
    }
    for (int i = 0; i < 21 && i < angles.size(); i++) {
        laser_angles[i] = angles[i];
    }
}

int LaserZ::isInitialized() {
    return 1;
}

double LaserZ::laserRangeModel(double z, double z_exp) {
    double a_norm = 0.7;
    double a_max = 0.3;
    if (z > LASER_MAX_RANGE) {return a_max;}
    else {return a_norm * 1/sqrt(2.0 * M_PI * LASER_STD_DEV) * exp(-(pow(z-z_exp, 2)) / (2 * LASER_STD_DEV));}
}


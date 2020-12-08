#include "LaserZ.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double LaserZ::laser_angles[21] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

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
    for (int i = 0; i < 21; i++) {
        laser_angles[i] = angles[i];
    }
}

int LaserZ::isInitialized() {
    return 1;
}

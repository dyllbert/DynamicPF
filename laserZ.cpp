#include "LaserZ.h"

LaserZ::LaserZ() {}

LaserZ::LaserZ(std::vector<double> z) {
    this->z = z;
}

void LaserZ::setMeasurements(std::vector<double> z) {
    this->z = z;
}

double LaserZ::getMeasurement(int laser_index) {
    return z[laser_index]
}

std::vector<double> LaserZ::getMeasurements() {
    return z;
}

double LaserZ::getLaserAngle(int laser_index) {
    return laser_angles[laser_index];
}

double LaserZ::getLaserCount() {
    return laser_angles.size();
}

void LaserZ::setLaserAngles(std::vector<double> angles) {
    LaserZ::laser_angles = angles
}

int LaserZ::isInitialized() {
    if (laser_angles.size() == 0) {
        return 0;
    }
    return 1;
}

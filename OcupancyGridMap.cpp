#include "OccupancyGridMap.h"
#include "OccupancyGrid.h"


OccupancyGridMap::OccupancyGridMap() {
    
}

OccupancyGridMap::OccupancyGridMap(double xlim[2], double ylim[2], vector<vector<double>> init_grid) {
    this->ogrid = OccupancyGrid(xlim, ylim, init_grid);
}

double OccupancyGridMap::laserRangeInverseSensorModel(int m_xi, int m_yi, RobotState x_t, double z_t) {
    //return the logodds update for a given map cell, robot state, and meas.
    //Will this need to be replaced with a not-inverse sensor model?
}

vector<int> OccupancyGridMap::findCellsToUpdateForRay(RobotState x_t, double z_theta_t, double max_range) {
    //Find the set of cells that lie along a ray given the robot pose, the angle of the ray with respect to the local x-axis of the robot, and the maximum range to be updated
}

void OccupancyGridMap::integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t) {
    //integrate a laser range measurement into the map
}

double OccupancyGridMap::findExpectedRange(RobotState x_t, double z_theta_t, double max_range) {
    //find the expected range that a sensor would return for a given pose in the map
}




// void OccupancyGridMap::updateMap(double z[2]){ //Update the map based on a [range,bearing] measurement, z


// }

// double* OccupancyGridMap::getZhat(double robpos[3], double angle) {

// }
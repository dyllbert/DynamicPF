#include "DynamicOccupancyGridMap.h"

DynamicOccupancyGridMap::DynamicOccupancyGridMap(double xlim[2], double ylim[2], vector<vector<double>> init_grid, vector<vector<bool>> st_mtx): OccupancyGridMap(xlim, ylim, init_grid) {
    this->st_mtx = st_mtx;
} //Do I need to include the parameterized base class in the header file as well?

void DynamicOccupancyGridMap::integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t){
        //integrate a laser range measurement into the map
        //
}



// void DynamicOccupancyGridMap::updateMap(double z[2]){ //Update the map based on a [range,bearing] measurement, z

// }

// vector<vector<int>> DynamicOccupancyGridMap::getSensorMap(double z[2]){ //Returns a matrix corresponding to the OCM in which each element contains either: 0=unobserved,1=miss,2=hit
// //Consider having a list of dynamic indices, and only updating those xand the cells that are in the range of the sensor
// }
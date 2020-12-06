#ifndef OCCUPANCY_GRID_MAP_H_
#define OCCUPANCY_GRID_MAP_H_

#include <vector>
#include <set>
#include <iostream>
#include <tuple>
#include "OccupancyGrid.h"
#include "RobotState.h"
using namespace std;

class OccupancyGridMap {
    private:
    
    protected:
        virtual double laserRangeInverseSensorModel(int m_xi, int m_yi, RobotState x_t, double z_t);
        set<tuple<int,int>> findCellsToUpdateForRay(RobotState x_t, double z_theta_t, double max_range);
        
    public:
        OccupancyGridMap();
        OccupancyGridMap(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid);
        OccupancyGridMap(OccupancyGrid ogrid);
        virtual void integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t, double max_range);
        double findExpectedRange(RobotState x_t, double z_theta_t, double max_range);
        OccupancyGrid ogrid;


        // void updateMap(double z[2]); //Update the OGM given a measurement
        // double* getZhat(double robpos[3]); //Get expected range and bearing
};

#endif

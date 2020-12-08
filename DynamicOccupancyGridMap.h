#pragma once
#include <vector>
#include <set>
#include <iostream>
#include <tuple>
#include "RobotState.h"
#include "DynamicOccupancyGrid.h"

using namespace std;

class DynamicOccupancyGridMap {
    private:
    
    protected:
        double laserRangeInverseSensorModel(int m_xi, int m_yi, RobotState x_t, double z_t);
        set<tuple<int,int>> findCellsToUpdateForRay(RobotState x_t, double z_theta_t, double max_range);
        
    public:
        DynamicOccupancyGridMap();
        DynamicOccupancyGridMap(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid, vector<vector<int>> st_mtx, double p_occ_from_free, double p_free_from_occ);
        void integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t, double max_range);
        double findExpectedRange(RobotState x_t, double z_theta_t, double max_range);
        DynamicOccupancyGrid ogrid;
};
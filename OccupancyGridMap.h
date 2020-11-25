#include <vector>
#include <iostream>
#include "OccupancyGrid.h"
#include "RobotState.h"
using namespace std;

class OccupancyGridMap {
    private:
    
    protected:
        double laserRangeInverseSensorModel(int m_xi, int m_yi, RobotState x_t, double z_t);
        vector<int> findCellsToUpdateForRay(RobotState x_t, double z_theta_t, double max_range);
        
    public:
        OccupancyGridMap();
        OccupancyGridMap(double xlim[2], double ylim[2], vector<vector<double>> init_grid);
        virtual void integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t);
        double findExpectedRange(RobotState x_t, double z_theta_t, double max_range);
        OccupancyGrid ogrid;


        // void updateMap(double z[2]); //Update the OGM given a measurement
        // double* getZhat(double robpos[3]); //Get expected range and bearing
};
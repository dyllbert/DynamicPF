#include <vector>
#include "OccupancyGridMap.h"
using namespace std;

class DynamicOccupancyGridMap: public OccupancyGridMap {
    private:

    protected:
        vector<vector<bool>> st_mtx; //matrix indicating whether each cell is static or not
    public:
        DynamicOccupancyGridMap();
        DynamicOccupancyGridMap(double xlim[2], double ylim[2], vector<vector<double>> init_grid, vector<vector<bool>> st_mtx);
        void integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t);
};
#include "DynamicOccupancyGridMap.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DynamicOccupancyGridMap::DynamicOccupancyGridMap() {
}

DynamicOccupancyGridMap::DynamicOccupancyGridMap(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid, vector<vector<int>> st_mtx, double p_occ_from_free, double p_free_from_occ) {
    this->ogrid = DynamicOccupancyGrid(xlim, ylim, init_grid, st_mtx, p_occ_from_free, p_free_from_occ);
    set<tuple<int,int>> findCellsToUpdateForRay(RobotState x_t, double z_theta_t, double max_range);
}

double DynamicOccupancyGridMap::laserRangeInverseSensorModel(int m_xi, int m_yi, RobotState x_t, double z_t) {
    //return the logodds update for a given map cell, robot state, and meas.
    //Will this need to be replaced with a not-inverse sensor model?
    double x_diff = x_t.x-m_xi;
    double y_diff = x_t.y-m_yi;
    double dist = sqrt(pow(x_diff,2)+pow(y_diff,2));

    double p_occ = 0.7;
    double p_prior = 0.5;
    double p_free = 0.3;

    double rx = this->ogrid.resolution_x;
    double ry = this->ogrid.resolution_y;
    double r = sqrt(pow(rx*x_diff/dist,2)+ pow(ry*y_diff/dist,2)); //something like that
    if(dist < (z_t - r/2)) {
        return log(p_free/(1-p_free));
    } else if (dist < (z_t + r/2)) {
        return log(p_occ/(1-p_occ));
    } else {
        return log(p_prior/(1-p_prior));
    }
}

set<tuple<int,int>> DynamicOccupancyGridMap::findCellsToUpdateForRay(RobotState x_t, double z_theta_t, double max_range) {
    //Find the set of cells that lie along a ray given the robot pose, the angle of the ray with respect to the local x-axis of the robot, and the maximum range to be updated
    double ray_angle = z_theta_t + x_t.theta;
    ray_angle = (ray_angle*M_PI)/180;
    double resolution = sqrt(pow(this->ogrid.resolution_x*cos(ray_angle),2) + pow(this->ogrid.resolution_y*sin(ray_angle),2)); //something like that   
    double step = resolution/4;
    double x_end = max_range*cos(ray_angle) + x_t.x;
    double y_end = max_range*sin(ray_angle) + x_t.y;

    int num_steps = max_range/step;
    double lam;
    double x;
    double y;
    set<tuple<int,int>> cell_index_list;
    //cout << "pose x:" << x_t.x << "pose y:" << x_t.y << " ray angle: " << ray_angle << "\n";
    //cout << "end x:" << x_end << "end y:" << y_end << "\n";
    for (int i = 0; i < num_steps; i++) {
        lam = i/(double)num_steps;
        x = x_t.x + (x_end - x_t.x)*lam;
        y = x_t.y + (y_end - x_t.y)*lam;
        //cout << "x:" << x << " y:" << y << "\n";
        if ((x < get<0>(this->ogrid.xlim) + resolution) || (x > get<1>(this->ogrid.xlim) - resolution) || (y < get<0>(this->ogrid.ylim) + resolution) || (y > get<1>(this->ogrid.ylim) - resolution)) {
            break;
        }
        try {
            tuple<int,int> index = this->ogrid.getCellIndex(x,y);
            if (this->ogrid.st_mtx[get<1>(index)][get<0>(index)] == 0) { //If dynamic. Make sure this indexing is correct for the passed in st_mtx
                cell_index_list.insert(index); 
            }
        } catch (char* e) {}
    }
    return cell_index_list;
}

double DynamicOccupancyGridMap::findExpectedRange(RobotState x_t, double z_theta_t, double max_range) {
    //find the expected range that a sensor would return for a given pose in the map
    double ray_angle = z_theta_t + x_t.theta;
    ray_angle = (ray_angle*M_PI)/180;
    double resolution = sqrt(pow(this->ogrid.resolution_x*cos(ray_angle),2) + pow(this->ogrid.resolution_y*sin(ray_angle),2)); //something like that   
    double step = resolution/4;
    double x_end = max_range*cos(ray_angle) + x_t.x;
    double y_end = max_range*sin(ray_angle) + x_t.y;
    int num_steps = max_range/step;
    double expected_range = max_range;
    double lam;
    double x;
    double y;

    for (int i = 0; i < num_steps; i++) {
        lam = i/num_steps;
        x = x_t.x + (x_end - x_t.x)*lam;
        y = x_t.y + (y_end - x_t.y)*lam;
        if ((x < get<0>(this->ogrid.xlim) + resolution) || (x > get<1>(this->ogrid.xlim) - resolution) || (y < get<0>(this->ogrid.ylim) + resolution) || (y > get<1>(this->ogrid.ylim) - resolution)) {
            break;
        }
        try {
            tuple<int,int> index = this->ogrid.getCellIndex(x,y);
            if (this->ogrid.grid[get<1>(index)][get<0>(index)] >= 0) {
                expected_range = sqrt(pow(x-x_t.x,2)+pow(y-x_t.y,2));
                return expected_range;
            }
        } catch (char* e) {}
    }
    return expected_range;
}

void DynamicOccupancyGridMap::integrateLaserRangeRay(RobotState x_t, double z_theta_t, double z_t, double max_range){
    //integrate a laser range measurement into the map
    this->ogrid.updateDynamicCells();
    set<tuple<int,int>> cells_to_update = this->findCellsToUpdateForRay(x_t,z_theta_t,max_range);   
    for (tuple<int,int> idx: cells_to_update) {
        tuple<double,double> cell_pos = this->ogrid.getCellCenter(idx);
        double l = this->laserRangeInverseSensorModel(get<0>(cell_pos),get<1>(cell_pos),x_t,z_t);
        this->ogrid.updateCellWithMeasLogodds(idx,l);
    }
}






// void OccupancyGridMap::updateMap(double z[2]){ //Update the map based on a [range,bearing] measurement, z


// }

// double* OccupancyGridMap::getZhat(double robpos[3], double angle) {

// }
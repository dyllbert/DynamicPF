#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <tuple>
#include <fstream>

using namespace std;

class DynamicOccupancyGrid {
    private:

    protected:
        vector<double> xcoords;
        vector<double> ycoords;
        
    public:
        DynamicOccupancyGrid();
        DynamicOccupancyGrid(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid, vector<vector<int>> st_mtx, double p_occ_from_free, double p_free_from_occ);
        double resolution_x;
        double resolution_y;
        //we cold replace the following with 2d arrays which might make the code run faster, if we know the size of the map in advance
        vector<vector<double>> grid; //matrix containing the probability that each cell is occupied
        tuple<int,int> getCellIndex(double x_pos, double y_pos);
        tuple<double,double> getCellCenter(tuple<int,int> index);
        void updateCellWithMeasLogodds(tuple<int,int> index, double l);
        void plotGrid();
        void exportGrid(string outfilename);
        tuple<double,double> xlim;
        tuple<double,double> ylim;
        int xdim;
        int ydim;
        double prior_lg_odd;

        //Stuff specific to dynamic
        vector<vector<int>> st_mtx; //matrix indicating whether each cell is static (1) or not (0)
        vector<vector<int>> steps_since_last_update;
        double p_occ_from_free;
        double p_free_from_occ;
        double p_occ_from_occ;
        double p_free_from_free;
        void updateDynamicCells();
};
#include <vector>
#include <iostream>
//#include "matplotlibcpp.h"
using namespace std;

class OccupancyGrid {
    private:

    protected:
        double* xlim;
        double* ylim;
        int xdim;
        int ydim;
        double prior = 0.5;
        vector<double> xcoords;
        vector<double> ycoords;
        //we cold replace the following with 2d arrays which might make the code run faster, if we know the size of the map in advance
        vector<vector<double>> grid; //matrix containing the probability that each cell is occupied
        int* getCellIndex(double x_pos, double y_pos);
        double* getCellCenter(int index[2]);
        void updateCellWithMeasLogodds(int index[2], double l);
        void plotGrid();
        
    public:
        OccupancyGrid();
        ~OccupancyGrid();
        OccupancyGrid(double xlim[2], double ylim[2], vector<vector<double>> init_grid);
        double resolution_x;
        double resolution_y;
};
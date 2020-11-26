#include <vector>
#include <iostream>
#include <cmath>
#include <tuple>

using namespace std;

class OccupancyGrid {
    private:

    protected:
        tuple<double,double> xlim;
        tuple<double,double> ylim;
        int xdim;
        int ydim;
        double prior_lg_odd;
        vector<double> xcoords;
        vector<double> ycoords;
        
    public:
        OccupancyGrid();
        ~OccupancyGrid();
        OccupancyGrid(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid);
        double resolution_x;
        double resolution_y;
        //we cold replace the following with 2d arrays which might make the code run faster, if we know the size of the map in advance
        vector<vector<double>> grid; //matrix containing the probability that each cell is occupied
        tuple<int,int> getCellIndex(double x_pos, double y_pos);
        tuple<double,double> getCellCenter(tuple<int,int> index);
        void updateCellWithMeasLogodds(tuple<int,int> index, double l);
        void plotGrid();
};
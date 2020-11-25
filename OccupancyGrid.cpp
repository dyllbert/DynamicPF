#include "OccupancyGrid.h"

OccupancyGrid::OccupancyGrid() {
}

OccupancyGrid::~OccupancyGrid() {
    delete[] xlim;
    delete[] ylim;
}

OccupancyGrid::OccupancyGrid(double xlim[2], double ylim[2], vector<vector<double>> init_grid) {
    int xdim = init_grid.size(); //if we decide on a constant size for our grid, these can be changed
    int ydim = init_grid[1].size();
    int xwidth = xlim[1]-xlim[0];
    int resolution = xwidth/xdim;
    int ywidth = ylim[1]-ylim[0];
    int resolution = ywidth/ydim;

    this->grid = init_grid;
    this->resolution_x = resolution_x;
    this->resolution_y = resolution_y;
    this->xlim = xlim;
    this->ylim = ylim;
    this->xdim = xdim;
    this->ydim = ydim;
    this->xcoords;
    this->ycoords;
}

int* OccupancyGrid::getCellIndex(double x_pos, double y_pos) {
    //Return the [x,y] index of a cell given an x position and a y position
}

double* OccupancyGrid::getCellCenter(int index[2]) {
    //Return the [x,y] position of the center of the cells of given index
}

void OccupancyGrid::updateCellWithMeasLogodds(int index[2], double l) {
    //update a cell by adding the log odds value, l
}

void OccupancyGrid::plotGrid() {
    //plots the grid to the screen. May require we download matplotlibcpp
    //https://github.com/lava/matplotlib-cpp.git
}


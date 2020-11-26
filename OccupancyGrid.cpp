#include "OccupancyGrid.h"

OccupancyGrid::OccupancyGrid() {
}

OccupancyGrid::OccupancyGrid(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid) {
    int xdim = init_grid.size(); //if we decide on a constant size for our grid, these can be changed
    int ydim = init_grid[1].size();
    int xwidth = get<1>(xlim)-get<0>(xlim);
    int resolution_x = xwidth/xdim;
    int ywidth = get<1>(ylim)-get<0>(ylim);
    int resolution_y = ywidth/ydim;
    double prior = 0.5;

    this->grid = init_grid;
    this->resolution_x = resolution_x;
    this->resolution_y = resolution_y;
    this->xlim = xlim;
    this->ylim = ylim;
    this->xdim = xdim;
    this->ydim = ydim;
    this->xcoords;
    this->ycoords;
    this->prior_lg_odd = log(prior/(1-prior));
}

tuple<int,int> OccupancyGrid::getCellIndex(double x_pos, double y_pos) {
    //Return the [x,y] index of a cell given an x position and a y position
    if (x_pos < get<0>(this->xlim) || x_pos >= get<1>(this->xlim)) {
        throw invalid_argument("X position is outside grid.");
    }
    if (y_pos < get<0>(this->ylim) || y_pos >= get<1>(this->ylim) {
        throw invalid_argument("Y position is outside grid.");
    }

    int x = round((x_pos - this->get<0>(this->xlim))/this->resolution_x);
    int y = round((y_pos - this->get<0>(this->ylim))/this->resolution_y);

    tuple<int,int> plot_pos = make_tuple(x,y);
    return plot_pos;
}

tuple<double,double> OccupancyGrid::getCellCenter(tuple<int,int> index) {
    //Return the [x,y] position of the center of the cells of given index
    if(get<0>(index) < 0 || get<0>(index) >= this->xdim) {
        throw invalid_argument("X index invalid");
    }
    if(get<1>(index)  < 0 || get<1>(index) >= this->ydim) {
        throw invalid_argument("Y index invalid");
    }

    double x = this->xcoords[get<0>(index)] + this->resolution_x/2;
    double y = this->ycoords[get<1>(index)] + this->resolution_y/2;

    tuple<double,double> cell_center = make_tuple(x,y);
    return cell_center;
}

void OccupancyGrid::updateCellWithMeasLogodds(tuple<int,int> index, double l) {
    //update a cell by adding the log odds value, l
    if(get<0>(index) < 0 || get<0>(index) >= this->xdim) {
        throw invalid_argument("X index invalid");
    }
    if(get<1>(index)  < 0 || get<1>(index) >= this->ydim) {
        throw invalid_argument("Y index invalid");
    }
    //FOR OUR MAP, IS INDEX[1] FIRST, OR INDEX[0]?
    this->grid[get<1>(index)][get<0>(index)] += l;
    this->grid[get<1>(index)][get<0>(index)] -= this->prior_lg_odd;
}

void OccupancyGrid::plotGrid() {
    /*plots the grid to the screen. May require we download a plotting library, such as
    https://github.com/lava/matplotlib-cpp.git*/
}


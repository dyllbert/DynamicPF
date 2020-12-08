#include "DynamicOccupancyGrid.h"
#include <string>
#include <iostream>

DynamicOccupancyGrid::DynamicOccupancyGrid() {
}

DynamicOccupancyGrid::DynamicOccupancyGrid(tuple<double,double> xlim, tuple<double,double> ylim, vector<vector<double>> init_grid, vector<vector<int>> st_mtx, double p_occ_from_free, double p_free_from_occ) {
    /*
    xlim and ylim are the physical limits of the map. 
    init_grid is the initial grid with with each cell representing a probability that that cell is occupied in the map
    */
    
    if (init_grid.empty()) {
        throw invalid_argument("Initial grid cannot be empty");
    }
    
    int xdim = init_grid[1].size(); //if we decide on a constant size for our grid, these can be changed
    double xwidth = get<1>(xlim)-get<0>(xlim);
    double resolution_x = xwidth/xdim;
    int ydim = init_grid.size();
    double ywidth = get<1>(ylim)-get<0>(ylim);
    double resolution_y = ywidth/ydim;
    double prior = 0.5;

    double cell;
    vector<vector<double>> log_grid;
    for (int i = 0; i < init_grid.size(); i++) {
        vector<double> temp;
        log_grid.push_back(temp);
        for (int j = 0; j < init_grid[i].size(); j++) {
            cell = init_grid[i][j];
            if (cell > 1 || cell < 0) {
                throw invalid_argument("Each cell in the initial grid must contain a probability between 0 and 1");
            }
            if (cell == 1) {
                if (log_grid[i][j] == 0) {
                    log_grid[i].push_back(2.2);
                } else {
                    log_grid[i].push_back(10);
                }
            } else if (cell == 0) {
                if (log_grid[i][j] == 0) {
                    log_grid[i].push_back(-2.2);
                } else {
                    log_grid[i].push_back(-10);
                }
            } else {
                log_grid[i].push_back(log(cell/(1-cell)));
            }
        }
    }
    this->grid = log_grid;
    this->resolution_x = resolution_x;
    this->resolution_y = resolution_y;
    this->xlim = xlim;
    this->ylim = ylim;
    this->xdim = xdim;
    this->ydim = ydim;
    this->prior_lg_odd = log(prior/(1-prior));
    this->st_mtx = st_mtx;
    this->p_occ_from_free = p_occ_from_free;
    this->p_free_from_occ = p_free_from_occ;
    this->p_occ_from_occ = 1-p_free_from_occ;
    this->p_free_from_free = 1 - p_occ_from_free;

    for(int i = 0; i <= xdim; i++) {
        this->xcoords.push_back(get<0>(xlim) + i*resolution_x);
    }
    for(int i = 0; i <= ydim; i++) {
        this->ycoords.push_back(get<0>(ylim) + i*resolution_y);
    }
}

tuple<int,int> DynamicOccupancyGrid::getCellIndex(double x_pos, double y_pos) {
    //Return the [x,y] index of a cell given an x position and a y position
    if (x_pos < get<0>(this->xlim) || x_pos >= get<1>(this->xlim)) {
        throw invalid_argument("X position is outside grid.");
    }
    if (y_pos < get<0>(this->ylim) || y_pos >= get<1>(this->ylim)) {
        throw invalid_argument("Y position is outside grid.");
    }

    int x = round((x_pos - get<0>(this->xlim))/this->resolution_x);
    int y = round((y_pos - get<0>(this->ylim))/this->resolution_y);
    //cout << "x idx:" << x << " y idx:" << y << "\n";
    tuple<int,int> plot_pos = make_tuple(x,y);
    return plot_pos;
}

tuple<double,double> DynamicOccupancyGrid::getCellCenter(tuple<int,int> index) {
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

void DynamicOccupancyGrid::updateCellWithMeasLogodds(tuple<int,int> index, double l) {
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

void DynamicOccupancyGrid::updateDynamicCells() {
    /*
    Updates cells that are dynamic given the probaility that they are to change
   */
    double p_sum;
    double l_sum; //log odds
    double p_recurs; // recursive probability
    for (int i = 0; i < this->grid.size(); i++) {
        for (int j = 0; j < this->grid[i].size(); j++) {
            if (this->st_mtx[i][j] == 0) { //If dynamic
                p_recurs = 1-1/(1+exp(this->grid[i][j]));
                //if (p_recurs < 0.5) { //if probably free in the previous time step
                //    p_sum =  p_occ_from_free*p_recurs + p_occ_from_occ*(1 - p_recurs);
                //} else { //if probably occupied in the previous time step
                    p_sum = p_occ_from_occ*p_recurs + p_occ_from_free*(1 - p_recurs);
                //}
                l_sum = log(p_sum/(1-p_sum));
                this->grid[i][j] = l_sum;
            }
        }
    }
}

void DynamicOccupancyGrid::plotGrid() {
    /*plots the grid to the screen. May require we download a plotting library, such as
    https://github.com/lava/matplotlib-cpp.git*/
}

void DynamicOccupancyGrid::exportGrid(string outfilename) {
    /*exports the grid to a csv file
    You can then plot this grid in MATLAB using the following code:
        T = readtable('OGM.csv');
        occ_map = T{:,:};
        figure;
        imagesc(occ_map.');
    */
    double grid_prob;
    cout << "exporting grid map to " + outfilename << "\n";
    ofstream exportFile(outfilename);
    for(int i = 0; i < this->grid[0].size();++i) {
        for(int j = 0; j < this->grid.size();++j) {
            grid_prob = 1-1/(1+exp(this->grid[j][i]));
            exportFile << grid_prob;
            if(j != this->grid.size() - 1) {
                exportFile << ",";
            }
        }
        exportFile << "\n";
    }
    exportFile.close();
}



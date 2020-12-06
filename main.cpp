#include "FileLoader.h"
#include "RobotState.h"
#include "History.h"
#include "DynamicOccupancyGridMap.h"

int main(int argc, char* argv[]) {
    // Load Prior Occupancy Grid into memory
    History history;
    FileLoader loader;
    loader.loadGridMap("occupancy_grid.omap"); // This is the prior map before boxes were moved and the current experiment was ran
    // Load Controls and Measurements from experiment into memory
    loader.loadSensorAngles("Angles.data");
    loader.loadMeasurements("Measurements (1).data", "number_of_steps.data", &history);
    loader.loadNoisyMeasurements("Measurements_Noisy (1).data", "number_of_steps.data", &history);
    loader.loadControls("Controls (1).data", "number_of_steps.data", &history);
    loader.loadNoisyControls("Controls_Noisy (1).data", "number_of_steps.data", &history);
    loader.loadState("State (1).data", "number_of_steps.data", &history);
    // Initialize Particle Filter
    // Setup Plotting
    // Loop through the data stream
    //  Extract data (get measurement z and control u at this time step t)
    //  Operate on data to run particle filter algorithm
    //  Use extended sensor model for mapping problem
    //  Plot
    return 0;
}
#include "FileLoader.h"
#include <fstream>
#include <stdio.h>
#include "LaserZ.h"
#include "ControlU.h"

vector<vector<double>> FileLoader::loadGridMap(std::string fileName) {
    // Open file for gridmap
    std::ifstream f(fileName, std::ios::in | std::ios::binary);
    // Extract header string that defines gridmap size
    char buffer[200];
    int done_extracting_header = 0;
    std::uint32_t i = 0;
    while (!done_extracting_header) {
        f.read(buffer + i, 1); 
        if (buffer[i] == '\0' || i >= 199) {
            done_extracting_header = 1;
        }
        i++;
    }
    // Extract values from header string
    char byte[1];
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t num_pixels;
    sscanf(buffer, "%u,%u", &width, &height);
    // Initialize occupancy grid, and fill with data
    vector<vector<double>> ogrid(height, vector<double>(width));
    for (std::uint32_t j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            f.read(byte, 1);
            if (byte[0] == 0) {
                ogrid[j][i] = 0.0;
            }
            else {
                ogrid[j][i] = 1.0;
            }
        }
    }
    f.close();
    return ogrid;
}

std::uint32_t loadNumSteps(std::string fname) {
    // Load number of steps from file
    std::ifstream f_steps(fname, std::ios::in);
    std::uint32_t num_steps = 0;
    f_steps >> num_steps;
    f_steps.close();
    return num_steps;
}

void FileLoader::loadMeasurements(std::string measurements_fname, std::string num_steps_fname, History *history) {
    if (!LaserZ::isInitialized()) {
        throw "ERROR: loadSensorAngles() must be called before loadMeasurements()!";
    }
    // Load number of steps from file
    std::uint32_t num_steps = loadNumSteps(num_steps_fname);
    // Read measurement file for correct number of steps
    std::vector<LaserZ> measurements;
    std::ifstream f_meas(measurements_fname, std::ios::in | std::ios::binary);
    for (std::uint32_t i = 0; i < num_steps; i++) {
        std::vector<double> meas(LaserZ::getLaserCount(), 0.0);
        for (std::uint16_t j = 0; j < LaserZ::getLaserCount(); j++) {
            std::uint16_t measurement;
            f_meas.read(reinterpret_cast<char *>(&measurement), sizeof(measurement));
            meas[j] = (double) measurement;
        }
        LaserZ z(meas);
        measurements.push_back(z);
    }
    f_meas.close();
    history->loadMeasurementHistory(measurements);
}

void FileLoader::loadNoisyMeasurements(std::string filename, std::string num_steps_fname, History *history) {
    if (!LaserZ::isInitialized()) {
        throw "ERROR: loadSensorAngles() must be called before loadNoisyMeasurements()!";
    }
    // Load number of steps from file
    std::uint32_t num_steps = loadNumSteps(num_steps_fname);
    // Read measurement file for correct number of steps
    std::vector<LaserZ> measurements;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    for (std::uint32_t i = 0; i < num_steps; i++) {
        std::vector<double> meas(LaserZ::getLaserCount(), 0.0);
        for (std::uint16_t j = 0; j < LaserZ::getLaserCount(); j++) {
            float measurement;
            f.read(reinterpret_cast<char *>(&measurement), sizeof(measurement));
            meas[j] = (double) measurement;
        }
        LaserZ z(meas);
        measurements.push_back(z);
    }
    f.close();
    history->loadMeasurementHistory(measurements);
}

void FileLoader::loadControls(std::string filename, std::string num_steps_fname, History *history) {
    std::uint32_t num_steps = loadNumSteps(num_steps_fname);
    std::vector<ControlU> controls;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    for (std::uint32_t i = 0; i < num_steps; i++) {
        char ddist;
        char dtheta;
        f.read(&ddist, sizeof(ddist));
        f.read(&dtheta, sizeof(dtheta));
        ControlU u(ddist, dtheta);
        controls.push_back(u);
    }
    f.close();
    history->loadControlHistory(controls);
}

void FileLoader::loadNoisyControls(std::string filename, std::string num_steps_fname, History *history) {
    std::uint32_t num_steps = loadNumSteps(num_steps_fname);
    std::vector<ControlU> controls;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    for (std::uint32_t i = 0; i < num_steps; i++) {
        float ddist;
        float dtheta;
        f.read(reinterpret_cast<char *>(&ddist), sizeof(ddist));
        f.read(reinterpret_cast<char *>(&dtheta), sizeof(dtheta));
        ControlU u(ddist, dtheta);
        controls.push_back(u);
    }
    f.close();
    history->loadControlHistory(controls);
}

void FileLoader::loadState(std::string filename, std::string num_steps_fname, History *history) {
    std::uint32_t num_steps = loadNumSteps(num_steps_fname);
    std::vector<RobotState> states;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    for (std::uint32_t i = 0; i < num_steps; i++) {
        std::uint32_t x;
        std::uint32_t y;
        std::uint32_t direction;
        f.read(reinterpret_cast<char *>(&x), sizeof(x));
        f.read(reinterpret_cast<char *>(&y), sizeof(y));
        f.read(reinterpret_cast<char *>(&direction), sizeof(direction));
        RobotState rs((double) x, (double) y, (double) direction);
        states.push_back(rs);
    }
    f.close();
    history->loadRobotStates(states);
}

void FileLoader::loadSensorAngles(std::string filename) {
    // std::vector<double> laser_angles;
    // std::uint16_t angle;
    // while ()
}

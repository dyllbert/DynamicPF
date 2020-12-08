#include "FileManager.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "LaserZ.h"
#include "ControlU.h"
#include <cstdio>
#include <iostream>

std::vector<std::vector<double>> FileManager::loadGridMap(std::string fileName) {
    // Open file for gridmap
    std::ifstream f(fileName, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open grid map file.\n";}
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
                ogrid[j][i] = 0.01;
            }
            else {
                ogrid[j][i] = 1.0;
            }
        }
    }
    f.close();
    return ogrid;
}

std::vector<std::vector<int>> FileManager::loadStaticMap(std::string fileName) {
    // Open file for gridmap
    std::ifstream f(fileName, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open static/dynamic map file.\n";}
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
    vector<vector<int>> sgrid(height, vector<int>(width));
    for (std::uint32_t j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            f.read(byte, 1);
            sgrid[j][i] = (int)(byte[0]);
        }
    }
    f.close();
    return sgrid;
}

std::uint32_t FileManager::loadNumSteps(std::string filename, History *history) {
    // Load number of steps from file
    std::ifstream f(filename, std::ios::in);
    if (!f) {std::cout << "ERROR: Unable to open num_steps file.\n";}
    std::uint32_t num_steps = 0;
    f >> num_steps;
    f.close();
    history->setNumSteps(num_steps);
    return num_steps;
}

void FileManager::loadMeasurements(std::string filename, History *history) {
    if (!LaserZ::isInitialized()) {
        throw "ERROR: loadSensorAngles() must be called before loadMeasurements()!";
    }
    if (history->getNumSteps() == 0) {
        throw "ERROR: loadNumSteps() must be called before loadMeasurements()!";
    }
    // Load number of steps from file
    std::uint32_t num_steps = history->getNumSteps();
    // Read measurement file for correct number of steps
    std::vector<LaserZ> measurements;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open measurements file.\n";}
    for (std::uint32_t i = 0; i < num_steps; i++) {
        std::vector<double> meas(LaserZ::getLaserCount(), 0.0);
        for (std::uint16_t j = 0; j < LaserZ::getLaserCount(); j++) {
            std::uint16_t measurement;
            f.read(reinterpret_cast<char *>(&measurement), sizeof(measurement));
            meas[j] = (double) measurement;
        }
        LaserZ z(meas);
        measurements.push_back(z);
    }
    f.close();
    history->setMeasurementHistory(measurements);
}

void FileManager::loadNoisyMeasurements(std::string filename, History *history) {
    if (!LaserZ::isInitialized()) {
        throw "ERROR: loadSensorAngles() must be called before loadNoisyMeasurements()!";
    }
    if (history->getNumSteps() == 0) {
        throw "ERROR: loadNumSteps() must be called before loadNoisyMeasurements()!";
    }
    // Load number of steps from file
    std::uint32_t num_steps = history->getNumSteps();
    std::cout << "FileManager: About to load " << num_steps << " noisy measurements from storage.\n";
    // Read measurement file for correct number of steps
    std::vector<LaserZ> measurements;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open noisy measurements file.\n";}
    else {std::cout << "Successfully opened " << filename << "\n";}
    for (std::uint32_t i = 0; i < num_steps; i++) {
        std::vector<double> meas;//(LaserZ::getLaserCount(), 0.0);
        for (std::uint16_t j = 0; j < LaserZ::getLaserCount(); j++) {
            float measurement;
            try {
                f.read(reinterpret_cast<char *>(&measurement), sizeof(measurement));
                meas.push_back((double) measurement); //[j] = (double) measurement;
            }
            catch (exception e) {
                std::cout << "ERROR: Could not read value for z#" << i << ", laser#" << j << "\n";
            }
        }
        LaserZ z(meas);
        if (z.getMeasurements().size() != 21) {
            std::cout << "ERROR: Initialization of z did not preserve values!\n";
        }
        measurements.push_back(z);
    }
    f.close();
    history->setMeasurementHistory(measurements);
}

void FileManager::loadControls(std::string filename, History *history) {
    if (history->getNumSteps() == 0) {
        throw "ERROR: loadNumSteps() must be called before loadControls()!";
    }
    std::uint32_t num_steps = history->getNumSteps();
    std::vector<ControlU> controls;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open controls file.\n";}
    for (std::uint32_t i = 0; i < num_steps; i++) {
        char ddist;
        char dtheta;
        f.read(&ddist, sizeof(ddist));
        f.read(&dtheta, sizeof(dtheta));
        ControlU u(ddist, dtheta);
        controls.push_back(u);
    }
    f.close();
    history->setControlHistory(controls);
}

void FileManager::loadNoisyControls(std::string filename, History *history) {
    if (history->getNumSteps() == 0) {
        throw "ERROR: loadNumSteps() must be called before loadNoisyControls()!";
    }
    std::uint32_t num_steps = history->getNumSteps();
    std::vector<ControlU> controls;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open noisy controls file.\n";}
    for (std::uint32_t i = 0; i < num_steps; i++) {
        float ddist;
        float dtheta;
        f.read(reinterpret_cast<char *>(&ddist), sizeof(ddist));
        f.read(reinterpret_cast<char *>(&dtheta), sizeof(dtheta));
        ControlU u(ddist, dtheta);
        controls.push_back(u);
    }
    f.close();
    history->setControlHistory(controls);
}

void FileManager::loadState(std::string filename, History *history) {
    if (history->getNumSteps() == 0) {
        throw "ERROR: loadNumSteps() must be called before loadState()!";
    }
    std::uint32_t num_steps = history->getNumSteps();
    std::vector<RobotState> states;
    std::ifstream f(filename, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open state file.\n";}
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
    history->setRobotStates(states);
}

void FileManager::loadSensorAngles(std::string filename) {
    std::vector<double> laser_angles;
    std::uint16_t angle;
    ifstream f(filename, std::ios::in | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open angles file.\n";}
    // f.seekg(std::ios::end);
    // std::uint64_t num_angles = ((std::uint64_t) f.tellg()) / 2;
    // f.seekg(ifstream::beg);
    std::uint64_t num_angles = 21;
    for (std::uint64_t i = 0; i < num_angles; i++) {
        std::uint16_t angle;
        f.read(reinterpret_cast<char *>(&angle), sizeof(angle));
        laser_angles.push_back((double) angle);
    }
    f.close();
    LaserZ::setLaserAngles(laser_angles);
}

bool saveState(vector<RobotState> state_snapshot, std::string filename) {
    std::ofstream f(filename, std::ios::out | std::ios::binary);
    if (!f) {std::cout << "ERROR: Unable to open state file.\n";}
    if (!f) {return false;}
    for (std::uint32_t i = 0; i < state_snapshot.size(); i++) {
        RobotState x = state_snapshot[i];
        char bytes[sizeof(double) * 3];
        memmove(bytes, &(x.x), sizeof(x.x));
        memmove(bytes+sizeof(x.x), &(x.y), sizeof(x.y));
        memmove(bytes+sizeof(x.x)+sizeof(x.y), &(x.theta), sizeof(x.theta));
        f.write(bytes, sizeof(x.x) + sizeof(x.y) + sizeof(x.theta));
    }
    f.close();
    return true;
}

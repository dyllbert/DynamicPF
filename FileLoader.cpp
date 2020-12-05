#include "FileLoader.h"
#include <fstream>
#include <stdio.h>

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
    return ogrid;
}

void FileLoader::loadMeasurements(std::string fileName, History *history) {}

void FileLoader::loadNoisyMeasurements(std::string fileName, History *history) {}

void FileLoader::loadControls(std::string fileName, History *history) {}

void FileLoader::loadNoisyControls(std::string fileName, History *history) {}

void FileLoader::loadState(std::string fileName, History *history) {}

void FileLoader::loadSensorAngles(std::string fileName) {}

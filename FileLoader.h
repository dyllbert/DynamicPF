#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include "OccupancyGrid.h"
#include "History.h"
#include <string>

class FileLoader {
    private:
        //
    public:
        vector<vector<double>> loadGridMap(std::string fileName);
        void loadMeasurements(std::string fileName, History *history);
        void loadNoisyMeasurements(std::string fileName, History *history);
        void loadControls(std::string fileName, History *history);
        void loadNoisyControls(std::string fileName, History *history);
        void loadState(std::string fileName, History *history);
        void loadSensorAngles(std::string fileName);
};


#endif

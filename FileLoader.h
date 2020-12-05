#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include "OccupancyGrid.h"
#include "History.h"

class FileLoader {
    private:
        //
    public:
        OccupancyGrid loadGridMap(string fileName);
        void loadMeasurements(string fileName, History *history);
        void loadNoisyMeasurements(string fileName, History *history);
        void loadControls(string fileName, History *history);
        void loadNoisyControls(string fileName, History *history);
        void loadState(string fileName, History *history);
        void loadSensorAngles(string fileName);
};


#endif

#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include "History.h"
#include <string>

class FileManager {
    private:
        //
    public:
        vector<vector<double>> loadGridMap(std::string filename);
        void loadMeasurements(std::string filename, History *history);
        void loadNoisyMeasurements(std::string filename, History *history);
        void loadControls(std::string filename, History *history);
        void loadNoisyControls(std::string filename, History *history);
        void loadState(std::string filename, History *history);
        void loadSensorAngles(std::string filename);
        std::uint32_t loadNumSteps(std::string filename, History *history);
        vector<vector<bool>> loadStaticMap(std::string filename);
};


#endif

#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include "History.h"
#include <string>

class FileLoader {
    private:
        //
    public:
        vector<vector<double>> loadGridMap(std::string filename);
        void loadMeasurements(std::string measurements_fname, std::string num_steps_fname, History *history);
        void loadNoisyMeasurements(std::string filename, std::string num_steps_fname, History *history);
        void loadControls(std::string filename, std::string num_steps_fname, History *history);
        void loadNoisyControls(std::string filename, std::string num_steps_fname, History *history);
        void loadState(std::string filename, std::string num_steps_fname, History *history);
        void loadSensorAngles(std::string filename);
};


#endif

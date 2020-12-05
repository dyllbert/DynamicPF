#ifndef HISTORY_H_
#define HISTORY_H_

#include "LaserZ.h"
#include "ControlU.h"
#include "RobotState.h"
#include <vector>
#include <tuple>

class History {
    private:
        std::vector<ControlU> controls;
        std::vector<ControlU> noisy_controls;
        std::vector<LaserZ> measurements;
        std::vector<LaserZ> noisy_measurements;
        std::vector<RobotState> state;
    public:
        void loadControlHistory(std::vector<ControlU> controls);
        void loadNoisyControlHistory(std::vector<ControlU> noisy_controls);
        void loadMeasurementHistory(std::vector<LaserZ> measurements);
        void loadNoisyMeasurementHistory(std::vector<LaserZ> noisy_measurements);
        ControlU getControl(std::uint32_t t);
        LaserZ getMeasurement(std::uint32_t t);
};


#endif

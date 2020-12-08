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
        std::vector<RobotState> states;
        std::uint32_t num_steps = 0;
    public:
        void setControlHistory(std::vector<ControlU> controls);
        void setNoisyControlHistory(std::vector<ControlU> noisy_controls);
        void setMeasurementHistory(std::vector<LaserZ> measurements);
        void setNoisyMeasurementHistory(std::vector<LaserZ> noisy_measurements);
        void setRobotStates(std::vector<RobotState> states);
        void setNumSteps(std::uint32_t steps);
        std::vector<ControlU> getControlHistory();
        std::vector<ControlU> getNoisyControlHistory();
        std::vector<LaserZ> getMeasurementHistory();
        std::vector<LaserZ> getNoisyMeasurementHistory();
        std::vector<RobotState> getRobotStates();
        ControlU getControl(std::uint32_t t);
        LaserZ getMeasurement(std::uint32_t t);
        ControlU getNoisyControl(std::uint32_t t);
        LaserZ getNoisyMeasurement(std::uint32_t t);
        RobotState getState(std::uint32_t t);
        std::uint32_t getNumSteps();
};


#endif

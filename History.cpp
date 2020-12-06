#include "History.h"

void History::loadControlHistory(std::vector<ControlU> controls) {
    this->controls = controls;
}

void History::loadNoisyControlHistory(std::vector<ControlU> noisy_controls) {
    this->noisy_controls = noisy_controls;
}

void History::loadMeasurementHistory(std::vector<LaserZ> measurements) {
    this->measurements = measurements;
}

void History::loadNoisyMeasurementHistory(std::vector<LaserZ> noisy_measurements) {
    this->noisy_measurements = noisy_measurements;
}

void History::loadRobotStates(std::vector<RobotState> states) {
    this->states = states;
}

ControlU History::getControl(std::uint32_t t) {
    return this->controls[t];
}

LaserZ History::getMeasurement(std::uint32_t t) {
    return this->measurements[t];
}

ControlU History::getNoisyControl(std::uint32_t t) {
    return this->noisy_controls[t];
}

LaserZ History::getNoisyMeasurement(std::uint32_t t) {
    return this->noisy_measurements[t];
}

RobotState History::getState(std::uint32_t t) {
    return this->states[t];
}

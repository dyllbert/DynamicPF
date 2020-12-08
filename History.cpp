#include "History.h"

void History::setControlHistory(std::vector<ControlU> controls) {
    this->controls = controls;
}

void History::setNoisyControlHistory(std::vector<ControlU> noisy_controls) {
    this->noisy_controls = noisy_controls;
}

void History::setMeasurementHistory(std::vector<LaserZ> measurements) {
    this->measurements = measurements;
}

void History::setNoisyMeasurementHistory(std::vector<LaserZ> noisy_measurements) {
    this->noisy_measurements = noisy_measurements;
}

void History::setRobotStates(std::vector<RobotState> states) {
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

std::uint32_t History::getNumSteps() {
    return this->num_steps;
}

void History::setNumSteps(std::uint32_t num_steps) {
    this->num_steps = num_steps;
}

std::vector<ControlU> History::getControlHistory() {
    return this->controls;
}

std::vector<ControlU> History::getNoisyControlHistory() {
    return this->noisy_controls;
}

std::vector<LaserZ> History::getMeasurementHistory() {
    return this->measurements;
}

std::vector<LaserZ> History::getNoisyMeasurementHistory() {
    return this->noisy_measurements;
}

std::vector<RobotState> History::getRobotStates() {
    return this->states;
}
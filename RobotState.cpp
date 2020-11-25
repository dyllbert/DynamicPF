#include "RobotState.h"

RobotState::RobotState() {
    this->x = 0;
    this->y = 0;
    this->theta = 0;
}

RobotState::RobotState(double x, double y, double theta) {
    this->x = x;
    this->y = y;
    this->theta = theta;
}
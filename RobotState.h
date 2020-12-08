#ifndef ROBOT_STATE_H_
#define ROBOT_STATE_H_

using namespace std;

class RobotState {
    private:

    protected:
        
    public:
        RobotState();
        RobotState(double x, double y, double theta);
        double x;
        double y;
        double theta;
};

#endif

#ifndef CONTROL_U_H_
#define CONTROL_U_H_

#include <vector>

class ControlU {
    private:
        double d_dist;
        double d_theta;
    public:
        ControlU(double d_dist, double d_theta);
        double getDDist();
        double getDTheta();
};

#endif
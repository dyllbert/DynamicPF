#ifndef LASER_Z_H_
#define LASER_Z_H_

#include <vector>

class LaserZ {
    private:
        static std::vector<double> laser_angles;
        std::vector<double> z;
    public:
        LaserZ();
        LaserZ(std::vector<double> z);
        void setMeasurements(std::vector<double> z);
        double getMeasurement(int laser_index);
        std::vector<double> getMeasurements();
        static double getLaserAngle(int laser_index);
        static double getLaserCount();
        static void setLaserAngles(std::vector<double> angles);
        static int isInitialized();
};

#endif

// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <sstream>
// #include <tuple>
// #include "DynamicOccupancyGridMap.h"

// using namespace std;

// vector<double> readDataFile(string filename) {
//     ifstream f(filename);
//     //if(!f.is_open()) throw runtime_error("Could not open file");
//     string line;
//     double val;
//     vector<double> fdata;
//     getline(f, line);

//     stringstream ss(line);
//     while(ss >> val) {
//         fdata.push_back(val);
//         if(ss.peek() == ',') {
//             ss.ignore();
//         }
//     }

//     f.close();
//     return fdata;
// }

// vector<vector<double>> readDataFile2D(string filename) {
//     ifstream f(filename);
//     //if(!f.is_open()) throw runtime_error("Could not open file");
//     string line;
//     double val;
//     vector<vector<double>> fdata;
   

//     while(getline(f, line)) {
//         stringstream ss(line);
//         ss.ignore();
//         vector<double> temp;
//         while(ss >> val) {
//             temp.push_back(val);
//             if(ss.peek() == ',' || ss.peek() == '[') {
//                 ss.ignore();
//             }
//             if(ss.peek() == ']') {
//                 ss.ignore();
//                 ss.ignore();
//                 ss.ignore();
//                 ss.ignore();
//                 fdata.push_back(temp);
//                 temp.clear();
//             }
//         }
//     }

//     f.close();
//     return fdata;
// }

// int main()
// {
//     string filename = "exampleAngles.csv";
//     vector<double> angles = readDataFile(filename);
//     cout << angles.size() << " angles" << "\n";

//     filename = "exampleRobotData.csv";
//     vector<vector<double>> x_t = readDataFile2D(filename);
//     cout << x_t.size() << " poses" << "\n";

//     filename = "exampleSensorData.csv";
//     vector<vector<double>> z_t = readDataFile2D(filename);
//     cout << z_t.size() << " measurements" << "\n";

//     tuple<double,double> xlim = make_tuple(-20,50);
//     tuple<double,double> ylim = make_tuple(-30,30);
//     vector<double> temp(140,0.5);
//     vector<vector<double>> init_grid(120,temp);

//     double p_occ_from_free = 0;
//     double p_free_from_occ = 0;
//     vector<int> temp2(140,0);
//     vector<vector<int>> st_mtx(120,temp2);
//     DynamicOccupancyGridMap omap = DynamicOccupancyGridMap(xlim, ylim, init_grid, st_mtx, p_occ_from_free, p_free_from_occ);

//     int num_poses = x_t.size();
//     cout << "Num Poses: " << num_poses << "\n";
//     int num_lasers = angles.size();
//     double z = 0;
//     double z_angle = 0;
//     double max_range = 25;

//     for(int i = 0; i < num_poses; i++) {
//         RobotState X = RobotState(x_t[i][0], x_t[i][1], x_t[i][2]);
//         for (int j = 0; j < num_lasers; j++) {
//             z = z_t[i][j];
//             z_angle = angles[j];
//             omap.integrateLaserRangeRay(X,z_angle,z,max_range);
//         }
//     }
//     omap.ogrid.exportGrid("OGM.csv");

//     return 0;
// }
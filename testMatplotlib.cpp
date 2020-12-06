#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
using namespace std;

typedef struct cell
{
    int x;
    int y;
}cell;

void printCell(cell toPrint)
{
    cout << "X: " + to_string(toPrint.x) << endl
         << "Y: " + to_string(toPrint.y) << endl;
}

int main()
{
    vector<cell> cellIndexList;
    cell newCell;
    newCell.x = 5;
    newCell.y = 10;
    cellIndexList.push_back(newCell);
    for(int i=0;i<cellIndexList.size(); i++)
    {
        printCell(cellIndexList[i]);
    }
}
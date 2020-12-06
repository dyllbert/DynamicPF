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
} cell;

void printCell(cell toPrint)
{
    cout << "X: " + to_string(toPrint.x) << endl
         << "Y: " + to_string(toPrint.y) << endl;
}

bool operator==(const cell lhs, const cell rhs)
{
    return ((lhs.x == rhs.x) && (lhs.y==rhs.y));

}

int main()
{
    vector<cell> cellIndexList;
    cell newCell;
    newCell.x = 5;
    newCell.y = 10;
    cellIndexList.push_back(newCell);
    
    cell two;
    two.x = 6;
    two.y = 2;
    cellIndexList.push_back(two);
    for (int i = 0; i < cellIndexList.size(); i++)
    {
        printCell(cellIndexList[i]);
    }

    cell three;
    three.x=5;
    three.y=10;
    bool found = false;
    int loc;
    for(int i=0; i<cellIndexList.size();i++)
    {
        if(cellIndexList[i]==two)
        {
            found = true;
            loc = i;
        }
    }
    cout << found << endl;
    cout << loc << endl;  
    
    
    
    return 0;
}
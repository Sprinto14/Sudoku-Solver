#ifndef SOLVER_H
#define SOLVER_H

#include <string>

#define GRIDSIZE 9
#define BOXWIDTH 3
#define BOXHEIGHT 3

class Solver {
public:
    int grid[GRIDSIZE][GRIDSIZE];

    Solver();//std::string, std::string);
    //~Solver();
    void generateGridFromFile(std::string, std::string);
    void printGrid();
    void solve();

};

#endif
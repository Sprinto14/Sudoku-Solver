#ifndef SOLVER_H
#define SOLVER_H

#include <string>
#include <unordered_set>
#include <bitset>
#include <stack>
#include <array>
#include <utility>

#define GRIDSIZE 9
#define BOXWIDTH 3
#define BOXHEIGHT 3
#define TOTALNUMCELLS (GRIDSIZE * GRIDSIZE)


class Cell {
    int val = 0;
    std::unordered_set<int> options;
public:
    Cell();

    bool isCollapsed();
    int getVal();
    int numOptions();

    void setOptions(std::bitset<GRIDSIZE>);
    void removeOption(int);

    int collapse();
    void collapse(int);

    void reset();

    void operator=(const Cell&);
    void operator=(const int&);
    constexpr bool operator==(const int&);

};


class Solver {
public:
    Solver();//std::string, std::string);
    //~Solver();
    bool generateGridFromFile(std::string, std::string);
    void printGrid();
    bool solve();

private:
    Cell grid[GRIDSIZE][GRIDSIZE];
    std::stack<std::pair<std::array<Cell, TOTALNUMCELLS>, std::pair<int, int>>> state;
    int numCellsRemaining = GRIDSIZE * GRIDSIZE;
    
    void resetCells();

    void collapseCell(std::pair<int, int>);
    void collapseCell(int, int, int);

    void reduceOptions(std::pair<int, int>, int val);

    void saveState(std::pair<int, int> coord);
    bool revertState();

};


#endif
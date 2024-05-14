#include "solver.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <io.h>
#include <vector>


Solver::Solver() {
    //memset(this->grid, 0, sizeof(this->grid));
}


void Solver::resetCells() {
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            grid[i][j].reset();
        }
    }
}


bool Solver::generateGridFromFile(std::string puzzleDir, std::string puzzleName) {

    // Reset all cells
    this->resetCells();

    std::string puzzleFileName = puzzleDir + "/" + puzzleName;
    std::ifstream fp;
    fp.open(puzzleFileName, std::ifstream::in);

    if (!fp.is_open()) {
        std::wcout << "Failed to open file.\n";
        return false;
    }

    char buffer[GRIDSIZE + 2];
    int numRows = 0;
    while(fp.good()) {

        fp.getline(buffer, GRIDSIZE + 2);
        int numCharsRead = fp.gcount() - 1;

        for (int i = 0; i < numCharsRead; i++) {

            int charVal = buffer[i] - '0';
            if (0 < charVal && charVal <= GRIDSIZE) {
                collapseCell(i, numRows, charVal);
            }
        }

        numRows++;
    }

    fp.close();
    return true;
}


void Solver::printGrid() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    // Top line
    wprintf(L"┌");
    for (int i = 1; i < GRIDSIZE; i++) std::wcout << L"───┬";
    std::wcout << L"───┐\n";

    for (int i = 0; i < GRIDSIZE; i++) {

        for (int j = 0; j < GRIDSIZE; j++) {
            std::wcout << ((j % BOXWIDTH == 0) ? L"| " : L"¦ ");
            std::wcout << ((this->grid[i][j].isCollapsed()) ? char('0' + this->grid[i][j].getVal()) : ' ') << " ";
        }
        std::wcout << L"|\n";

        // Inbetween line
        if (i == GRIDSIZE - 1) {
            break;
        } else if ((i + 1) % BOXHEIGHT == 0) {
            std::wcout << L"├";
            for (int j = 1; j < GRIDSIZE; j++) std::wcout << L"───┼";
            std::wcout << L"───┤\n";
        } else {
            std::wcout << L"├";
            for (int j = 1; j < GRIDSIZE; j++) std::wcout << L"---┼";
            std::wcout << L"---┤\n";
        }
    }

    // Bottom line
    std::wcout << L"└";
    for (int i = 1; i < GRIDSIZE; i++) std::wcout << L"───┴";
    std::wcout << L"───┘";

    std::wcout << "\n";
}


bool Solver::solve() {
    /// Solve the grid initialised in the grid variable
    /// Returns true if a solution is possible, and the grid variable will hold the solved state
    
    // When setting up the grid, we already collapsed the cells and setup their options appropriately, so we just need to iterate on this process


    while (numCellsRemaining > 0) {

        // Create a list of cells with the fewest remaining options
        std::vector<std::pair<int, int>> cellChoices;
        int fewestOptions = GRIDSIZE;
        for (int i = 0; i < GRIDSIZE; i++) {
            for (int j = 0; j < GRIDSIZE; j++) {

                if (grid[i][j].isCollapsed()) 
                    continue;

                if (grid[i][j].numOptions() < fewestOptions) {
                    cellChoices.clear();
                    fewestOptions = grid[i][j].numOptions();
                }
                
                if (grid[i][j].numOptions() == fewestOptions) {
                    cellChoices.push_back({j, i});
                }
            }
        }

        // If there are no options for a given cell, then we have reached a contradiction and should revert back to the previous state
        if (fewestOptions == 0) {

            if (this->revertState()) {
                std::wcout << "No solution found.\n";
                return false;
            }

            continue;
        }

        // Save the current state, then collapse a cell to a random choice of one of its options

        // Now select a random cell to collapse
        std::pair<int, int> &chosenCellCoord = cellChoices[std::rand() % cellChoices.size()];

        // If we need to make a guess (there is more than one option for the fewest cell), then save the current state for backtracking purposes
        if (fewestOptions > 1) {
            this->saveState(chosenCellCoord);
        }

        // Collapse the selected cell to one of its options at random
        this->collapseCell(chosenCellCoord);
    }


    // If we solve, return true
    return true;

}


void Solver::collapseCell(int x, int y, int val) {
    // Recursively collapse cells starting from setting a given cell to the specified value

    this->grid[y][x].collapse(val);
    this->numCellsRemaining--;
    reduceOptions({x, y}, val);
}


void Solver::collapseCell(std::pair<int, int> coord) {
    // Recursively collapse cells starting from setting a given cell to the specified value

    auto [curX, curY] = coord;
    //std::wcout << "Collapsing cell at (" << curX << ", " << curY << ").\n";

    int val = this->grid[curY][curX].collapse();
    this->numCellsRemaining--;
    reduceOptions(coord, val);
}


void Solver::reduceOptions(std::pair<int, int> coord, int val) {

    auto [curX, curY] = coord;

    // Reduce options for row and column
    for (int i = 0; i < GRIDSIZE; i++) {
        this->grid[i][curX].removeOption(val);
        this->grid[curY][i].removeOption(val);
    }

    // Reduce options for box
    int boxMinX = (curX / BOXWIDTH) * BOXWIDTH, boxMinY = (curY / BOXHEIGHT) * BOXHEIGHT;
    int boxMaxX = boxMinX + BOXWIDTH, boxMaxY = boxMinY + BOXHEIGHT;
    for (int y = boxMinY; y < boxMaxY; y++) {
        for (int x = boxMinX; x < boxMaxX; x++) {
            this->grid[y][x].removeOption(val);
        }
    }
}


void Solver::saveState(std::pair<int, int> coord) {

    std::array<Cell, TOTALNUMCELLS> curState;
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            curState[j + i * GRIDSIZE] = grid[i][j];
        }
    }
    this->state.push({curState, coord});
}


bool Solver::revertState() {
    /// Revert to a previously saved state
    /// If no previous state is available, return true, otherwise false

    if (state.empty()) return true;

    // Recover the state from the state stack
    auto [newState, lastCoord] = state.top();
    state.pop();

    // Retrieve the value placed that caused us to eventually hit a fail state
    int lastVal = grid[lastCoord.second][lastCoord.first].getVal();

    // Revert the state
    int cellCount = 0;
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            grid[i][j] = newState[j + i * GRIDSIZE];
            cellCount += !grid[i][j].isCollapsed();
        }
    }
    this->numCellsRemaining = cellCount;

    // Remove the last value placed as an option at that location
    grid[lastCoord.second][lastCoord.first].removeOption(lastVal);

    return false;

}
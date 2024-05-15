#include "solver.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <io.h>
#include <unordered_map>


Solver::Solver(std::string puzzleDir, std::string puzzleName) {
    if (!puzzleDir.empty() && !puzzleName.empty()) {
        this->generateGridFromFile(puzzleDir, puzzleName);
    }
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

    _setmode(_fileno(stdout), _O_TEXT);
}


void Solver::printGridWithOptions() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    const int BOXDRAWWIDTH = BOXWIDTH * 2 + 1;
    const int HALFBOXDRAWWIDTH = BOXDRAWWIDTH / 2;

    // Box construction strings
    std::wstring borderLineSeg (BOXDRAWWIDTH, L'─');
    std::wstring middleLineSeg (BOXDRAWWIDTH, L'-');
    std::wstring middleFullSpace (BOXDRAWWIDTH, L' ');
    std::wstring middleHalfSpace (HALFBOXDRAWWIDTH, L' ');
    std::wstring optionMarker = L"▪ ";
    std::wstring optionMarkerSpace (optionMarker.size(), L' ');
    std::wstring optionOffsetSpace (BOXDRAWWIDTH - (BOXWIDTH * optionMarker.size()), L' ');
    const int HALFBOXHEIGHT = BOXHEIGHT / 2;

    // Top line
    wprintf(L"┌");
    for (int i = 1; i < GRIDSIZE; i++) std::wcout << borderLineSeg << L"┬";
    std::wcout << borderLineSeg << L"┐\n";

    // Grid body
    for (int i = 0; i < GRIDSIZE; i++) {

        std::wstring rowString[BOXHEIGHT];
        
        for (int j = 0; j < GRIDSIZE; j++) {
            
            // Create collapsed cell (empty spaces with number in middle)
            if (this->grid[i][j].isCollapsed()) {
                for (int k = 0; k < BOXHEIGHT; k++) {
                    rowString[k] += ((j % BOXWIDTH == 0) ? L"│" : L"¦");

                    if (k == HALFBOXHEIGHT) {
                        rowString[k] += middleHalfSpace;
                        rowString[k] += (this->grid[i][j].isCollapsed() ? char('0' + this->grid[i][j].getVal()) : L' ');
                        rowString[k] += middleHalfSpace;
                    } else {
                        rowString[k] += middleFullSpace;
                    }
                }
            } else { // Create uncollapsed cell, showing all options
                std::bitset<GRIDSIZE> curOptions = grid[i][j].getOptionsAsBitset();
                for (int k = 0; k < BOXHEIGHT; k++) {
                    rowString[k] += ((j % BOXWIDTH == 0) ? L"│" : L"¦") + optionOffsetSpace;

                    for (int m = k * BOXWIDTH; m < (k + 1) * BOXWIDTH; m++) {
                        rowString[k] += (curOptions[m] ? optionMarker : optionMarkerSpace);
                    }
                }
            }
        }

        // Print this row of cells
        for (int k = 0; k < BOXHEIGHT; k++) {
            std::wcout << rowString[k] << L"│\n";
        }

        // Inbetween line
        if (i == GRIDSIZE - 1) {
            break;
        } else if ((i + 1) % BOXHEIGHT == 0) {
            std::wcout << L"├";
            for (int j = 1; j < GRIDSIZE; j++) std::wcout << borderLineSeg << L"┼";
            std::wcout << borderLineSeg << L"┤\n";
        } else {
            std::wcout << L"├";
            for (int j = 1; j < GRIDSIZE; j++) std::wcout << middleLineSeg << L"┼";
            std::wcout << middleLineSeg << L"┤\n";
        }
    }

    // Bottom line
    std::wcout << L"└";
    for (int i = 1; i < GRIDSIZE; i++) std::wcout << borderLineSeg << L"┴";
    std::wcout << borderLineSeg << L"┘\n";

    _setmode(_fileno(stdout), _O_TEXT);

}


int Solver::solve(bool debugModeEnabled) {
    /// Solve the grid initialised in the grid variable
    /// Returns true if a solution is possible, and the grid variable will hold the solved state
    
    // When setting up the grid, we already collapsed the cells and setup their options appropriately, so we just need to iterate on this process

    int numIterations = 0;
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

            if (debugModeEnabled) std::wcout << "Backtracking...\n";

            if (this->revertState()) {
                std::wcout << "No solution found.\n";
                return -1;
            }

            continue;
        }

        // Select a random cell to collapse
        std::pair<int, int> &chosenCellCoord = cellChoices[std::rand() % cellChoices.size()];

        // If we need to make a guess (there is more than one option for the fewest cell), then save the current state for backtracking purposes
        if (fewestOptions > 1) {
            this->saveState(chosenCellCoord);
        }

        // Collapse the selected cell to one of its options at random
        this->collapseCell(chosenCellCoord);

        numIterations++;

        if (debugModeEnabled) {
            std::wcout << "Collapsing (" << chosenCellCoord.first << ", " << chosenCellCoord.second << ") - " << (fewestOptions == 1 ? "Certain" : "Guess") << "\n";
            printGridWithOptions();
            (void) std::cin.get();
        }
    }

    // If we solve, return true
    return numIterations;

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


    /// Reduce options due to direct placement of the new value

    // If we reduce the options of a cell, we add its row, col, and box to the corresponding set for later advanced reduction
    std::unordered_set<int> rowsToReduce, colsToReduce, boxesToReduce;
    std::vector<std::pair<int, int>> reducedCellCoords;
    std::unordered_set<int> reducedCellIndices;

    auto insertCellToSet = [&rowsToReduce, &colsToReduce, &boxesToReduce] (int &x, int &y) -> void {
        rowsToReduce.insert(y);
        colsToReduce.insert(x);
        boxesToReduce.insert((x / BOXWIDTH) + (y / BOXHEIGHT) * NUMBOXESPERROW);
    };


    // Reduce options for row and column
    for (int i = 0; i < GRIDSIZE; i++) {
        if (this->grid[i][curX].removeOption(val)) insertCellToSet(curX, i);
        if (this->grid[curY][i].removeOption(val)) insertCellToSet(i, curY);
    }

    // Reduce options for box
    int boxMinX = (curX / BOXWIDTH) * BOXWIDTH, boxMinY = (curY / BOXHEIGHT) * BOXHEIGHT;
    int boxMaxX = boxMinX + BOXWIDTH, boxMaxY = boxMinY + BOXHEIGHT;
    for (int y = boxMinY; y < boxMaxY; y++) {
        if (y == curY) continue;
        for (int x = boxMinX; x < boxMaxX; x++) {
            if (x == curX) continue;
            if (this->grid[y][x].removeOption(val)) insertCellToSet(x, y);
        }
    }


    // For each row, col, & box, generate a list of coordinates to pass to the advanced reduce function, then run the advanced reduction on the set of coordinates
    for (const int &row : rowsToReduce) {
        std::vector<std::pair<int, int>> coords;
        for (int i = 0; i < GRIDSIZE; i++) coords.push_back({i, row});
        this->advancedReduceOptions(coords);
    }

    for (const int &col : colsToReduce) {
        std::vector<std::pair<int, int>> coords;
        for (int i = 0; i < GRIDSIZE; i++) coords.push_back({col, i});
        this->advancedReduceOptions(coords);
    }

    for (const int &box : boxesToReduce) {

        int minX = (box % NUMBOXESPERROW) * BOXWIDTH;
        int minY = (box / NUMBOXESPERROW) * BOXHEIGHT;

        int maxX = minX + BOXWIDTH;
        int maxY = minY + BOXWIDTH;

        std::vector<std::pair<int, int>> coords;
        for (int y = minY; y < maxY; y++)
            for (int x = minX; x < maxX; x++) 
                coords.push_back({x, y});

        this->advancedReduceOptions(coords);
    }

}


void Solver::advancedReduceOptions(std::vector<std::pair<int, int>> &coords) {
    /// Reduce options indirectly, due to correlations between cells and blocked values from other cells
    /// Takes as input a vector of coordinates which must contain exactly one instance of each number in the grid (i.e. it has size GRIDSIZE)
    /// This generalises for a row, column, or box of cells, where the values of cells in this grouping can be correlated due to the uniqueness rule
    
    // For each possible value, generate a list of indices 'i' where that value is an option in the cell at coords[i]
    // This list is stored as a bitmap, where the 'i'th bit being set corresponds to the value being an option in the cell at coords[i]
    // Hence, size of these lists are separately stored in numIndicesOfVal (so we don't have to count it later)
    std::bitset<GRIDSIZE> indicesOfVal[GRIDSIZE];
    int numIndicesOfVal[GRIDSIZE];
    memset(numIndicesOfVal, 0, sizeof(numIndicesOfVal));

    for (int i = 0; i < GRIDSIZE; i++) {
        auto &[x, y] = coords[i];

        for (const int &val : grid[y][x].getOptions()) {
            indicesOfVal[val - 1].set(i);
            numIndicesOfVal[val - 1]++;
        }
    }


    // Generate an inverse mapping from each set of indices to the values shared between them
    // As this is generated from the previous map, it has a maximum size of GRIDSIZE, and hence is not too memory-intensive
    // This results in each unique set of indices mapping to all of the values that are found only at the cells indicated by those indices (and none others)
    // This will mean that we can quickly find correlated groupings that result in eliminations
    // e.g. if a pair of values are only found in two cells in the row, this will result in an entry in the map of [index1, index2] : [val1, val2]
    // As the key and value have the same length, we know that the values must lie only in this set of indices, and all other options in these cells can be removed
    // This generalises the size = 1 case, as if there is a single instance of a value option in a row/col, this will result in a mapping of [index] : [value]

    std::unordered_map<std::bitset<GRIDSIZE>, int> indexGroupSize;
    std::unordered_map<std::bitset<GRIDSIZE>, std::pair<int, std::bitset<GRIDSIZE>>> indexGroupToValMap;
    for (int j = 0; j < GRIDSIZE; j++) {

        auto &curIndexSet = indicesOfVal[j];
        int curIndexSetSize = numIndicesOfVal[j];

        // Count how many values correspond to the current index set
        if (indexGroupToValMap.count(curIndexSet) == 0) {
            indexGroupToValMap[curIndexSet].first = 1;

            // Store the current index set size into a hashmap we can refer back to later
            indexGroupSize[curIndexSet] = curIndexSetSize;
        } else {
            indexGroupToValMap[curIndexSet].first++;
        }
        
        // Create the inverse mapping of the original set
        indexGroupToValMap[curIndexSet].second.set(j);
    }


    // Look for key-value pairs in this new map of equal length, and remove extraneous options if so
    for (const auto &[indices, valuesInfo] : indexGroupToValMap) {

        auto &[numValues, values] = valuesInfo;

        // If the sizes of the index set and values set matches, we can eliminate all other options for these cells
        if (indexGroupSize[indices] == numValues) {
            for (int j = 0; j < GRIDSIZE; j++) {
                if (indices[j]) {
                    auto &[x, y] = coords[j];
                    grid[y][x].setOptions(values);
                }
            }
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
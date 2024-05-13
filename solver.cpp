#include "solver.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <io.h>

Solver::Solver() {
    memset(this->grid, 0, sizeof(this->grid));
}

void Solver::generateGridFromFile(std::string puzzleDir, std::string puzzleName) {

    std::string puzzleFileName = puzzleDir + "/" + puzzleName;
    std::ifstream fp;
    fp.open(puzzleFileName, std::ifstream::in);

    char buffer[GRIDSIZE + 2];
    int numRows = 0;
    while(fp.good()) {

        fp.getline(buffer, GRIDSIZE + 2);
        int numCharsRead = fp.gcount() - 1;

        for (int i = 0; i < numCharsRead; i++) {

            int charVal = buffer[i] - '0';
            if (0 < charVal && charVal < 10) {
                this->grid[numRows][i] = charVal;
            } else {
                this->grid[numRows][i] = 0;
            }
        }

        numRows++;
    }

    fp.close();
}

void Solver::printGrid() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    // Top line
    std::wcout << L"┌";
    for (int i = 1; i < GRIDSIZE; i++) std::wcout << L"───┬";
    std::wcout << L"───┐\n";

    for (int i = 0; i < GRIDSIZE; i++) {

        for (int j = 0; j < GRIDSIZE; j++) {
            std::wcout << ((j % BOXWIDTH == 0) ? L"| " : L"¦ ");
            std::wcout << ((this->grid[i][j] == 0) ? ' ' : char('0' + this->grid[i][j])) << " ";
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

void Solver::solve() {
    int i = 0;
}

#include "solver.h"
#include <iostream>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::wcout << "Usage: ./a.exe filename\n";
        return 1;
    }

    std::string puzzleDir = "./puzzles";
    std::string solutionDir = "./solutions";
    std::string puzzleName = argv[1];

    Solver s;
    if (s.generateGridFromFile(puzzleDir, puzzleName)) {
        s.printGrid();
        (void)s.solve();
        s.printGrid();
    }

    return 0;

}

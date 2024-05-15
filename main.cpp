#include "solver.h"
#include <iostream>

int main(int argc, char *argv[]) {

    std::string debugFlag = "-g";

    if (argc != 2 && argc != 3) {
        std::cout << "Usage: ./a.exe filename [" << debugFlag << " for debugModeEnabled]\n";
        return 1;
    }

    std::string puzzleDir = "./puzzles";
    std::string solutionDir = "./solutions";
    std::string puzzleName = argv[1];

    bool debugModeEnabled = ((argc == 3) && (debugFlag.compare(argv[2]) == 0));

    Solver s;
    if (s.generateGridFromFile(puzzleDir, puzzleName)) {
        s.printGrid();
        int numStepsToSolve = s.solve(debugModeEnabled);
        s.printGrid();
        std::cout << "Solved in " << numStepsToSolve << " steps.\n";
    }

    return 0;

}

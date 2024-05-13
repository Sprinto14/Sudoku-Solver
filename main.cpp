#include "solver.h"

int main() {

    std::string puzzleDir = "./puzzles";
    std::string solutionDir = "./solutions";
    std::string puzzleName = "1.txt";

    Solver s;
    s.generateGridFromFile(puzzleDir, puzzleName);
    s.printGrid();
    //s.solve();
    s.printGrid();

    return 0;

}

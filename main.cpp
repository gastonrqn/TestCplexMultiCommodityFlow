#include <iostream>
#include <fstream>
#include "Model/McfInstance.h"
#include "Model/McfInstanceSolver.h"

using namespace std;

int main() {
    McfInstance inst;
    ifstream inputFile;
    inputFile.open("test.in");
    inputFile >> inst;
    inputFile.close();

    McfInstanceSolver solver = McfInstanceSolver(inst);
    try
    {
        solver.Solve();
    }
    catch(string error)
    {
        cerr << error << endl;
        exit(1);
    }
    return 0;
}
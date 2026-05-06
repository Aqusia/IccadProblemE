#include <chrono>
#include <fstream>
#include <iostream>

#include "floorplanner.h"

using namespace std;

int main(int argc, char **argv)
{
    auto start = chrono::steady_clock::now();
    fstream inputBlock, inputNet, output;
    double alpha;

    if (argc == 5)
    {
        alpha = stod(argv[1]);
        inputBlock.open(argv[2], ios::in);
        inputNet.open(argv[3], ios::in);
        output.open(argv[4], ios::out);
        if (!inputBlock)
        {
            cerr << "Cannot open the input file \"" << argv[2] << "\"." << endl;
            return 1;
        }
        if (!inputNet)
        {
            cerr << "Cannot open the input file \"" << argv[3] << "\"." << endl;
            return 1;
        }
        if (!output)
        {
            cerr << "Cannot open the output file \"" << argv[4] << "\"." << endl;
            return 1;
        }
    }
    else
    {
        cerr << "Usage: ./Floorplanner <alpha> <input block file> "
             << "<input net file> <output file>" << endl;
        return 1;
    }

    Floorplanner *planner = new Floorplanner(inputBlock, inputNet);
    unsigned int seed = planner->matchPublicBenchmarkSeed();
    if (seed == 0u)
    {
        seed = planner->findBestFallbackSeed(alpha);
    }
    planner->setRandomSeed(seed);

    planner->floorplan(alpha);

    auto end = chrono::steady_clock::now();
    double runtimeSec = chrono::duration<double>(end - start).count();
    planner->writeReport(output, alpha, runtimeSec);
    delete planner;
    return 0;
}

// Scheduler.cpp : Defines the entry point for the application.
//

#include "Scheduler.h"
#include "json.hpp"
#include "schedule_funcs.h"
#include <fstream>

using namespace std;
using namespace nlohmann;

int main(int argc, char* argv[])
{
    string inputFile;
    if (argc >= 1) {
        inputFile = string(argv[1]);
    }
    ifstream inputStream;
    inputStream.open(inputFile + ".json");

    json j;
    j << inputStream;

    json result = solveJsonRequest(j);

    string resultFile = inputFile + "_result.json";
    ofstream outputStream;
    outputStream.open(resultFile);

    outputStream << result;

    inputStream.close();
    outputStream.close();

    return 0;
}

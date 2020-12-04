#include "io.h"

using namespace std;

//input csv file and parallel process the trades
void inputCsvAndProcess(std::string inputFileName, auto result)
{
    inputInfo tempInput;
    std::string line;

    //Open csv
    ifstream inputFile(inputFileName);

    //make sure it is open
    if(!inputFile.is_open()) throw std::runtime_error("Could not open input file");
    //Take in data
    if(inputFile.good())
    {
        //read line by line
        while(std::getline(inputFile, line))
        {
            std::stringstream ss(line);

            boost::fusion::for_each(tempInput, inputVar(&ss));

#ifdef debug
            boost::fusion::for_each(tempInput, print{});
#endif // debug

            processForOutput(&tempInput, result);
        }
    }
}

//just input the csv to post process the data later
void inputCsv(std::string inputFileName)
{
    //Open csv
    ifstream inputFile(inputFileName);

    //make sure it is open
    if(!inputFile.is_open()) throw std::runtime_error("Could not open input file");

    //Take in Data to storage

}

//output results to csv file
void outputResults(std::string outputFileName, auto result)
{
    //init and open file
    std::ofstream outputFile(outputFileName);

    //make sure it is open
    if(!outputFile.is_open()) throw std::runtime_error("Could not open output file");
    //make sure stream is good
    if(outputFile.good())
    {
        //print results
        for (auto i = result->begin(); i
            != result->end(); i++) {
                outputFile << i->second->getOutput() << '\n';
        }
    }

    //Close the file
    outputFile.close();
}

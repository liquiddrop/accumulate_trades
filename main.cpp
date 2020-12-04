#include "io.c"
#include "asset.c"
#include <iostream>
#include <fstream>
#include <string>           // std::string
#include <locale>           // std::locale, std::tolower
#include <sstream>          // std::stringstream
#include <stdexcept>        // std::runtime_error
#include <algorithm>        // std::max, std::min
#include <map>              // std::map
#include <chrono>           // std::chrono
#include <utility>          // std::pair
#include <memory>           // std::shared_ptr
#include <boost/algorithm/string.hpp>

using namespace std;

//#define debug
//#define debugSpeed

//Function to look for symbol and add new trade
void processForOutput(inputInfo* inputTrade, auto result)
{
    auto iter=result->find(inputTrade->symbol);
    //for trade look in results map if an entry exist
    if (iter == result->end())
    {
        //if it doesn't initialize a new entry to results and fill out
        result->insert(std::make_pair(inputTrade->symbol, std::shared_ptr<asset>(new asset(inputTrade))));
    }
    else{
        //if it exits then process all the new inputs and update values
        iter->second->addTrade(inputTrade);
    }
}

void handleUserInput(std::string userInput)
{
    std::map<std::string, std::shared_ptr<inputInfo>> inputData;
    std::map<std::string, std::shared_ptr<asset>> result;

    if(userInput.length()==4 && userInput.compare("test")==0)
    {
        //run test cases and end program after
        //TODO: make this search the directory and run all test*.csv files then output similar named files
        cout << "testAssetOrder" << endl;
        inputCsvAndProcess("testAssetOrder.csv", &result);
        outputResults("testAssetOrderOutput.csv", &result);
        result.clear();
        cout << "testMaxPrice" << endl;
        inputCsvAndProcess("testMaxPrice.csv", &result);
        outputResults("testMaxPriceOutput.csv", &result);
        result.clear();
        cout << "testTimeGap" << endl;
        inputCsvAndProcess("testTimeGap.csv", &result);
        outputResults("testTimeGapOutput.csv", &result);
        result.clear();
        cout << "testVolume" << endl;
        inputCsvAndProcess("testVolume.csv", &result);
        outputResults("testVolumeOutput.csv", &result);
        result.clear();
        cout << "testWeightedAveragePrice" << endl;
        inputCsvAndProcess("testWeightedAveragePrice.csv", &result);
        outputResults("testWeightedAveragePriceOutput.csv", &result);
        result.clear();
    }
    else{
#ifdef debugSpeed
        auto start = std::chrono::high_resolution_clock::now();
#endif // debugSpeed
        //if you just want to generate the output
        inputCsvAndProcess(userInput, &result);

        //if you want to take in the input and do post processing instead of concurrent
        //inputCsv(userInput, *inputData);

#ifdef debug
        cout << "starting output" << endl;
#endif // debugSpeed
        //print the results to output.csv
        outputResults("output.csv", &result);

#ifdef debugSpeed
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        cout << "Time taken: "
            << duration.count() << " microseconds" << endl;
#endif // debugSpeed

        //cleanup results
        result.clear();
    }
}



//Tests:
//Verify the max time gap where with only 1 input (should be 0), with 2 inputs, 3 inputs, and 10 inputs
//Verify volume traded with only one trade it should be the quantity of the trade, and for multiple trade verify the quantities added up equals volume
//Verify Max trade price with only one trade is the value in the one trade, and for multiple trade verify the max trade price is the highest of all trades
//Verify weighted average price with one trade is the price in the trade, for multiple make sure the value is correct, and verify that fractions get truncated correctly
//Verify that the output is in alphabetical order based on symbol

int main(int argc, char *argv[])
{
    cout << "we are in new code" << endl;
    if(argc==1)
    {
        cout << endl;
        cout << "Valid usage:" << endl;
        cout << "./AccumulateTrades <input file name>" << endl;
        cout << "or" << endl;
        cout << "./AccumulateTrades test" << endl;
        cout << endl;
    }
    if(argc>=2)
    {
        printf("\nNumber Of Arguments Passed: %d",argc);
        printf("\n----Following Are The Command Line Arguments Passed----");
        for(int counter=0;counter<argc;counter++)
            printf("\nargv[%d]: %s",counter,argv[counter]);

        std::string str(argv[1]);
        boost::algorithm::to_lower(str);
        handleUserInput(str);
    }
    return 0;
}

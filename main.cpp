#include "asset.h"
#include "accumulateTrades.h"
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

void handleUserInput(std::string userInput)
{
    accumulateTrades accumulator;

    if(userInput.length()==4 && userInput.compare("test")==0)
    {
        //run test cases and end program after
        //TODO: make this search the directory and run all test*.csv files then output similar named files
	cout << "Running all tests:" << endl;
	cout << "testAssetOrder" << endl;
        accumulator.inputCsvToAsset("testAssetOrder.csv");
        accumulator.outputAssetResults("testAssetOrderOutput.csv");
        accumulator.clearAssets();
        cout << "testMaxPrice" << endl;
        accumulator.inputCsvToAsset("testMaxPrice.csv");
        accumulator.outputAssetResults("testMaxPriceOutput.csv");
        accumulator.clearAssets();
        cout << "testTimeGap" << endl;
        accumulator.inputCsvToAsset("testTimeGap.csv");
        accumulator.outputAssetResults("testTimeGapOutput.csv");
        accumulator.clearAssets();
        cout << "testVolume" << endl;
        accumulator.inputCsvToAsset("testVolume.csv");
        accumulator.outputAssetResults("testVolumeOutput.csv");
        accumulator.clearAssets();
        cout << "testWeightedAveragePrice" << endl;
        accumulator.inputCsvToAsset("testWeightedAveragePrice.csv");
        accumulator.outputAssetResults("testWeightedAveragePriceOutput.csv");
    }
    else{
#ifdef debugSpeed
        auto start = std::chrono::high_resolution_clock::now();
#endif // debugSpeed
        //if you just want to generate the output
        accumulator.inputCsvToAsset(userInput);

        //if you want to take in the input and do post processing instead of concurrent
        //accumulator.inputCsvToTrades(userInput);

#ifdef debug
        cout << "starting output" << endl;
#endif // debug
        //print the results to output.csv
        accumulator.outputAssetResults("output.csv");

#ifdef debugSpeed
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        cout << "Time taken: "
            << duration.count() << " microseconds" << endl;
#endif // debugSpeed
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
        std::string str(argv[1]);
        boost::algorithm::to_lower(str);
        handleUserInput(str);
    }
    return 0;
}

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
#include "boost/lexical_cast.hpp"
#include "boost/fusion/adapted/struct/adapt_struct.hpp"
#include "boost/fusion/include/adapt_struct.hpp"
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace std;

//#define debug
//#define debugSpeed
#define defaultSmaPeriod 10

//input <TimeStamp>,<Symbol>,<Quantity>,<Price>
//To add an input variable just add to struct and boost fusion below
//and it will be inputed from csv to struct
struct inputInfo
{
    unsigned long long timeStamp;
    std::string symbol;
    unsigned int quantity;
    unsigned int price;
};

BOOST_FUSION_ADAPT_STRUCT(
    inputInfo,
    (auto, timeStamp)
    (auto, symbol)
    (auto, quantity)
    (auto, price)
)

//output <symbol>,<MaxTimeGap>,<Volume>,<WeightedAveragePrice>,<MaxPrice>
struct outputInfo{
    std::string symbol;
    unsigned long long maxTimeGap;
    unsigned int volume;
    unsigned int weightedAveragePrice;
    unsigned int maxPrice;
};

BOOST_FUSION_ADAPT_STRUCT(
    outputInfo,
    (auto, symbol)
    (auto, maxTimeGap)
    (auto, volume)
    (auto, weightedAveragePrice)
    (auto, maxPrice)
)

//cast from string to what ever type to input data from getline
template <typename T>
T convert(std::string stringIn) {
    return boost::lexical_cast<T>(stringIn);
}

//input function that takes a string stream and input it into a function
struct inputVar
{
    inputVar(std::stringstream* streamIn)
    {
        m_StringStream=streamIn;
    }

    template<typename T>
    void operator()(T& t) const
    {
        std::string temp;
        //TODO think about handling this error instead of throwing exception
        if(!getline(*m_StringStream, temp, ',')) throw std::runtime_error("Error in input file, probably not enough data to meet expected format");
        t = convert<T>(temp);
    }

    std::stringstream* m_StringStream;
};

//printing of struct function
struct print
{
  template <typename T>
  void operator()(const T &t) const
  {
    std::cout << t << '\n';
  }
};

//output function for stuct to string with csv data
struct outputCsvString
{
    outputCsvString(std::ostringstream* streamOutput)
    {
        m_StringStream=streamOutput;
    }

    template<typename T>
    void operator()(T& t) const
    {
        *m_StringStream << t << ',';
    }

    std::ostringstream* m_StringStream;
};

//Place for all the class templates for calculations that need to happen
template <uint8_t N, class input_t = uint16_t, class sum_t = uint32_t>
class SMA {
  public:
    input_t operator()(input_t input) {
        sum -= previousInputs[index];
        sum += input;
        previousInputs[index] = input;
        if (++index == N)
            index = 0;
        return (sum + (N / 2)) / N;
    }

    static_assert(
        sum_t(0) < sum_t(-1),  // Check that `sum_t` is an unsigned type
        "Error: sum data type should be an unsigned integer, otherwise, "
        "the rounding operation in the return statement is invalid.");

  private:
    uint8_t index             = 0;
    input_t previousInputs[N] = {};
    sum_t sum                 = 0;
};

//class to handle each symbol
class asset {
    public:
        asset();
        asset(inputInfo* firstTrade);
        ~asset();
        void addTrade(inputInfo* nextTrade);
        auto getOutput();

    private:
        std::string symbol;
        unsigned long long lastTradeTime;
        unsigned long long maxTimeGap;
        unsigned int volume;
        unsigned int averageVol;
        unsigned int numTrades;
        unsigned int weightedTotalPrice;
        unsigned int weightedAveragePrice;
        unsigned int maxPrice;
        unsigned int minPrice;
        unsigned int simpleMovingAverage;
        SMA<defaultSmaPeriod> movingSma;
};

asset::asset()
{
    lastTradeTime=0;
    maxTimeGap=0;
    volume=0;
    numTrades=0;
    weightedTotalPrice=0;
    weightedAveragePrice=0;
    maxPrice=0;
    minPrice=0;
    simpleMovingAverage=0;
}

asset::asset(inputInfo* firstTrade)
{
    //initialize everything
    symbol=firstTrade->symbol;
    lastTradeTime=firstTrade->timeStamp;
    maxTimeGap=0;
    volume=firstTrade->quantity;
    numTrades=1;
    weightedTotalPrice=firstTrade->price * firstTrade->quantity;
    maxPrice=firstTrade->price;
    minPrice=firstTrade->price;
    simpleMovingAverage=movingSma(firstTrade->price);
}

asset::~asset()
{

}

void asset::addTrade(inputInfo* nextTrade)
{
    //need to include this because I allow a constructor without input
    symbol=nextTrade->symbol;
    //add info and calculate was is needed
    auto timeGap = nextTrade->timeStamp - lastTradeTime;
    maxTimeGap=max(timeGap, maxTimeGap);
    lastTradeTime=nextTrade->timeStamp;
    volume+=nextTrade->quantity;
    numTrades++;
    weightedTotalPrice+=(nextTrade->price*nextTrade->quantity);
    maxPrice=max(nextTrade->price, maxPrice);
    minPrice=min(nextTrade->price, minPrice);
    simpleMovingAverage=movingSma(nextTrade->price);
}

//function to return the rolling output of the symbol
//this should return a csv string of the output struct
auto asset::getOutput()
{
    std::string csvOutput;
    std::ostringstream oss;
    /*
    outputInfo
        std::string symbol;
        unsigned long long maxTimeGap;
        unsigned int volume;
        unsigned int weightedAveragePrice;
        unsigned int maxPrice;
    */
    struct outputInfo generatedOutput = {symbol, maxTimeGap, volume, (weightedTotalPrice/volume), maxPrice};

    boost::fusion::for_each(generatedOutput, outputCsvString(&oss));

    csvOutput = oss.str();

    //remove the last char which is an extra ','
    csvOutput.pop_back();

    return csvOutput;
}

//Function to look for symbol and add new trade
void processForOutput(inputInfo* inputTrade, auto result)
{
    auto iter=result->find(inputTrade->symbol);
    //for trade look in results map if an entry exist
    if (iter == result->end())
    {
        //if it doesn't initialize a new entry to results and fill out
        result->insert(std::make_pair(inputTrade->symbol, new asset(inputTrade)));
    }
    else{
        //if it exits then process all the new inputs and update values
        iter->second->addTrade(inputTrade);
    }
}

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

//clean up malloc'ed assets
void clearResults( auto result)
{
    //delete assets in results
    for (auto i = result->begin(); i
        != result->end(); i++) {
            delete i->second;
    }
    //clear map
    result->clear();
}


//Tests:
//Verify the max time gap where with only 1 input (should be 0), with 2 inputs, 3 inputs, and 10 inputs
//Verify volume traded with only one trade it should be the quantity of the trade, and for multiple trade verify the quantities added up equals volume
//Verify Max trade price with only one trade is the value in the one trade, and for multiple trade verify the max trade price is the highest of all trades
//Verify weighted average price with one trade is the price in the trade, for multiple make sure the value is correct, and verify that fractions get truncated correctly
//Verify that the output is in alphabetical order based on symbol

int main()
{
    std::string inputString;
    std::locale loc;
    std::map<std::string, inputInfo*> inputData;
    std::map<std::string, asset*> result;

    while(1)
    {
        cout << "Please provide input file name or enter Q to quit:" << endl;
        getline(cin, inputString);
        //Quit the program
        if(inputString.length()==1 && std::tolower(inputString[0],loc)=='q' )
        {
            break;
        }
        if((inputString.length()==1 && std::tolower(inputString[0],loc)=='h') || inputString.length()==0 )
        {
            cout << endl;
            cout << "Valid instructions:" << endl;
            cout << "h - open help menu" << endl;
            cout << "t - run predetermined tests" << endl;
            cout << "<input csv file name> - run program on input file example: input.csv" << endl;
            cout << "q - quit program" << endl;
            cout << endl;
            continue;
        }
        if(inputString.length()==1 && std::tolower(inputString[0],loc)=='t' )
        {
            //run test cases and end program after
            //TODO: make this search the directory and run all test*.csv files then output similar named files
            cout << "testAssetOrder" << endl;
            inputCsvAndProcess("testAssetOrder.csv", &result);
            outputResults("testAssetOrderOutput.csv", &result);
            clearResults(&result);
            cout << "testMaxPrice" << endl;
            inputCsvAndProcess("testMaxPrice.csv", &result);
            outputResults("testMaxPriceOutput.csv", &result);
            clearResults(&result);
            cout << "testTimeGap" << endl;
            inputCsvAndProcess("testTimeGap.csv", &result);
            outputResults("testTimeGapOutput.csv", &result);
            clearResults(&result);
            cout << "testVolume" << endl;
            inputCsvAndProcess("testVolume.csv", &result);
            outputResults("testVolumeOutput.csv", &result);
            clearResults(&result);
            cout << "testWeightedAveragePrice" << endl;
            inputCsvAndProcess("testWeightedAveragePrice.csv", &result);
            outputResults("testWeightedAveragePriceOutput.csv", &result);
            clearResults(&result);
            continue;
        }
#ifdef debugSpeed
        auto start = std::chrono::high_resolution_clock::now();
#endif // debugSpeed
        //if you just want to generate the output
        inputCsvAndProcess(inputString, &result);

        //if you want to take in the input and do post processing instead of concurrent
        //inputCsv(inputString, *inputData);

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
        clearResults(&result);
    }
    return 0;
}

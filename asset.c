#include "asset.h"


using namespace std;

asset::asset()
{
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
std::string asset::getOutput()
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

    boost::fusion::for_each(generatedOutput, outputAssetToCsvString(&oss));

    csvOutput = oss.str();

    //remove the last char which is an extra ','
    csvOutput.pop_back();

    return csvOutput;
}

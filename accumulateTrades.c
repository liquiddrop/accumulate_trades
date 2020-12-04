#include "accumulateTrades.h"

using namespace std;

//Public functions

accumulateTrades::accumulateTrades()
{
}

accumulateTrades::~accumulateTrades()
{
}

void accumulateTrades::inputCsvToAsset(std::string inputFileName)
{
	inputCsv(inputFileName, &assetResults);
}

void accumulateTrades::inputCsvToTrades(std::string inputFileName)
{
	inputCsv(inputFileName, &tradeData);
}

void accumulateTrades::clearAssets()
{
	assetResults.clear();
}

void accumulateTrades::clearTrades()
{
	tradeData.clear();
}

//output results to csv file
void accumulateTrades::outputAssetResults(std::string outputFileName)
{
    //init and open file
    std::ofstream outputFile(outputFileName);

    //make sure it is open
    if(!outputFile.is_open()) throw std::runtime_error("Could not open output file");
    //make sure stream is good
    if(outputFile.good())
    {
        //print results
        for (auto i = assetResults.begin(); i
            != assetResults.end(); i++) {
                outputFile << i->second->getOutput() << '\n';
        }
    }

    //Close the file
    outputFile.close();
}

//Private functions


//Function to insert trade into asset
void accumulateTrades::insertToMap(inputInfo* inputTrade, std::map<std::string,std::shared_ptr<asset>>* assetMap)
{
    auto iter=assetMap->find(inputTrade->symbol);
    //for trade look in the asset map if an entry exist
    if (iter == assetMap->end())
    {
        //if it doesn't initialize a new entry to results and fill out
        assetMap->insert(std::make_pair(inputTrade->symbol, std::shared_ptr<asset>(new asset(inputTrade))));
    }
    else{
        //if it exits then process all the new inputs and update values
        iter->second->addTrade(inputTrade);
    }
}

//Function to insert trade into map of trades
void accumulateTrades::insertToMap(inputInfo* inputTrade, std::map<std::string, std::shared_ptr<inputInfo>>* tradeMap)
{
    //add trade to map
    tradeMap->insert(std::make_pair(inputTrade->symbol, std::shared_ptr<inputInfo>(new inputInfo(*inputTrade))));
}

//input csv file to map 
void accumulateTrades::inputCsv(std::string inputFileName, auto map)
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

            insertToMap(&tempInput, map);
        }
    }
}

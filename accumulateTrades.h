#include "io.h"
#include "asset.h"
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

#ifndef ACCUMULATE_TRADES_H
#define ACCUMULATE_TRADES_H

//class to accumulate trades into either a map of assets or trades(inputInfo's) 
class accumulateTrades {
    public:
        accumulateTrades();
        ~accumulateTrades();
	void inputCsvToAsset(std::string inputFileName);
	void inputCsvToTrades(std::string inputFileName);
	void clearAssets();
	void clearTrades();
	void outputAssetResults(std::string outputFileName);

    private:
    	std::map<std::string, std::shared_ptr<inputInfo>> tradeData;
    	std::map<std::string, std::shared_ptr<asset>> assetResults;
	void insertToMap(inputInfo* inputTrade, std::map<std::string,std::shared_ptr<asset>>* assetMap);
	void insertToMap(inputInfo* inputTrade, std::map<std::string, std::shared_ptr<inputInfo>>* tradeMap);
	void inputCsv(std::string inputFileName, auto map);
};

#endif

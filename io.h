#include "boost/lexical_cast.hpp"
#include "boost/fusion/adapted/struct/adapt_struct.hpp"
#include "boost/fusion/include/adapt_struct.hpp"
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
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

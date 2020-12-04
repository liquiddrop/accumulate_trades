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

#define defaultSmaPeriod 10

//output function for stuct to string with csv data
struct outputAssetToCsvString
{
    outputAssetToCsvString(std::ostringstream* streamOutput)
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

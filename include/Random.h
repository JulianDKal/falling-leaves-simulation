#pragma once
#include <random>
#include <iostream>

class Random {
public: 
    Random() = delete;
    static float Float() {
        // return (float) distribution(randomEngine) / (float) std::numeric_limits<std::mt19937::result_type>::max();
        if(!initialized) {
            std::cout << "Run Random::Init() before calling this method!" << std::endl;
            return 0.0f;
        }
        return static_cast<float>(rDistribution(randomEngine));
    }
    static void Init() {
        //std::random_device()() calls the () operator on the random_device class, is equivalent to
        //std::random_device rd;
        //unsigned int randomNum = rd();
        //seed(randomNum)
        randomEngine.seed(std::random_device()());
        initialized = true;
    }
private: 
    //If we wouldn't use the inline keyword, we would have to define these member variables in another .cpp file
    //because now they are only declared (but with inline you can define variables inside of a header file)
    inline static bool initialized = false;
    inline static std::mt19937 randomEngine;
    inline static std::uniform_real_distribution<float> rDistribution {0.0f, 1.0f};

};


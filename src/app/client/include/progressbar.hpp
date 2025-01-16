#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include <iostream>
#include <string>
#include "./constants.hpp"

class ProgressBar {
private:
    int totalSteps;
    int barWidth;
    char barChar;
    char emptyChar;
    int currentStep;

public:
    // Constructor
    ProgressBar(
        int totalSteps = ClientConstants::PROGRESS_BAR_TOTAL_STEPS,
        int barWidth = ClientConstants::PROGRESS_BAR_WIDTH,
        char barChar = ClientConstants::PROGRESS_BAR_CHAR,
        char emptyChar = ClientConstants::PROGRESS_BAR_EMPTY_CHAR
    );

    // Member functions
    void print();
    void increment();
    void update(int currentStep);
};

#endif // PROGRESSBAR_HPP
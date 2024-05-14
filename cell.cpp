#include "solver.h"

#include <iostream>
#include <cstdlib>


Cell::Cell() {
    this->reset();
}


void Cell::reset() {
    std::bitset<GRIDSIZE> temp;
    temp.flip();
    this->setOptions(temp);
    this->val = 0;
}


bool Cell::isCollapsed() {
    return this->val != 0;
}


int Cell::getVal() {
    return this->val;
}


int Cell::numOptions() {
    return this->options.size();
}


void Cell::setOptions(std::bitset<GRIDSIZE> optionsSet) {
    for (int i = 0; i < GRIDSIZE; i++) {
        if (optionsSet[i]) {
            this->options.insert(i + 1);
        }
    }
}


void Cell::removeOption(int val) {
    this->options.erase(val);
}


void Cell::collapse(int val) {
    /// Collapse the cell to the value specified

    if (val < 1 || val > GRIDSIZE) {
        std::cout << "Collapsing cell to invalid value " << val << "!\n";
    }

    this->val = val;
    this->options.clear();
}


int Cell::collapse() {
    /// Collapse the cell to a random value from its options
    /// Assumes there are a non-zero number of options from which to choose from

    int optionChoiceIndex = std::rand() % this->options.size();
    auto it = options.begin();
    int i;
    for (i = 0; i < optionChoiceIndex; i++) {
        it++;
    }

    //std::wcout << "Chose option " << i << ": Collapsing cell to " << *it << ".\n";

    this->collapse(*it);
    return *it;
}


void Cell::operator=(const Cell &other) {
    this->val = other.val;
    this->options = other.options;
}


void Cell::operator=(const int &val) {
    this->val = val;
}


constexpr bool Cell::operator==(const int &val) {
    return this->val == val;
}
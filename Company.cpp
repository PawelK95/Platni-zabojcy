#include <string>
#include <iostream>
#include "Company.h"
#include "Parameters.h"

Company::Company() {
    this->reputation = 0;
    this->numberOfKillers = KILLERS;
}

Company::Company(int reputation, int numberOfKillers) {
    this->reputation = reputation;
    this->numberOfKillers = numberOfKillers;
}

void Company::addReputation(int points) {
    this->reputation += points;
}

void Company::sendKiller() {
    this->numberOfKillers --;
}

void Company::killerEndedWork() {
    this->numberOfKillers ++;
}
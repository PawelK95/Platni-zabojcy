#ifndef PLATNI_ZABOJCY_ASSASIN_H
#define PLATNI_ZABOJCY_ASSASIN_H

#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <mutex>
#include "Lamport.h"
#include "Company.h"
#include "Parameters.h"

enum State {
    IDLE,
    WAITNG_FOR_ASSASINATION,
    REST,
    WANT_TO_JOIN_QUEUE,
    IN_QUEUE,
    AFTER_ASSASINATION
};


class Client {
    int lamportClockOnRequest;
    int selectedCompany;
    int answerCount;
    int currentInCompany;

    std::mutex * listOfQueuesMutex;
    std::mutex * listOfCompaniesMutex;
    Company listOfCompanies[COMPANIES];
    std::vector<int> listOfQueues[COMPANIES];

    State state;
    Lamport *lamport;

public:
    int choseCompany(Company listOfCompanies[], std::vector<int> listOfQueues[]);
    int choseBetterCompany(Company listOfCompanies[], std::vector<int> listOfQueues[]);
    int findPositionQueue();
    void handleInCompany();
    void askForCompany();
    void handleAfterMurder();
    void handleMsg();
    void doStuff();
    void handleInQueue();
    void handleAnswerWantToJoinQueue(Message m);
    void handleWantToJoinQueue(Message m);
    void checkForBetterOption();
    Client(int size, int rank);
    void clientLoop();

};

#endif

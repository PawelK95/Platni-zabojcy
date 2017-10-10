#ifndef PLATNI_ZABOJCY_COMPANY_H
#define PLATNI_ZABOJCY_COMPANY_H

class Company {

public:
    int reputation;
    int numberOfKillers;
    void addReputation(int points);
    void killerEndedWork();
    void sendKiller();
    Company();
    Company(int reputation, int numberOfKillers);

};


#endif

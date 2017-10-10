#ifndef PLATNI_ZABOJCY_LAMPORT_H
#define PLATNI_ZABOJCY_LAMPORT_H
#include <string>
#include <iostream>
#include <mpi.h>
using std::string;

enum MessageTag {
    WantToChangeQueue,
    AfterMurder,
    InQueue,
    InCompany,
    AnswerWantToJoinQueue,
    WantToJoinQueue
};

struct Message {
    int oldCompanyId;
    bool answer;
    int companyId;
    int points;
    int processId;
    int lamportClock;
};


class Lamport {

protected:
    int timestamp;

public:
    int size;
    int rank;
    Lamport(int size, int rank);
    void broadcast(Message message, MessageTag tag);
    void increment();
    int getTimestamp();
    void sendMessage(int to, Message message, MessageTag tag);
    void setMax(int timestamp);


protected:
    void send(int to, Message message, MessageTag tag);
};


#endif

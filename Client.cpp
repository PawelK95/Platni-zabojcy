#include "Client.h"
#include "Company.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <zconf.h>
#include <algorithm>
#include <thread>



Client::Client(int size, int rank) {
    this->lamport = new Lamport(size, rank);
    this->answerCount = size-1;
    this->selectedCompany = -1;
    this->currentInCompany = -1;
    this->state = IDLE;
    this->lamportClockOnRequest = -1;
    this->listOfCompaniesMutex = new std::mutex();
    this->listOfQueuesMutex = new std::mutex();
}

void handleMessage(Client * msg){
    while(1){
        msg->handleMsg();
        sleep(1);
    }
}

void handleStuff(Client * msg){
    while(1){
        msg->doStuff();
        sleep(1);
    }
}



void Client::clientLoop() {
    std::thread t1(handleMessage,this);
    std::thread t2(handleStuff,this);
    t1.join();
    t2.join();
    }



void Client::doStuff() {
    switch (this->state) {
        case WANT_TO_JOIN_QUEUE: {
            if (this->answerCount == 0) {
                handleInQueue();
                this->answerCount = - 1;
                this->state = IN_QUEUE;
            }
            break;
        }
        case IN_QUEUE: {
                int id = this->currentInCompany;
                this->selectedCompany = -1;
                if ((this->listOfCompanies[id].numberOfKillers != 0) &&
                    this->lamport->rank == this->listOfQueues[id].front()) {
                    this->state = WAITNG_FOR_ASSASINATION;
                    handleInCompany();
                    break;
                }
            checkForBetterOption();
            break;
        }
        case WAITNG_FOR_ASSASINATION: {
                sleep(random() % 5);
                this->state = AFTER_ASSASINATION;
            break;
        }
        case AFTER_ASSASINATION: {
            this->state = REST;
            handleAfterMurder();
            break;
        }
        case REST: {
                sleep(random()%5);
                this->state = IDLE;
            break;
        }
        case IDLE: {
                askForCompany();
            break;
        }


    }
}

void Client::askForCompany() {
    Message msg={};
    listOfCompaniesMutex->lock();
    listOfQueuesMutex->lock();
    this->selectedCompany = this->choseCompany(this->listOfCompanies, this->listOfQueues);
    listOfQueuesMutex->unlock();
    listOfCompaniesMutex->unlock();
    msg.companyId = this->selectedCompany;
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    this->state = WANT_TO_JOIN_QUEUE;
    lamport->broadcast(msg, MessageTag(WantToJoinQueue));
    this->lamportClockOnRequest = this->lamport->getTimestamp();
    this->answerCount = lamport->size - 1;

    printf("%d: \t %d --> broadcast - Klient chce wejsc do kolejki komapni o id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank,  msg.companyId);

}

int Client::choseCompany(Company listOfCompanies[], std::vector<int> listOfQueues[]) {
    int id = 0;
    int maxPoints =  listOfCompanies[0].reputation - listOfQueues[0].size();
    for(int i=0;i<COMPANIES;i++){
        int companyPoints = listOfCompanies[i].reputation - 3*(listOfQueues[i].size());
        if((companyPoints) > maxPoints){
            maxPoints=companyPoints;
            id = i;
        }
    }
    return id;
}

int Client::choseBetterCompany(Company listOfCompanies[], std::vector<int> listOfQueues[]) {
    int id = 0;
    int maxPoints =  listOfCompanies[this->currentInCompany].reputation - 3*(findPositionQueue());
    for(int i=0;i<COMPANIES;i++){
        int companyPoints = 0;
        if(i != this->currentInCompany){
            companyPoints = listOfCompanies[i].reputation - 3*(listOfQueues[i].size());
        }
        if((companyPoints) > maxPoints){
            maxPoints = companyPoints;
            id = i;
        }
    }
    return id;
}
int Client::findPositionQueue () {
    for(int i=0; i< this->listOfQueues[this->currentInCompany].size(); i++){
        if(this->lamport->rank == this->listOfQueues[this->currentInCompany][i]){
            return i;
        }
    }
    return -1;
}

void Client::checkForBetterOption() {
    listOfQueuesMutex->lock();
    listOfCompaniesMutex->lock();
    int id = this->choseBetterCompany(this->listOfCompanies, this->listOfQueues);
    listOfCompaniesMutex->unlock();
    listOfQueuesMutex->unlock();
    if(id != this->currentInCompany){
        Message msg={};
        this->state = WANT_TO_JOIN_QUEUE;
        this->selectedCompany=id;
        msg.companyId = this->selectedCompany;
        msg.lamportClock = this->lamport->getTimestamp();
        msg.processId = this->lamport->rank;
        lamport->broadcast(msg, MessageTag(WantToChangeQueue));
        this->answerCount = lamport->size - 1;
        listOfQueuesMutex->lock();
        this->listOfQueues[msg.companyId]
                .erase(std::remove(
                        this->listOfQueues[msg.companyId].begin(),
                        this->listOfQueues[msg.companyId].end(),
                        msg.processId),
                     this->listOfQueues[msg.companyId].end());
        listOfQueuesMutex->unlock();
        printf("%d: \t %d --> broadcast - Klient rezygnuje z kompani o id: %d  i chce wejsc do kolejki komapni o id: %d\n",
               this->lamport->getTimestamp(), this->lamport->rank, msg.oldCompanyId, msg.companyId);
    }
}

void Client::handleInQueue() {
    Message msg={};
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    msg.companyId = this->selectedCompany;
    this->currentInCompany =this->selectedCompany;
    listOfQueuesMutex->lock();
    this->listOfQueues[msg.companyId].push_back(msg.processId);
    listOfQueuesMutex->unlock();

    lamport->broadcast(msg, MessageTag(InQueue));
    printf("%d: \t %d --> broadcast - Klient wszedl do kolejki kompani o id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank,  msg.companyId);
}

void Client::handleInCompany() {
    Message msg={};
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    msg.companyId = this->currentInCompany;

    listOfCompaniesMutex->lock();
    this->listOfCompanies[msg.companyId].sendKiller();
    listOfCompaniesMutex->unlock();
    listOfQueuesMutex->lock();
    this->listOfQueues[msg.companyId].erase(listOfQueues[msg.companyId].begin());
    listOfQueuesMutex->unlock();

    lamport->broadcast(msg, MessageTag(InCompany));
    printf("%d: \t %d --> broadcast - Klient wszedl do komapni o id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank,  msg.companyId);
}

void Client::handleAfterMurder() {
    Message msg={};
    msg.lamportClock = this->lamport->getTimestamp();
    msg.processId = this->lamport->rank;
    msg.companyId = this->currentInCompany;
    msg.points = rand() % 3;
    lamport->broadcast(msg, MessageTag(AfterMurder));

    listOfCompaniesMutex->lock();
    this->listOfCompanies[msg.companyId].addReputation(msg.points);
    this->listOfCompanies[msg.companyId].killerEndedWork();
    listOfCompaniesMutex->unlock();
    this->currentInCompany = -1;
    printf("%d: \t %d --> broadcast - Klient wyszedl z komapni o id: %d\n",
           this->lamport->getTimestamp(), this->lamport->rank,  msg.companyId);
    this->selectedCompany=-1;
}

void Client::handleWantToJoinQueue(Message msg){
    Message m;
    m.processId = this->lamport->rank;
    m.companyId = msg.companyId;
    if(this->lamportClockOnRequest < m.lamportClock && this->selectedCompany == msg.companyId) {
        m.answer = false;
    } else
    if(this->state == WANT_TO_JOIN_QUEUE
       && this->selectedCompany == msg.companyId
       && this->lamport->rank < msg.processId ) {
        m.answer = false;
    } else {
        m.answer = true;
    }
    lamport->sendMessage(msg.processId, m, MessageTag(AnswerWantToJoinQueue));
    printf("%d: \t %d --> %d Klient wysyła Answer, że jest na %d \n",
           this->lamport->getTimestamp(), this->lamport->rank, msg.processId,  m.answer);

}

void Client::handleAnswerWantToJoinQueue(Message msg){
    if(msg.answer && msg.lamportClock > this->lamportClockOnRequest ){
        this->answerCount--;
    } else {
        this->state = IDLE;
    }
}


void Client::handleMsg() {


    int flag;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if(flag) {

        Message m;


        MPI_Recv(&m, sizeof(Message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        this->lamport->setMax(m.lamportClock);

        switch (status.MPI_TAG) {
            case WantToJoinQueue: {
                printf("%d: \t %d <--  %d - Klient prosi o zgode bo chce wejsc do kolejki komapni o id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.companyId);
                handleWantToJoinQueue(m);
                break;
            }
            case WantToChangeQueue: {
                printf("%d: \t %d <--  %d - Klient zmienil decyzje z kompani o id: %d i prosi o zgode bo chce wejsc do kolejki komapni o id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.oldCompanyId, m.companyId);
                listOfQueuesMutex->lock();
                this->listOfQueues[m.companyId]
                        .erase(std::remove(
                                this->listOfQueues[m.companyId].begin(),
                                this->listOfQueues[m.companyId].end(),
                                m.processId),
                               this->listOfQueues[m.companyId].end());
                listOfQueuesMutex->unlock();
                handleWantToJoinQueue(m);
                break;
            }
            case InCompany: {
                printf("%d: \t %d <--  %d - Otrzymalem informacje ze klient wszedl do komapni o id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.companyId);
                listOfCompaniesMutex->lock();
                this->listOfCompanies[m.companyId].sendKiller();
                listOfCompaniesMutex->unlock();

                listOfQueuesMutex->lock();
                this->listOfQueues[m.companyId].erase(listOfQueues[m.companyId].begin());
                listOfQueuesMutex->unlock();
                break;
            }
            case InQueue: {
                printf("%d: \t %d <--  %d - Otrzymalem informacje ze klient jest w kolejce do komapni o id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.companyId);
                this->listOfQueues[m.companyId].push_back(m.processId);
                break;
            }
            case AfterMurder: {
                printf("%d: \t %d <--  %d - Otrzymalem informacje ze klient zakonczyl sprawy w komapni o id: %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.companyId);
                listOfCompaniesMutex->lock();
                this->listOfCompanies[m.companyId].addReputation(m.points);
                this->listOfCompanies[m.companyId].killerEndedWork();
                listOfCompaniesMutex->unlock();
                break;
            }
            case AnswerWantToJoinQueue :{
                printf("%d: \t %d <--  %d - Answer od procesu ,że jest na %d\n",
                       this->lamport->getTimestamp(), this->lamport->rank, m.processId, m.answer);
                handleAnswerWantToJoinQueue(m);
                break;
            }
        }

    }
}



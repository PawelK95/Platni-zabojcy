#include "Lamport.h"


Lamport::Lamport(int size, int rank) {
    this->size = size;
    this->rank = rank;
    this->timestamp = 0;
}

void Lamport::broadcast(Message message, MessageTag tag) {
    this->increment();
    message.lamportClock = this->getTimestamp();
    for(int i = 0; i < size; i++) {
        if (i != rank) {
            this->send(i, message, tag);
        }
    }
}

void Lamport::increment() {
    this->timestamp++;
}
int Lamport::getTimestamp() {
    return this->timestamp;
}

void Lamport::setMax(int timestamp) {
    this->timestamp = std::max(this->timestamp, timestamp);
    this->increment();
}

void Lamport::sendMessage(int to, Message message, MessageTag tag) {

    this->increment();
    message.lamportClock = this->getTimestamp();
    send(to, message, tag);
}

void Lamport::send(int to, Message message, MessageTag tag) {
    MPI_Send(&message, sizeof(message), MPI_BYTE, to, tag, MPI_COMM_WORLD);
}



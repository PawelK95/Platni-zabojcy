#include <iostream>
#include <mpi.h>
#include "Lamport.h"
#include "Client.h"


int main(int argc, char **argv) {
    int size, rank;

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Barrier(MPI_COMM_WORLD);
    srand((unsigned)time(NULL));


    Client client = Client(size, rank);
    client.clientLoop();

    MPI_Finalize();


    return 0;
}
/*
 * HelloSerial.cpp
 *
 *  Created on: Oct 7, 2020
 *      Author: ITCS 3145 Q.
 */
#include "mpi.h"
#include <stdio.h>
#include <iostream>
using namespace std;

class HelloSerial {
public:
	void greetingsMPI(int argc, char* argv[]);

	void greetings(int argc, char* argv[]);
	void greetings();



};

//Part 2

void HelloSerial::greetingsMPI(int argc, char* argv[]) {
	int rank;
	int size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int dest = 0, source = 0, tag = 1;
	MPI_Status status;
	char message[100];

	//7.a
	if (rank == 0) {
		int msg = 1;
		source = 1;
		
		std::cout << "7.a: " << size << " : " << rank << "\n";
		while (source <= size - 1) {
			MPI_Recv(message, sizeof(message), MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
			std::cout << "7.a: " << message << "\n";
			source++;
		}
	}
	else {
		int msg = 0;
		sprintf_s(message, "%i : %i ", size, rank);
		MPI_Send(message, sizeof(message), MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	//7.b
	if (rank == 0) {
		int msg = 1;
		source = 1;

		std::cout << "\n7.b: " << size << " : " << rank << "\n";
		while (source <= size - 1) {
			MPI_Recv(message, sizeof(message), MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
			std::cout << "7.b: " << message << "\n";
			source++;
		}
	}
	else {
		int msg = 0;
		sprintf_s(message, "%i : %i ", size, rank);
		MPI_Send(message, sizeof(message), MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//7.b
	if (rank == 0) {
		int msg = 1;
		source = 1;

		std::cout << "\n7.c: " << size << " : " << rank << "\n";
		while (source <= size - 1) {
			MPI_Recv(message, sizeof(message), MPI_CHAR, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			std::cout << "7.c: " << message << "\n";
			source++;
		}
	}
	else {
		int msg = 0;
		int tag = rand() % 100 / rank;
		sprintf_s(message, "%i : %i : %i", size, rank, tag);
		MPI_Send(message, sizeof(message), MPI_CHAR, dest, tag , MPI_COMM_WORLD);
	}

	std::cout << "Have a great day!" << "\n";
	MPI_Finalize();
}

void HelloSerial::greetings(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++)
		cout << argv[i] << " ";

	cout << endl;
}

void HelloSerial::greetings() {
	cout << "Have a great day!" << endl;
}

//Part 3

int main(int argc, char* argv[]) {
	HelloSerial hello;
	hello.greetingsMPI(argc, argv);

	return 0;
}


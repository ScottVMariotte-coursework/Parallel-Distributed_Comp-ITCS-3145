#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
using namespace std;

class PI {
public:
	double computingPiSerial(int n);
	double computingPiSection(int start, int end);
	double computingPiByCollectiveMPI(int n);
	double computingPiByPTP(int n);

	double computingPiByPTPLinear(int n);

	double computPIParallel(int n, int rank, int p);
	void getFirstLast(int &first, int &last, int rank,int, int np);
};

int main(int argc, char* argv[]) {
	int n;
	//cout << "Please type an integer: " << flush;
	//scanf("%d", &n);
	n = (int)strtol(argv[1], NULL, 10);
	n = 10000000;

	PI pi;
	MPI_Init(&argc, &argv);
	int rank;
	int p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	double sum;
	float start = clock();
	if(!rank) sum = pi.computingPiSerial(n);
	float end = clock();

	int timeSerial = end - start;

	if (!rank)printf("(Serial) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << "\n";
	MPI_Barrier(MPI_COMM_WORLD);

	start = clock();
	sum = pi.computingPiByPTP(n);
	end = clock();

	double speed = timeSerial/(end-start);

	if (!rank)printf("(Tree) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << " Speed : " << speed << "Eff : " << speed/ p << "\n";
	MPI_Barrier(MPI_COMM_WORLD);

	start = clock();
	sum = pi.computingPiByCollectiveMPI(n);
	end = clock();
	speed = timeSerial / (end - start);

	if (!rank)printf("(Collective) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << " Speed : " << speed << "Eff : " << speed / p << "\n";
	MPI_Barrier(MPI_COMM_WORLD);
	
	start = clock();
	sum = pi.computingPiByPTPLinear(n);
	end = clock();
	speed = timeSerial / (end - start);

	if (!rank)printf("(Linear) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << " Speed : " << speed << "Eff : " << speed / p << "\n";
	MPI_Barrier(MPI_COMM_WORLD); 


	if (!rank) std::cout << "--------100000000-----------";
	n = 100000000;

	start = clock();
	if (!rank) sum = pi.computingPiSerial(n);
	end = clock();

	timeSerial = end - start;

	if (!rank)printf("(Serial) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << "\n";
	MPI_Barrier(MPI_COMM_WORLD);

	start = clock();
	sum = pi.computingPiByPTP(n);
	end = clock();
	speed = timeSerial / (end - start);

	if (!rank)printf("(Tree) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << " Speed : " << speed << "Eff : " << speed / p << "\n";
	MPI_Barrier(MPI_COMM_WORLD);

	start = clock();
	sum = pi.computingPiByCollectiveMPI(n);
	end = clock();
	speed = timeSerial / (end - start);

	if (!rank)printf("(Collective) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << " Speed : " << speed << "Eff : " << speed / p << "\n";
	MPI_Barrier(MPI_COMM_WORLD);

	start = clock();
	sum = pi.computingPiByPTPLinear(n);
	end = clock();
	speed = timeSerial / (end - start);
	if (!rank)printf("(Linear) Pi is equal to %f\n", sum);
	if (!rank) std::cout << "Time : " << end - start << " Speed : " << speed << "Eff : " << speed / p << "\n";
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	
	

	MPI_Finalize();

	return EXIT_SUCCESS;
}

//Tree
double PI::computingPiByPTP(int n) {
	int rank;
	int p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	double sum = computPIParallel(n, rank, p);

	//Tree
	int count = 0;
	int stride = 1;
	while (stride < p) {
		if (rank % stride == 0) {
			if (rank % (2 * stride) == 0) {
				if (rank + stride > p - 1) { stride *= 2; count++;  continue; }
				MPI_Status status;
				double inSum = 0;

				//std::cout << count << ":Rank " << rank << " Waiting on " << rank + stride<<"\n";
				MPI_Recv(&inSum, sizeof(inSum), MPI_DOUBLE, rank + stride, 1, MPI_COMM_WORLD, &status);
				sum += inSum;
			}
			else {
				//std::cout << count  << ":Rank " << rank << " sending to " << rank - stride << "\n";
				MPI_Send(&sum, sizeof(sum), MPI_DOUBLE, rank - stride, 1, MPI_COMM_WORLD);
			}
		}
		stride *= 2;
		count++;
	}

	if (!rank)return sum;
}
/*

*/
double PI::computingPiByPTPLinear(int n) {
	int rank;
	int p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	double sum = computPIParallel(n, rank, p);

	MPI_Status status;
	if (!rank) {
		for (int i = 1; i < p; i ++) {
			double inSum = 0;
			MPI_Recv(&inSum, sizeof(inSum), MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
			sum += inSum;
		}
		return sum;
	}
	else {
		MPI_Send(&sum, sizeof(sum), MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
	}
}

//Collective
double PI::computingPiByCollectiveMPI(int n) {
	int rank;
	int p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	double sum = computPIParallel(n, rank, p);

	double global_sum;
	MPI_Reduce(&sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (!rank)return global_sum;
}

//Helper
double PI::computPIParallel(int n, int rank, int p) {
	int first_i, last_i;
	getFirstLast(first_i, last_i, rank, p, n);

	return computingPiSection(first_i, last_i);
}

//Helper
void PI::getFirstLast(int &first, int &last, int rank,int p ,int n) {
	if (n+1 % p == 0) {
		int block_size = (n+1) / p;
		first = block_size * rank;
		last = first + block_size - 1;
	}
	else {
		int block_size = n / p;
		first = (block_size * rank) + rank;
		last = first + block_size;
		//I dont like if statements	>> if last_i < end then last_i = last_i else = end
		last = ((last < n) * last) + ((last >= n) * n);
	}
}

//PiCalc
double PI::computingPiSection(int start, int end) {
	long double sum = 0;
	double factor;
	for (long i = start; i < end; i++) {
		factor = (i % 2 == 0) + ((i % 2 != 0) * -1);
		sum = sum + 4 * factor / (2 * i + 1);
	}
	return sum;
}

double PI::computingPiSerial(int n) {
	double sum = 0;
	double factor;

	for (long i = 0; i < n; i++) {
		factor = (i % 2 == 0) + ((i % 2 != 0) * -1);
		sum = sum + 4 * factor / (2 * i + 1);
	}
	return sum;
}



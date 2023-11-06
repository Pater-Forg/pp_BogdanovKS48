#include <iostream>
#include <cstdlib>
#include <numeric>
#include "mpi.h"

const int punches = 3;
const int min_damage = 10;
const int max_damage = 50;

int rand_int(int min, int max) {
	return rand() % (max - min + 1) + min;
}

void mpi_boxer(int id, int enemy_id) {
	MPI_Status status;
	int recvd_damage = 0;
	for (int i = 0; i < punches; i++) {
		int send_punch = rand_int(min_damage, max_damage);
		MPI_Send(&send_punch, 1, MPI_INT, enemy_id, 0, MPI_COMM_WORLD);
		int recvd_punch;
		MPI_Recv(&recvd_punch, 1, MPI_INT, enemy_id, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		recvd_damage += recvd_punch;
		std::cout << "Boxer " << id << " got " << recvd_punch << " damage" << std::endl;
	}
	
	MPI_Send(&recvd_damage, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void mpi_referee() {
	MPI_Status status;
	int boxer1_recv_damage;
	int boxer2_recv_damage;
	MPI_Recv(&boxer1_recv_damage, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	MPI_Recv(&boxer2_recv_damage, 1, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	int min_recv_damage = std::min(boxer1_recv_damage, boxer2_recv_damage);
	std::cout << "Boxer 1 total received damage: " << boxer1_recv_damage << std::endl
		<< "Boxer 2 total received damage: " << boxer2_recv_damage << std::endl
		<< "Boxer " << (boxer1_recv_damage == min_recv_damage ? 1 : 2) << " is the winner!" << std::endl;
}

int main(int argc, char** argv) {
	int process_id;
	int process_num;
	int processor_name_len;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Comm_size(MPI_COMM_WORLD, &process_num);
	MPI_Get_processor_name(processor_name, &processor_name_len);

	if (process_num != 3) {
		std::cerr << "Error: number of processes must be 3" << std::endl;
		return 1;
	}

	srand((unsigned)time(NULL) + process_id * process_num + processor_name_len);

	switch (process_id)
	{
	case 0:
		mpi_referee();
		break;
	case 1:
		mpi_boxer(1, 2);
		break;
	case 2:
		mpi_boxer(2, 1);
		break;
	default:
		std::cerr << "Unknown process id" << std::endl;
		return 1;
	}

	MPI_Finalize();
	return 0;
}
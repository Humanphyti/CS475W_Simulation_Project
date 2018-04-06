////////////////////////////////////
// Authors: Andrew, Eben and Tristan 
// Operating System Simulator
// Main function for testing and running the simulator
// Last Modified: 4/6/2018
////////////////////////////////////

#include "PCB.h"


#include "FCFS.cpp"
#include "RoundRobin.cpp"
#include "SPN.cpp"
#include "Multicore.cpp"
#include "MultiLevel_FQ.cpp"
#include "calculate.cpp"


#include <ctime>
#include <iostream>
using namespace std;

int main() {

	
	srand(time(NULL));
	vector<int> ids = { 0 };
	
	//Creating PIDs
	vector<PCB> pcbs;

	for (int i = 0; i < 1000; i++) {
		PCB p1;
		p1.Randomize(ids);
		pcbs.push_back(p1);
		//cout << p1.get_PID() << endl;
	}

	//Create file of processes
	///TO DO!!!!!!!!!!

	//Read in file of processes
	///TO DO!!!!!!!!!!

	//Sort vector of pcbs by arrival time (first arrival time at the front and last arrival time at the back)
	///TO DO!!!!!!!!!!

	//Algorithm analysis information
	int avgTurnaround;
	int avgResponse;
	int avgWait;

	///Tests
	//Run FCFC
	FCFS(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);


	//Run Round Robin with time quantum = 5, context switch time = 2
	RoundRobin(pcbs, 2, 5);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);

	//Run Round Robin with time quantum = 10, context switch time = 2
	RoundRobin(pcbs, 2, 10);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);

	//Run Round Robin with time quantum = 20, context switch time = 2
	RoundRobin(pcbs, 2, 20);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);

	//Run SPN
	SPN(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);

	//Run Multicore
	Multicore(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);

	//Run multilevel feedback queue with context switch time = 2, and time quantums 10, 20 and 30
	MultiLevel_FQ(pcbs, 2, 10, 20, 30);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);

	//Run multilevel feedback queue with context switch time = 2, and time quantums 5, 10 and 15
	MultiLevel_FQ(pcbs, 2, 5, 10, 15);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);


	return 0;
}
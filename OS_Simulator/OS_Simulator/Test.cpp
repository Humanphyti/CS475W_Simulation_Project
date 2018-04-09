////////////////////////////////////
// Authors: Andrew, Eben and Tristan 
// Operating System Simulator
// Main function for testing and running the simulator
// Last Modified: 4/6/2018
////////////////////////////////////

#include "PCB.h"
#include "ProcessTxt.h"


#include "FCFS.cpp"
#include "RoundRobin.cpp"
#include "SPN.cpp"
#include "Multicore.cpp"
#include "MultiLevel_FQ.cpp"
#include "calculate.cpp"


#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
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
	WriteFile();
	

	//Read in file of processes
	//unnecessary given how our PCB is set up

	//Sort vector of pcbs by arrival time (first arrival time at the front and last arrival time at the back)
	PCB *p1 = 0, *p2 = 0;
	p1 = &pcbs.front();
	sort(pcbs.front(), pcbs.back(), (p1->get_arrival() < p2->get_arrival()));
	p2 = p1;
	pcbs.erase(pcbs.begin());
	

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
	cout << "First Come First Served:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;


	//Run Round Robin with time quantum = 5, context switch time = 2
	RoundRobin(pcbs, 2, 5);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Round Robin TQ: 5:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;

	//Run Round Robin with time quantum = 10, context switch time = 2
	RoundRobin(pcbs, 2, 10);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Round Robin TQ: 10:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;

	//Run Round Robin with time quantum = 20, context switch time = 2
	RoundRobin(pcbs, 2, 20);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Round Robin TQ: 20:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;

	//Run SPN
	SPN(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Shortest Process Next:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;

	//Run Multicore
	Multicore(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Multicore:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;

	//Run multilevel feedback queue with context switch time = 2, and time quantums 10, 20 and 30
	MultiLevel_FQ(pcbs, 2, 10, 20, 30);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Multilevel Feedback Queue TQs: 10, 20, 30:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;

	//Run multilevel feedback queue with context switch time = 2, and time quantums 5, 10 and 15
	MultiLevel_FQ(pcbs, 2, 5, 10, 15);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Multilevel Feedback Queue TQs: 5, 10, 15:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;


	return 0;
}
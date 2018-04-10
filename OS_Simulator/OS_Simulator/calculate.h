////////////////////////////////////
// Authors: Andrew, Eben and Tristan 
// Operating System Simulator
// Contains functions for finding simulator metrics: average response, average turnaround, etc.
// Last Modified: 4/6/2018
////////////////////////////////////

#include <vector>
#include "PCB.h"

using std::vector;

///All of these processes should be called after a simulator is used to calculate the metrics for the simulation

//Gets the turnaround time from every pcb object in order to calculate the avg turnaround time for all processes
int getAvgTurnaround(vector<PCB> &pcbs) {
	int sum = 0;
	for (int i = 0; i < pcbs.size(); i++) {
		sum += pcbs[i].get_turnaround();
	}
	double avg = sum / pcbs.size();

	return avg;
}

//Gets the response time from every pcb object in order to calculate the avg response time for all processes
int getAvgResponse(vector<PCB> &pcbs) {
	int sum = 0;
	for (int i = 0; i < pcbs.size(); i++) {
		sum += pcbs[i].get_response();
	}
	double avg = sum / pcbs.size();

	return avg;
}

//Gets the wait time from every pcb object in order to calculate the avg wait time for all processes
int getAvgWait(vector<PCB> &pcbs) {
	int sum = 0;
	for (int i = 0; i < pcbs.size(); i++) {
		sum += pcbs[i].get_wait();
	}
	double avg = sum / pcbs.size();

	return avg;
}

//Resets the changing values back to default for all processes by using the PCB reset() method
void resetPCB(vector <PCB> &pcbs) {
	for (int i = 0; i < pcbs.size(); i++) {
		pcbs[i].reset();
	}
}
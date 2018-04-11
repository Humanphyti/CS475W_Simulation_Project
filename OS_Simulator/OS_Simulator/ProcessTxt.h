#ifndef PROCESSTXT_H_
#define PROCESSTXT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "PCB.h"
using namespace std;

//int avgT, int avgR, int avgW,

void WriteFile(vector<PCB> pcbs, string name) {
	//vector<PCB*> pcbs;
	int differentiation = 1;
	ofstream processesIn;
	if (name == "RoundRobin")
		processesIn.open("processesRR.txt");
	else if (name == "SPN")
		processesIn.open("processesSPN.txt");
	else if (name == "FCFS")
		processesIn.open("processesFCFS.txt");
	else
		processesIn.open("processes.txt");

	if (!processesIn.is_open()) {
		cout << "File Could not be opened." << endl;
	}

	for (int i = 0; i < pcbs.size(); i++) {
		processesIn << "PID: " << pcbs[i].get_PID() << " ";		processesIn << "Arrival: " << pcbs[i].get_arrival() << " ";
		processesIn << "Estimated Total Burst: " << pcbs[i].get_estimated_burst() << " ";
		processesIn << "Estimated CPU: " << pcbs[i].get_estimated_cpu() << " ";
		processesIn << "Estimate IO: " << pcbs[i].get_estimated_io() << " ";
		processesIn << "Response: " << pcbs[i].get_response() << " ";
		processesIn << "Turnaround: " << pcbs[i].get_turnaround() << " ";
		processesIn << "Wait: " << pcbs[i].get_wait() << " ";

		processesIn << '\n';
	}
	
	//processesIn << '\n';
	//processesIn << "Average Turnaround Time: " << avgT << endl;
	//processesIn << "Average Response Time: " << avgR << endl;
	//processesIn << "Average Wait Time: " << avgW << endl;

};


#endif
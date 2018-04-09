#pragma once
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "PCB.h"
using namespace std;

void WriteFile() {
	vector<PCB> pcbs;
	ofstream processesIn;
	processesIn.open("processes.txt");

	if (!processesIn.is_open()) {
		cout << "File Could not be opened." << endl;
	}

	for (int i = 0; i < pcbs.size(); i++) {
		processesIn << pcbs[i].get_PID() << " ";
		processesIn << pcbs[i].get_arrival() << " ";
		processesIn << pcbs[i].get_estimated_burst() << " ";
		processesIn << pcbs[i].get_estimated_cpu() << " ";
		processesIn << pcbs[i].get_estimated_io() << " ";
		processesIn << '\n';
	}
}
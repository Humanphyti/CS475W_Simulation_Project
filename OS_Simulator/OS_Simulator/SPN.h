// Created by Eben Schumann
// 04/03/2018

#include <iostream>
#include <vector>
#include <queue>
#include "PCB.h"

using std::vector;
using std::queue;
using std::sort;

void SPN(vector<PCB> &pcbs) {

	queue<PCB> ready;

	vector<PCB> io_vector;

	//This will reference the PCB currently executing by the processor
	PCB *current_PCB;

	PCB *previous_PCB;

	int lastUpdated = -1;

	int currentTime = 0;

	int completedProcesses = 0;

	do {

		for (int i = 0; i < pcbs.size(); i++) {
			//if the arrival time of the currently examined process 
			if (pcbs[i].get_arrival() <= currentTime && pcbs[i].get_arrival() > lastUpdated)
				ready.push(pcbs[i]);
		}

		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vector.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vector[i].get_response() + io_vector[i].get_io() <= currentTime) {
				ready.push(io_vector[i]);
				io_vector.erase(io_vector.begin() + i);
			}
		}

		lastUpdated = currentTime;

		if (ready.empty()) {
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortestTime = NULL;

			for (int j = 0; j < pcbs.size(); j++) {
				//Check to see if a new process arrives first
				if (pcbs[j].get_response() == -1 && pcbs[j].get_arrival() < shortestTime)
					shortestTime = pcbs[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				if (pcbs[j].get_response() != -1 && pcbs[j].get_response() + pcbs[j].get_io() < shortestTime)
					shortestTime = pcbs[j].get_response() + pcbs[j].get_io();
			}

			currentTime = shortestTime;
		}

		//else the ready queue is not empty, so get the next process
		else {
		//sort the ready queue first then use the below code
	///	current_PCB = &ready.front();
	///	sort(ready.front(), ready.back(), ((current_PCB->get_estimated_burst()) < (previous_PCB->get_estimated_burst())));
	///	previous_PCB = current_PCB;
		ready.pop();
		current_PCB->set_running();

		//If the process has not been reacted to yet, set response time to current time
		if (current_PCB->get_response() == -1)
			current_PCB->set_response(currentTime);

		if (current_PCB->get_estimated_io() != current_PCB->get_io()) {
			current_PCB->update_io();
			std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
			//When IO is finished, place it in the io queue
			io_vector.push_back(*current_PCB);
		}
		else {
			//Update processed time by the remaining amount of processing time required
			int execution = current_PCB->get_remaining_time();
			current_PCB->update_cpu(execution);
			std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

			currentTime += execution;

			//Finalize values for the current process and do not put it back in queue: process is completed
			current_PCB->update_wait(currentTime);
			current_PCB->set_turnaround();

			std::cout << "Process " << current_PCB->get_PID() << " completed." << std::endl;
			completedProcesses++;
		}
		//In this case switches it to false, so not running
		current_PCB->set_running();
		}

	} while (completedProcesses < pcbs.size());

	delete current_PCB;
	delete previous_PCB;
}

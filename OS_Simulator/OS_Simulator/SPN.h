#pragma once
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
	
	int context_switch = 2;

	queue<PCB*> ready;
	//This is a vector that stores all of the processes that have been responded to, but are waiting for io to complete
	vector<PCB*> io_vector;

	//This will reference the PCB currently executing by the processor
	PCB *current_PCB;

	//This is the last time the processor checked for incoming processes  
	int last_updated = -1;
	//This is the current time of the processor
	int current_time = 0;

	//This is the number of completed processes, will loop until all processes in pcbs are completed
	int completed_processes = 0;
	//Total amount of time spent doing context switches
	int total_context = 0;

	do {
		//This adds all of the processes that have arrived since last execution started into the ready RR queue
		for (int i = 0; i < pcbs.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbs[i].get_arrival() <= current_time && pcbs[i].get_arrival() > last_updated) {
				ready.push(&pcbs[i]);
			}
		}

		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vector.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vector[i]->get_response() + io_vector[i]->get_io() <= current_time) {
				ready.push(io_vector[i]);
				io_vector.erase(io_vector.begin() + i);
			}
		}

		last_updated = current_time;

		//If the ready queue is empty, jump to the first time with something in the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (ready.empty()) {
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_time = 999999;

			for (int j = 0; j < pcbs.size(); j++) {
				//Check to see if a new process arrives first
				//				cout << pcbs[j].get_PID() << "has response " << pcbs[j].get_response() << endl;
				if (pcbs[j].get_response() == -1 && pcbs[j].get_arrival() < shortest_time)
					shortest_time = pcbs[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				//	if (pcbs[j].get_response() != -1 && (pcbs[j].get_response() + pcbs[j].get_io() + pcbs[j].get_arrival()) < shortest_time)
				//		shortest_time = pcbs[j].get_response() + pcbs[j].get_io();
			}

			for (int j = 0; j < io_vector.size(); j++) {
				if ((io_vector[j]->get_response() + io_vector[j]->get_io() + io_vector[j]->get_arrival()) < shortest_time)
					shortest_time = io_vector[j]->get_response() + io_vector[j]->get_io();
			}
			current_time = shortest_time;
		}
		//else the ready queue is not empty, so get the next process
		else {
		//sort the ready queue first then use the below code
	current_PCB = ready.front();
	bool sortedSPN = false;
	while (!sortedSPN) {
		sortedSPN = true;
		for (int i = 0; i < pcbs.size() - 1; i++) {
			if (pcbs[i].get_estimated_burst() > pcbs[i + 1].get_estimated_burst()) {
				sortedSPN = false;
				PCB temp = pcbs[i];
				pcbs[i] = pcbs[i + 1];
				pcbs[i + 1] = temp;
			}
		}
	}
	///	sort(ready.front(), ready.back(), ((current_PCB->get_estimated_burst()) < (previous_PCB->get_estimated_burst())));
	///	previous_PCB = current_PCB;
		ready.pop();
		current_PCB->set_running();

		//If the process has not been reacted to yet, set response time to current time
		if (current_PCB->get_response() == -1)
			current_PCB->set_response(current_time);

		if (current_PCB->get_estimated_io() != current_PCB->get_io()) {
			current_PCB->update_io();
			std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
			//When IO is finished, place it in the io queue
			io_vector.push_back(current_PCB);
		}
		else {
			//Update processed time by the remaining amount of processing time required
			int execution = current_PCB->get_remaining_time();
			current_PCB->update_cpu(execution);
			std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

			current_time += execution;

			//Finalize values for the current process and do not put it back in queue: process is completed
			current_PCB->update_wait(current_time);
			current_PCB->set_turnaround();

			std::cout << "Process " << current_PCB->get_PID() << " completed." << std::endl;
			completed_processes++;
		}
		//In this case switches it to false, so not running
		current_PCB->set_running();
		}

	} while (completed_processes < pcbs.size());

	
}

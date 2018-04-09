////////////////////////////////////
// Authors: Andrew McNeill 
// Operating System Simulator
// Implements a First Come First Serve (FCFS) simulator for a processor
// Last Modified: 4/5/2018
////////////////////////////////////

#include <vector>
#include <queue>
#include <iostream>
#include "PCB.h"

using std::vector;
using std::queue;

void FCFS(vector<PCB> &pcbs) {
	queue<PCB> ready;
	vector<PCB> io_vector;

	//This will reference the PCB currently executing by the processor
	PCB *current_PCB;

	int last_updated = -1;

	int current_time = 0;

	int completed_processes = 0;

	do {
		//This adds all of the processes that have arrived since last execution started into the ready queue
		for (int i = 0; i < pcbs.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if ((pcbs[i].get_arrival() <= current_time) && (pcbs[i].get_arrival() > last_updated))
				ready.push(pcbs[i]);
		}
		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vector.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vector[i].get_response() + io_vector[i].get_io() <= current_time) {
				ready.push(io_vector[i]);
				io_vector.erase(io_vector.begin() + i);
			}
		}

		last_updated = current_time;

		//If the ready queue is empty, jump to the first time with something in the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (ready.empty()) {
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_time = NULL;

			for (int j = 0; j < pcbs.size(); j++) {
				//Check to see if a new process arrives first
				if (pcbs[j].get_response() == -1 && pcbs[j].get_arrival() < shortest_time)
					shortest_time = pcbs[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				if (pcbs[j].get_response() != -1 && pcbs[j].get_response() + pcbs[j].get_io() < shortest_time)
					shortest_time = pcbs[j].get_response() + pcbs[j].get_io();
			}

			current_time = shortest_time;
		}
		//else the ready queue is not empty, so get the next process
		else {
			//Uses pointer to the next object in order to affect the actual PCB object in memory
			current_PCB = &ready.front();
			ready.pop();
			current_PCB->set_running();

			//If the process has not been reacted to yet, set response time to current time
			if (current_PCB->get_response() == -1)
				current_PCB->set_response(current_time);

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

	delete current_PCB;
}
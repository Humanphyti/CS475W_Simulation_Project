#pragma once
////////////////////////////////////
// Authors: Tristan Hildahl 
// Operating System Simulator
// Implements a Round Robin simulator for a processor
// Last Modified: 4/4/2018
////////////////////////////////////

/*#include <vector>
#include <queue>
#include <iostream>
#include "PCB.h"

using std::vector;
using std::queue;

//This simulates a round robin approach for a processor
//@param vector<PCB> &pcbs is a vector of PCB objects that represents all of the processes to be completed
//							All process information will be changed on the PCB objects (pass by reference) 
//							The vector must be in order from earliest to latest arrival time
//@param time_splice is the amount of time spent processing before a context switch for the round robin 
//@param context_switch is the amount of time spent during a context switch
void RoundRobin(vector<PCB> pcbs, int context_switch, int time_splice) {
	//This is the queue of processes which are ready for execution (have arrived at the processor)
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

	//Until every process is completed, continue processing using round robin
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
			//Uses pointer to the next object in order to affect the actual PCB object in memory
			current_PCB = ready.front();
			ready.pop();
			current_PCB->set_running();


			//If the process has not been reacted to yet, set response time to current time
			if (current_PCB->get_response() == -1) {
				current_PCB->set_response(current_time);
				//				cout << "Response: " << current_PCB->get_response() << endl;
			}


			if (current_PCB->get_estimated_io() != current_PCB->get_io()) {
				current_PCB->update_io();
				//				std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
				//When IO is finished, place it in the io queue
				io_vector.push_back(current_PCB);
			}
			//If it is a cpu bound process for this execution period
			else {
				//if the process will not finish during the current execution phase add set execution time and put at the back of the ready queue
				int execution = current_PCB->get_remaining_time();
				if (execution > time_splice) {
					//Update processed time by time splice
					current_PCB->update_cpu(time_splice);
					//					std::cout << "Ran process " << current_PCB->get_PID() << " for " << time_splice << std::endl;

					current_time += time_splice;

					//Place uncomplete process at the back of the queue
					ready.push(current_PCB);
				}
				//the process will finish during the current execution phase, so finish and remove process
				else {
					//Update processed time by the remaining amount of processing time required
					current_PCB->update_cpu(execution);
					//	std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

					current_time += execution;

					//Finalize values for the current process and do not put it back in queue: process is completed
					current_PCB->update_wait(current_time);
					current_PCB->set_turnaround();

					std::cout << "Process " << current_PCB->get_PID() << " completed at time " << current_time << std::endl;
					completed_processes++;
				}
			}
			//In this case switches it to false, so not running
			current_PCB->set_running();

			current_time += context_switch;
			total_context += context_switch;

		}
	} while (completed_processes < pcbs.size());	//Loops until all processes have been completed

	delete current_PCB;
}
*/
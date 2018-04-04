////////////////////////////////////
// Authors: Andrew McNeill 
// Operating System Simulator
// Implements a First Come First Serve (FCFS) simulator for a processor
// Last Modified: 4/3/2018
////////////////////////////////////

#include <vector>
#include <queue>
#include <iostream>
#include "PCB.h"

using std::vector;
using std::queue;

void FCFS(vector<PCB> &pcbs, int estimated_burst) {
	queue<PCB> ready;

	int last_updated = -1;

	int current_time = 0;

	int completed_processes = 0;

	do {

		for (int i = 0; i < pcbs.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbs[i].get_arrival() <= current_time && pcbs[i].get_arrival > last_updated)
				ready.push(pcbs[i]);
		}
		last_updated = current_time;

		PCB *current_PCB = &ready.front();
		ready.pop();
		current_PCB->set_running();

		//If the process has not been reacted to yet, set response time to current time
		if (current_PCB->get_response() == -1)
			current_PCB->set_response(current_time);

		///Need to vary depending on IO!!!!!!!!
		bool IO = false;

		if (IO) {
			current_PCB->update_io(estimated_burst);
			std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
			//When IO is finished, place it back in the queue 
			ready.push(*current_PCB);
		}
		//If it is a cpu bound process for this execution period
		else {
			//if the process will not finish during the current execution phase add set execution time and put at the back of the ready queue

				//Update processed time by time splice
			current_PCB->update_cpu(estimated_burst);
			std::cout << "Ran process " << current_PCB->get_PID() << " for " << estimated_burst << std::endl;

			current_time += estimated_burst;
			current_PCB->update_wait(current_time);
			current_PCB->set_turnaround();

			std::cout << "Process " << current_PCB->get_PID() << " completed." << std::endl;
			completed_processes++;
		}
	} while (completed_processes < pcbs.size());
////////////////////////////////////
// Authors: Tristan Hildahl 
// Operating System Simulator
// Implements a Round Robin simulator for a processor
// Last Modified: 3/27/2018
////////////////////////////////////

#include <vector>
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
void RoundRobin(vector<PCB> &pcbs, int time_splice, int context_switch) {
	//This is the queue of processes which are ready for execution (have arrived at the processor)
	queue<PCB> ready;

	//This is the last time the processor checked for incoming processes  
	int last_updated = -1;
	//This is the current time of the processor
	int current_time = 0;

	//This is the number of completed processes, will loop until all processes in pcbs are completed
	int completed_processes = 0;

	//Until every process is completed, continue processing using round robin
	do {
		//This adds all of the processes that have arrived since last execution started into the ready RR queue
		for (int i = 0; i < pcbs.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbs[i].get_arrival() <= current_time && pcbs[i].get_arrival > last_updated)
				ready.push(pcbs[i]);
		}
		last_updated = current_time;

		//Get the next process
		//Uses pointer to the next object in order to affect the actual PCB object in memory
		PCB *current_PCB = &ready.front();
		ready.pop();
		current_PCB->set_running();

		//If the process has not been reacted to yet, set response time to current time
		if (current_PCB->get_response() == -1)
			current_PCB->set_response(current_time);

		///Need to vary depending on IO!!!!!!!!
		bool IO = false;

		if (IO) {
			current_PCB->update_io(time_splice);
			std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
			//When IO is finished, place it back in the queue 
			ready.push(*current_PCB);
		}
		//If it is a cpu bound process for this execution period
		else {
			//if the process will not finish during the current execution phase add set execution time and put at the back of the ready queue
			int execution = current_PCB->get_remaining_time();
			if (execution > time_splice) {
				//Update processed time by time splice
				current_PCB->update_cpu(time_splice);
				std::cout << "Ran process " << current_PCB->get_PID() << " for " << time_splice << std::endl;

				current_time += time_splice;

				//Place uncomplete process at the back of the queue
				ready.push(*current_PCB);
			}
			//the process will finish during the current execution phase, so finish and remove process
			else {
				//Update processed time by the remaining amount of processing time required
				current_PCB->update_cpu(execution);
				std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

				current_time += execution;

				//Finalize values for the current process and do not put it back in queue: process is completed
				current_PCB->update_wait(current_time);
				current_PCB->set_turnaround();

				std::cout << "Process " << current_PCB->get_PID() << " completed." << std::endl;
				completed_processes++;
			}
		}
		//In this case switches it to false, so not running
		current_PCB->set_running();

		current_time += context_switch;
	} while (completed_processes < pcbs.size());	//Loops until all processes have been completed
}
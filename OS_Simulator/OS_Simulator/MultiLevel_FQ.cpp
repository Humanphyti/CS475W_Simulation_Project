////////////////////////////////////
// Authors: Tristan Hildahl 
// Operating System Simulator
// Implements a Multi-Level Feedback Queue simulator for a processor with 3 round robin queues and a FCFS queue
// Last Modified: 4/4/2018
////////////////////////////////////

#include <vector>
#include <queue>
#include <iostream>
#include "PCB.h"

using std::vector;
using std::queue;

void MultiLevel_FQ(vector<PCB> pcbs, int context_switch) {

	//These are the queues of processes which are ready for execution (have arrived at the processor)
	//ready1 is the highest priority queue down to ready3 which are round robin, 
	//then readyFCFS is the lowest priority and is used for first come first serve
	queue<PCB> ready1, ready2, ready3, readyFCFS;

	//Rather than be input a set time quantum, it will be changed to 3 different values depending on which RR queue
	int time_splice;

	//This is a vector that stores all of the processes that have been responded to, but are waiting for io to complete
	vector<PCB> io_vector;

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
			if (pcbs[i].get_arrival() <= current_time && pcbs[i].get_arrival > last_updated)
				ready1.push(pcbs[i]);
		}

		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vector.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vector[i].get_response() + io_vector[i].get_io() <= current_time) {
				ready1.push(io_vector[i]);
				io_vector.erase[i];
			}
		}

		last_updated = current_time;

		//If all queues are empty, jump to the first time with something able to go in to the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (ready1.empty() && ready2.empty() && ready3.empty() && readyFCFS.empty()) {
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
		//else there is a process ready, so get the next process
		else {
			//Uses pointer to the next object in order to affect the actual PCB object in memory
			PCB *current_PCB;
			//The processor will prioritize the upper queues and only run from the lower queues if the upper levels are empty
			//This gets the proper process to run next based on which queues are empty
			//Also sets the time quantum depending on which queue is running this instance
			if (ready1.empty() && ready2.empty() && ready3.empty()) {
				*current_PCB = readyFCFS.front();
				readyFCFS.pop();

				time_splice = current_PCB->get_remaining_time();
			}
			else if (ready1.empty() && ready2.empty()) {
				*current_PCB = ready3.front();
				ready3.pop();

				time_splice = 50;
			}
			else if (ready1.empty()) {
				*current_PCB = ready2.front();
				ready2.pop();

				time_splice = 25;
			}
			else {
				*current_PCB = ready1.front();
				ready1.pop();

				time_splice = 10;
			}

			current_PCB->set_running();

			//If the process has not been reacted to yet, set response time to current time
			if (current_PCB->get_response() == -1)
				current_PCB->set_response(current_time);


			if (current_PCB->get_estimated_io() < current_PCB->get_io()) {
				current_PCB->update_io();
				std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
				//When IO is finished, place it in the io queue
				io_vector.push_back(*current_PCB);
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

					//Update number of times this process has been in this queue
					current_PCB->update_round();

					//If it has been in the queue twice, place incomplete process at the back of the next lowest queue
					//Since the counter is updated prior, it will be 1 after first time through, then it will be reset at 2 to be 0 after the second time through
					if (current_PCB->get_round() == 0) {
						//If it was in RR queue 1: ready1, move to ready2
						if (time_splice == 10) 
							ready2.push(*current_PCB);

						//If it was in RR queue 2: ready2, move to ready3
						if (time_splice == 25)
							ready3.push(*current_PCB);

						//If it was in RR queue 3: ready3, move to ready4
						if (time_splice == 50)
							readyFCFS.push(*current_PCB);

						//Note, if it was in readyFCFS it will be completed next execution cycle so it will not be checked here
					}
					//It stays in the queue it was before
					else {
						if (time_splice == 10)
							ready1.push(*current_PCB);
						if (time_splice == 25)
							ready2.push(*current_PCB);						
						if (time_splice == 50)
							ready3.push(*current_PCB);
						//Again, readyFCFS would finish, and thus not reach this and it would not need to be placed in a queue
					}
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
			total_context += context_switch;
		}
	} while (completed_processes < pcbs.size());	//Loops until all processes have been completed
}
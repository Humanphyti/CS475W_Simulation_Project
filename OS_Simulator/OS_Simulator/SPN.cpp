// Created by Eben Schumann
// 04/03/2018

#include <iostream>
#include <vector>
#include <queue>
#include "PCB.h"

using std::vector;
using std::queue;
using std::sort;

void SPN(vector<PCB> &pcbs, int estimated_burst) {

	queue<PCB> ready;

	int lastUpdated = -1;

	int currentTime = 0;

	int completedProcesses = 0;

	do {

		for (int i = 0; i < pcbs.size(); i++) {
			//if the arrival time of the currently examined process 
			if (pcbs[i].get_arrival() <= currentTime && pcbs[i].get_arrival > lastUpdated)
				ready.push(pcbs[i]);
		}

		lastUpdated = currentTime;

		//FIX ME!! Sort the ready queue by the estimated_burst

		//sort the ready queue first then use the below code
		PCB *current_PCB = &ready.front();
		PCB *previous_PCB = current_PCB;
		sort(ready.front(), ready.back(), (current_PCB->get_estimated_burst < previous_PCB->get_estimated_burst));
		ready.pop();
		current_PCB->set_running();

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

			currentTime += estimated_burst;
			current_PCB->update_wait(currentTime);
			current_PCB->set_turnaround();

			std::cout << "Process " << current_PCB->get_PID() << " completed." << std::endl;
			completedProcesses++;
		}

	} while (completedProcesses < pcbs.size());
}

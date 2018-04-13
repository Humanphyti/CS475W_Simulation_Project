#pragma once
////////////////////////////////////
// Authors: Andrew McNeill 
// Operating System Simulator
// Implements a Multi-core scheduler using 8 cores running FCFS
// Last Modified: 4/7/2018
////////////////////////////////////

#include <vector>
#include <queue>
#include <iostream>
#include "PCB.h"

using std::vector;
using std::queue;

void Multicore(vector<PCB> &pcbs) {
	//queue of type PCB refrencing generated list of processes sorted by arrival time
	queue<PCB*> readyMC;
	//vector for io devices to finish input/output simulation
	//multicore vector to hold up to 8 concurrent processes
	vector<PCB*> io_vectorMC;
	vector<PCB> multicores;

	//Uses pointer to the next object in order to affect the actual PCB object in memory
	PCB *current_PCBMC;

	int last_updatedMC = -1;
	int current_timeMC = 0;
	int completed_processesMC = 0;
	int runningCores = 0;

	do {

		//Checks if the contents of the io_vector have met their estimated_io time and can be sent to the ready queue (for cpu bursts)
		for (int i = 0; i < io_vectorMC.size(); i++)
		{
			//If the process has been in the io_vector for as long as its estimated_io then push to the ready queue
			if (io_vectorMC[i]->get_response() + io_vectorMC[i]->get_io() <= current_timeMC)
			{
				readyMC.push(io_vectorMC[i]);
				io_vectorMC.erase(io_vectorMC.begin() + i);

			}
		}
		//This adds all of the processes that have arrived since last execution started into the ready queue
		for (int i = 0; i < pcbs.size(); i++)
		{
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbs[i].get_arrival() <= current_timeMC && pcbs[i].get_arrival() > last_updatedMC)
				readyMC.push(&pcbs[i]);
		}

		last_updatedMC = current_timeMC;

		//If the ready queue is empty, jump to the first time with something in the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (readyMC.empty())
		{
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_timeMC = 999999;

			for (int j = 0; j < pcbs.size(); j++)
			{
				//Check to see if a new process arrives first
				if (pcbs[j].get_response() == -1 && pcbs[j].get_arrival() < shortest_timeMC)
					shortest_timeMC = pcbs[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				if (pcbs[j].get_response() != -1 && pcbs[j].get_response() + pcbs[j].get_io() < shortest_timeMC)
					shortest_timeMC = pcbs[j].get_response() + pcbs[j].get_io();
			}

			current_timeMC = shortest_timeMC;
		}
		//else the ready queue is not empty, so get the next process
		else
		{
			//Load ready processes to the cores
			//Ensure no more than 8 cores are running at a time
			for (int k = 0; k < readyMC.size(); k++) {
				multicores[k] = *readyMC.front();
				readyMC.pop();
				++runningCores;
				if (runningCores == 8)
					break;
			}
			//If there is at least one process in the multicore
			if (!multicores.empty()) {
				//initialize execution time to the cpu burst of the first element in the multicore vector
				int execution = multicores[0].get_estimated_cpu();
				//Check all of the processes in the multicore if they have a cpu burst time less than the first element's
				for (int j = 0; j < multicores.size(); j++) {
					//If it finds a cpu burst less than another than that will be the execution time
					if (multicores[j].get_estimated_cpu() < execution) {
						//execution time becomes shortest remaining burst time
						execution = multicores[j].get_estimated_cpu();
					}
				}
				current_timeMC += execution;
				//Manipulate the actual process data of all processes in multicores
				for (int i = 0; i < multicores.size(); i++) {
					current_PCBMC = &multicores[i];
					current_PCBMC->set_running();

					//If the process has not been reacted to yet, set response time to current time
					if (current_PCBMC->get_response() == -1)
						current_PCBMC->set_response(current_timeMC);

					//If the process has started its cpu but not finished
					if (multicores[i].get_remaining_time() - multicores[i].get_estimated_io() > multicores[i].get_estimated_cpu()) {
						current_PCBMC->update_cpu(execution);
					}

					//If the process has not met its estimated io 
					if (current_PCBMC->get_estimated_io() != current_PCBMC->get_io()) {
						current_PCBMC->update_io();
						std::cout << "Ran process " << current_PCBMC->get_PID() << " with an IO device." << std::endl;
						//When IO is finished, place it in the io queue
						io_vectorMC.push_back(current_PCBMC);
					}
					//If the process has met both its cpu and io bursts
					if (current_PCBMC->get_estimated_burst() == current_PCBMC->get_estimated_cpu() + current_PCBMC->get_io()) {
						std::cout << "Ran process " << current_PCBMC->get_PID() << " for " << execution << std::endl;

						//Finalize values for the current process and do not put it back in queue: process is completed
						current_PCBMC->update_wait(current_timeMC);
						current_PCBMC->set_turnaround();
						std::cout << "Process " << current_PCBMC->get_PID() << " completed." << std::endl;
						current_PCBMC->set_running();
						completed_processesMC++;
						multicores.erase(multicores.begin() + i);
						--runningCores;
					}
				}
			}
		}
	} while (completed_processesMC < pcbs.size());
	delete current_PCBMC;
}

////////////////////////////////////
// Authors: Andrew, Eben and Tristan 
// Operating System Simulator
// Main function for testing and running the simulator
// Last Modified: 4/6/2018
////////////////////////////////////

#include "PCB.h"
#include "ProcessTxt.h"

//Our guess, is that including these files are causing linker errors by repeating inclusion
//However, it is not reading the code from our other source files without doing so
//A solution is currently to move the code for these methods into the same file as main, as seen below
//For some cleanliness, the methods here have been condensed and the other source files are also available unaltered for easier viewing 
//#include "FCFS.h"
//#include "RoundRobin.h"
//#include "SPN.h"
//#include "Multicore.h"
//#include "MultiLevel_FQ.h"
//#include "calculate.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;


int main() {

	///Create pcbs
	srand(time(NULL));
	vector<int> ids = { 0 };

	//Creating PIDs
	vector<PCB> pcbs;

	for (int i = 0; i < 1000; i++) {
		PCB p1;
		p1.Randomize(ids);
		pcbs.push_back(p1);
		//cout << p1.get_PID() << endl;

	}

	//Sort the pcbs by arrival time
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 0; i < pcbs.size() - 1; i++) {
			if (pcbs[i].get_arrival() > pcbs[i + 1].get_arrival()) {
				sorted = false;
				PCB temp = pcbs[i];
				pcbs[i] = pcbs[i + 1];
				pcbs[i + 1] = temp;
			}
		}
	}
	//WriteFile(pcbs, "Other");

	vector<PCB> pcbsFCFS;
	vector<PCB> pcbsSPN;
	vector<PCB> pcbsML;
	vector<PCB> pcbsMC;
	int k = 0;
	for ( k; k < pcbs.size(); k++){
		pcbsFCFS.push_back(pcbs[k]);
		pcbsSPN.push_back(pcbs[k]);
		pcbsML.push_back(pcbs[k]);
		pcbsMC.push_back(pcbs[k]);
	}
	//Algorithm analysis information
	int avgTurnaround;
	int avgResponse;
	int avgWait;

	//Forced to use pass by value if ran this way, but to increase efficiency, we just copied methods into main to test
	//RoundRobin(pcbs, 2, 10);

		///Simulation
	///RoundRobin
	//This is the queue of processes which are ready for execution (have arrived at the processor)
	int context_switch = 2;
	int time_splice = 10;
	
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
					//					std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

					current_time += execution;

					//Finalize values for the current process and do not put it back in queue: process is completed
					current_PCB->update_wait(current_time);
					current_PCB->set_turnaround();

					//std::cout << "Process " << current_PCB->get_PID() << " completed at time " << current_time << std::endl;
					completed_processes++;
				}
			}
			//In this case switches it to false, so not running
			current_PCB->set_running();

			current_time += context_switch;
			total_context += context_switch;

		}
	} while (completed_processes < pcbs.size());	//Loops until all processes have been completed

	
	cout << "Finished Round Robin\n";

	///Analytics
	//Get avg turnaround
	int sum = 0;
	for (int i = 0; i < pcbs.size(); i++) {
		sum += pcbs[i].get_turnaround();
	}
	double avgT = sum / pcbs.size();

	//Get avg response
	sum = 0;
	for (int i = 0; i < pcbs.size(); i++) {
		sum += pcbs[i].get_response();
	}
	double avgR = sum / pcbs.size();

	//Get avg wait
	sum = 0;
	for (int i = 0; i < pcbs.size(); i++) {
		sum += pcbs[i].get_wait();
	}
	double avgW = sum / pcbs.size();

	//Create the file
	WriteFile(pcbs, avgT, avgR, avgW, "RoundRobin");
	cout << avgT << endl << avgR << endl << avgW << endl << endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///*
	///SPN
	int context_switchSPN = 2;

	queue<PCB*> readySPN;
	//This is a vector that stores all of the processes that have been responded to, but are waiting for io to complete
	vector<PCB*> io_vectorSPN;

	//This will reference the PCB currently executing by the processor
	PCB *current_PCBSPN;

	//This is the last time the processor checked for incoming processes  
	int last_updatedSPN = -1;
	//This is the current time of the processor
	int current_timeSPN = 0;

	//This is the number of completed processes, will loop until all processes in pcbs are completed
	int completed_processesSPN = 0;
	//Total amount of time spent doing context switches
	int total_contextSPN = 0;

	do {
		//This adds all of the processes that have arrived since last execution started into the ready RR queue
		for (int i = 0; i < pcbsSPN.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbsSPN[i].get_arrival() <= current_timeSPN && pcbsSPN[i].get_arrival() > last_updatedSPN) {
				readySPN.push(&pcbsSPN[i]);
			}
		}

		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vectorSPN.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vectorSPN[i]->get_response() + io_vectorSPN[i]->get_io() <= current_timeSPN) {
				readySPN.push(io_vectorSPN[i]);
				io_vectorSPN.erase(io_vectorSPN.begin() + i);
			}
		}

		last_updatedSPN = current_timeSPN;

		//If the ready queue is empty, jump to the first time with something in the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (readySPN.empty()) {
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_timeSPN = 999999;

			for (int j = 0; j < pcbsSPN.size(); j++) {
				//Check to see if a new process arrives first
				//				cout << pcbs[j].get_PID() << "has response " << pcbs[j].get_response() << endl;
				if (pcbsSPN[j].get_response() == -1 && pcbsSPN[j].get_arrival() < shortest_timeSPN)
					shortest_timeSPN = pcbsSPN[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				//	if (pcbs[j].get_response() != -1 && (pcbs[j].get_response() + pcbs[j].get_io() + pcbs[j].get_arrival()) < shortest_time)
				//		shortest_time = pcbs[j].get_response() + pcbs[j].get_io();
			}

			for (int j = 0; j < io_vectorSPN.size(); j++) {
				if ((io_vectorSPN[j]->get_response() + io_vectorSPN[j]->get_io() + io_vectorSPN[j]->get_arrival()) < shortest_timeSPN)
					shortest_timeSPN = io_vectorSPN[j]->get_response() + io_vectorSPN[j]->get_io();
			}
			current_timeSPN = shortest_timeSPN;
		}
		//else the ready queue is not empty, so get the next process
		else {
			
			//sort the ready queue first then use the below code
			current_PCBSPN = readySPN.front();
			
			bool sortedSPN = false;
			while (!sortedSPN) {
				sortedSPN = true;
				for (int i = 0; i < pcbsSPN.size() - 1; i++) {
					if (pcbsSPN[i].get_estimated_burst() > pcbsSPN[i + 1].get_estimated_burst())   {
						sortedSPN = false;
						PCB tempSPN = pcbsSPN[i];
						pcbsSPN[i] = pcbsSPN[i + 1];
						pcbsSPN[i + 1] = tempSPN;
					}
				}
			}
			///	sort(ready.front(), ready.back(), ((current_PCB->get_estimated_burst()) < (previous_PCB->get_estimated_burst())));
			///	previous_PCB = current_PCB;
			readySPN.pop();
			current_PCBSPN->set_running();

			//If the process has not been reacted to yet, set response time to current time
			if (current_PCBSPN->get_response() == -1)
				current_PCBSPN->set_response(current_timeSPN);

			if (current_PCBSPN->get_estimated_io() != current_PCBSPN->get_io()) {
				current_PCBSPN->update_io();
				//std::cout << "Ran process " << current_PCBSPN->get_PID() << " with an IO device." << std::endl;
				//When IO is finished, place it in the io queue
				io_vectorSPN.push_back(current_PCBSPN);
			}
			else {
				//Update processed time by the remaining amount of processing time required
				int execution = current_PCBSPN->get_remaining_time();
				current_PCBSPN->update_cpu(execution);
				//std::cout << "Ran process " << current_PCBSPN->get_PID() << " for " << execution << std::endl;

				current_timeSPN += execution;

				//Finalize values for the current process and do not put it back in queue: process is completed
				current_PCBSPN->update_wait(current_timeSPN);
				current_PCBSPN->set_turnaround();

				//std::cout << "Process " << current_PCBSPN->get_PID() << " completed." << std::endl;
				completed_processesSPN++;
			}
			//In this case switches it to false, so not running
			current_PCBSPN->set_running();
		}

	} while (completed_processesSPN < pcbsSPN.size());
	
	cout << "Finished SPN\n";

	///Analytics
	//Get avg turnaround
	int sumSPN = 0;
	for (int i = 0; i < pcbsSPN.size(); i++) {
		sumSPN += pcbsSPN[i].get_turnaround();
	}
	double avgTSPN = sumSPN / pcbsSPN.size();

	//Get avg response
	sumSPN = 0;
	for (int i = 0; i < pcbsSPN.size(); i++) {
		sumSPN += pcbsSPN[i].get_response();
	}
	double avgRSPN = sumSPN / pcbsSPN.size();

	//Get avg wait
	sumSPN = 0;
	for (int i = 0; i < pcbsSPN.size(); i++) {
		sumSPN += pcbsSPN[i].get_wait();
	}
	double avgWSPN = sumSPN / pcbsSPN.size();
//	*/

	//Create the file
	WriteFile(pcbsSPN, avgTSPN, avgRSPN, avgWSPN, "SPN");
	cout << avgTSPN << endl << avgRSPN << endl << avgWSPN << endl << endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///*
	///FCFS
	queue<PCB*> readyFCFS;
	//This is a vector that stores all of the processes that have been responded to, but are waiting for io to complete
	vector<PCB*> io_vectorFCFS;

	//This will reference the PCB currently executing by the processor
	PCB *current_PCBFCFS;
	
	//This is the last time the processor checked for incoming processes  
	int last_updatedFCFS = -1;
	//This is the current time of the processor
	int current_timeFCFS = 0;

	//This is the number of completed processes, will loop until all processes in pcbs are completed
	int completed_processesFCFS = 0;
	//Total amount of time spent doing context switches
	int total_contextFCFS = 0;

	//Until every process is completed, continue processing using round robin
	do {
		//This adds all of the processes that have arrived since last execution started into the ready RR queue
		for (int i = 0; i < pcbsFCFS.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbsFCFS[i].get_arrival() <= current_timeFCFS && pcbsFCFS[i].get_arrival() > last_updatedFCFS) {
				readyFCFS.push(&pcbsFCFS[i]);
			}
		}

		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vectorFCFS.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vectorFCFS[i]->get_response() + io_vectorFCFS[i]->get_io() <= current_timeFCFS) {
				readyFCFS.push(io_vectorFCFS[i]);
				io_vectorFCFS.erase(io_vectorFCFS.begin() + i);
			}
		}

		last_updatedFCFS = current_timeFCFS;

		//If the ready queue is empty, jump to the first time with something in the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (readyFCFS.empty()) {
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_timeFCFS = 999999;

			for (int j = 0; j < pcbsFCFS.size(); j++) {
				//Check to see if a new process arrives first
				//				cout << pcbs[j].get_PID() << "has response " << pcbs[j].get_response() << endl;
				if (pcbsFCFS[j].get_response() == -1 && pcbsFCFS[j].get_arrival() < shortest_timeFCFS)
					shortest_timeFCFS = pcbsFCFS[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				//	if (pcbs[j].get_response() != -1 && (pcbs[j].get_response() + pcbs[j].get_io() + pcbs[j].get_arrival()) < shortest_time)
				//		shortest_time = pcbs[j].get_response() + pcbs[j].get_io();
			}

			for (int j = 0; j < io_vectorFCFS.size(); j++) {
				if ((io_vectorFCFS[j]->get_response() + io_vectorFCFS[j]->get_io() + io_vectorFCFS[j]->get_arrival()) < shortest_timeFCFS)
					shortest_timeFCFS = io_vectorFCFS[j]->get_response() + io_vectorFCFS[j]->get_io();
			}
			current_timeFCFS = shortest_timeFCFS;
		}
		//else the ready queue is not empty, so get the next process
		else {
			//Uses pointer to the next object in order to affect the actual PCB object in memory
			current_PCBFCFS = readyFCFS.front();
			readyFCFS.pop();
			current_PCBFCFS->set_running();

			//If the process has not been reacted to yet, set response time to current time
			if (current_PCBFCFS->get_response() == -1)
				current_PCBFCFS->set_response(current_timeFCFS);

			if (current_PCBFCFS->get_estimated_io() != current_PCBFCFS->get_io()) {
				current_PCBFCFS->update_io();
				//std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
				//When IO is finished, place it in the io queue
				io_vectorFCFS.push_back(current_PCBFCFS);
			}
			else {
				//Update processed time by the remaining amount of processing time required
				int executionFCFS = current_PCBFCFS->get_remaining_time();
				current_PCBFCFS->update_cpu(executionFCFS);
				//std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

				current_timeFCFS += executionFCFS;

				//Finalize values for the current process and do not put it back in queue: process is completed
				current_PCBFCFS->update_wait(current_timeFCFS);
				current_PCBFCFS->set_turnaround();

				//std::cout << "Process " << current_PCBFCFS->get_PID() << " completed." << std::endl;
				completed_processesFCFS++;
			}
			//In this case switches it to false, so not running
			current_PCBFCFS->set_running();
		}


	} while (completed_processesFCFS < pcbsFCFS.size());

	cout << "Finished FCFS\n";

	///Analytics
	//Get avg turnaround
	int sumFCFS = 0;
	for (int i = 0; i < pcbsFCFS.size(); i++) {
		sumFCFS += pcbsFCFS[i].get_turnaround();
	}
	double avgTFCFS = sumFCFS / pcbsFCFS.size();

	//Get avg response
	sumFCFS = 0;
	for (int i = 0; i < pcbsFCFS.size(); i++) {
		sumFCFS += pcbsFCFS[i].get_response();
	}
	double avgRFCFS = sumFCFS / pcbsFCFS.size();
	//Get avg wait
	sumFCFS = 0;
	for (int i = 0; i < pcbsFCFS.size(); i++) {
		sumFCFS += pcbsFCFS[i].get_wait();
	}
	double avgWFCFS = sumFCFS / pcbsFCFS.size();

	cout << avgTFCFS << endl << avgRFCFS << endl << avgWFCFS << endl << endl;

	//Create the file
	WriteFile(pcbsFCFS, avgTFCFS, avgRFCFS, avgWFCFS, "FCFS");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//These are the queues of processes which are ready for execution (have arrived at the processor)
	//ready1 is the highest priority queue down to ready3 which are round robin, 
	//then readyFCFS is the lowest priority and is used for first come first serve
	queue<PCB*> ready1, ready2, ready3, readyFCFSML;

	int time1 = 5, time2 = 10, time3 = 15;

	//Rather than be input a set time quantum, it will be changed to 3 different values depending on which RR queue
	int time_spliceML;

	//This is a vector that stores all of the processes that have been responded to, but are waiting for io to complete
	vector<PCB*> io_vectorML;

	//Uses pointer to the next object in order to affect the actual PCB object in memory
	PCB *current_PCBML;

	//This is the last time the processor checked for incoming processes  
	int last_updatedML = -1;
	//This is the current time of the processor
	int current_timeML = 0;

	//This is the number of completed processes, will loop until all processes in pcbs are completed
	int completed_processesML = 0;
	//Total amount of time spent doing context switches
	int total_contextML = 0;


	//Until every process is completed, continue processing using round robin
	do {
		//This adds all of the processes that have arrived since last execution started into the ready RR queue
		for (int i = 0; i < pcbsML.size(); i++) {
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbsML[i].get_arrival() <= current_timeML && pcbsML[i].get_arrival() > last_updatedML)
				ready1.push(&pcbsML[i]);
		}

		//This adds all of the processes that have finished their io execution into the ready RR queue
		for (int i = 0; i < io_vectorML.size(); i++) {
			//If the current time exceeds the time necessary to finish the io portion of the program, remove from vector and place in ready queue
			if (io_vectorML[i]->get_response() + io_vectorML[i]->get_io() <= current_timeML) {
				ready1.push(io_vectorML[i]);
				io_vectorML.erase(io_vectorML.begin() + i);
			}
		}

		last_updatedML = current_timeML;

		//If all queues are empty, jump to the first time with something able to go in to the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (ready1.empty() && ready2.empty() && ready3.empty() && readyFCFS.empty()) {

			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_timeML = 999999;

			for (int j = 0; j < pcbsML.size(); j++) {
				//Check to see if a new process arrives first
				//				cout << pcbs[j].get_PID() << "has response " << pcbs[j].get_response() << endl;
				if (pcbsML[j].get_response() == -1 && pcbsML[j].get_arrival() < shortest_timeML)
					shortest_timeML = pcbsML[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				//	if (pcbs[j].get_response() != -1 && (pcbs[j].get_response() + pcbs[j].get_io() + pcbs[j].get_arrival()) < shortest_time)
				//		shortest_time = pcbs[j].get_response() + pcbs[j].get_io();
			}

			for (int j = 0; j < io_vectorML.size(); j++) {
				if ((io_vectorML[j]->get_response() + io_vectorML[j]->get_io() + io_vectorML[j]->get_arrival()) < shortest_timeML)
					shortest_timeML = io_vectorML[j]->get_response() + io_vectorML[j]->get_io();
			}
			current_timeML = shortest_timeML;
		}
		//else there is a process ready, so get the next process
		else {
			//The processor will prioritize the upper queues and only run from the lower queues if the upper levels are empty
			//This gets the proper process to run next based on which queues are empty
			//Also sets the time quantum depending on which queue is running this instance
			if (ready1.empty() && ready2.empty() && ready3.empty()) {
				current_PCBML = readyFCFSML.front();
				readyFCFSML.pop();

				time_spliceML = current_PCBML->get_remaining_time();
			}
			else if (ready1.empty() && ready2.empty()) {
				current_PCBML = ready3.front();
				ready3.pop();

				time_spliceML = time3;
			}
			else if (ready1.empty()) {
				current_PCBML = ready2.front();
				ready2.pop();

				time_spliceML = time2;
			}
			else {
				current_PCBML = ready1.front();
				ready1.pop();

				time_spliceML = time1;
			}

			current_PCBML->set_running();

			//If the process has not been reacted to yet, set response time to current time
			if (current_PCBML->get_response() == -1)
				current_PCBML->set_response(current_timeML);


			if (current_PCBML->get_estimated_io() != current_PCBML->get_io()) {
				current_PCBML->update_io();
				//std::cout << "Ran process " << current_PCB->get_PID() << " with an IO device." << std::endl;
				//When IO is finished, place it in the io queue
				io_vectorML.push_back(current_PCBML);
			}
			//If it is a cpu bound process for this execution period
			else {
				//if the process will not finish during the current execution phase add set execution time and put at the back of the ready queue
				int executionML = current_PCBML->get_remaining_time();
				if (executionML > time_spliceML) {
					//Update processed time by time splice
					current_PCBML->update_cpu(time_spliceML);
					//std::cout << "Ran process " << current_PCB->get_PID() << " for " << time_splice << std::endl;

					current_timeML += time_spliceML;

					//Update number of times this process has been in this queue
					current_PCBML->update_round();

					//If it has been in the queue twice, place incomplete process at the back of the next lowest queue
					//Since the counter is updated prior, it will be 1 after first time through, then it will be reset at 2 to be 0 after the second time through
					if (current_PCBML->get_round() == 0) {
						//If it was in RR queue 1: ready1, move to ready2
						if (time_spliceML == time1)
							ready2.push(current_PCBML);

						//If it was in RR queue 2: ready2, move to ready3
						if (time_spliceML == time2)
							ready3.push(current_PCBML);

						//If it was in RR queue 3: ready3, move to ready4
						if (time_spliceML == time3)
							readyFCFSML.push(current_PCBML);

						//Note, if it was in readyFCFS it will be completed next execution cycle so it will not be checked here
					}
					//It stays in the queue it was before
					else {
						if (time_spliceML == time1)
							ready1.push(current_PCBML);
						if (time_spliceML == time2)
							ready2.push(current_PCBML);
						if (time_spliceML == time3)
							ready3.push(current_PCBML);
						//Again, readyFCFS would finish, and thus not reach this and it would not need to be placed in a queue
					}
				}
				//the process will finish during the current execution phase, so finish and remove process
				else {
					//Update processed time by the remaining amount of processing time required
					current_PCBML->update_cpu(executionML);
					//std::cout << "Ran process " << current_PCB->get_PID() << " for " << execution << std::endl;

					current_timeML += executionML;

					//Finalize values for the current process and do not put it back in queue: process is completed
					current_PCBML->update_wait(current_timeML);
					current_PCBML->set_turnaround();

					//std::cout << "Process " << current_PCBML->get_PID() << " completed." << std::endl;
					completed_processesML++;
				}
			}
			//In this case switches it to false, so not running
			current_PCBML->set_running();

			current_timeML += context_switch;
			total_contextML += context_switch;
		}
	} while (completed_processesML < pcbsML.size());	//Loops until all processes have been completed


	cout << "Finished Multilevel Feedback Queue\n";

	///Analytics
	//Get avg turnaround
	int sumML = 0;
	for (int i = 0; i < pcbsML.size(); i++) {
		sumML += pcbsML[i].get_turnaround();
	}
	double avgTML = sumML / pcbsML.size();

	//Get avg response
	sumML = 0;
	for (int i = 0; i < pcbsML.size(); i++) {
		sumML += pcbsML[i].get_response();
	}
	double avgRML = sumML / pcbsML.size();
	//Get avg wait
	sumML = 0;
	for (int i = 0; i < pcbsML.size(); i++) {
		sumML += pcbsML[i].get_wait();
	}
	double avgWML = sumML / pcbsML.size();

	cout << avgTML << endl << avgRML << endl << avgWML << endl << endl;

	//Create the file
	WriteFile(pcbsML, avgTML, avgRML, avgWML, "ML");


	///MultiCore
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
		for (int i = 0; i < pcbsMC.size(); i++)
		{
			//If the arrival time for a process is between the time last checked and current time, add it to the ready RR queue
			if (pcbsMC[i].get_arrival() <= current_timeMC && pcbsMC[i].get_arrival() > last_updatedMC)
				readyMC.push(&pcbsMC[i]);
		}

		last_updatedMC = current_timeMC;

		//If the ready queue is empty, jump to the first time with something in the ready queue
		//Find the next time something enters, or shortest time between remaining arrival times and io completion times
		if (readyMC.empty())
		{
			//This will be the next time the cpu is running a process, and thus the time to start keeping track of the cpu again
			int shortest_timeMC = 999999;

			for (int j = 0; j < pcbsMC.size(); j++)
			{
				//Check to see if a new process arrives first
				if (pcbsMC[j].get_response() == -1 && pcbsMC[j].get_arrival() < shortest_timeMC)
					shortest_timeMC = pcbsMC[j].get_arrival();
				//or if a (previously responded to) io-running device becomes ready for cpu processing first
				//if (pcbsMC[j].get_response() != -1 && pcbsMC[j].get_response() + pcbsMC[j].get_io() < shortest_timeMC)
					//shortest_timeMC = pcbsMC[j].get_response() + pcbsMC[j].get_io();
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
						//std::cout << "Ran process " << current_PCBMC->get_PID() << " with an IO device." << std::endl;
						//When IO is finished, place it in the io queue
						io_vectorMC.push_back(current_PCBMC);
					}
					//If the process has met both its cpu and io bursts
					if (current_PCBMC->get_estimated_burst() == current_PCBMC->get_estimated_cpu() + current_PCBMC->get_io()) {
						//std::cout << "Ran process " << current_PCBMC->get_PID() << " for " << execution << std::endl;

						//Finalize values for the current process and do not put it back in queue: process is completed
						current_PCBMC->update_wait(current_timeMC);
						current_PCBMC->set_turnaround();
						//std::cout << "Process " << current_PCBMC->get_PID() << " completed." << std::endl;
						current_PCBMC->set_running();
						completed_processesMC++;
						multicores.erase(multicores.begin() + i);
						--runningCores;
					}
				}
			}
		}
	} while (completed_processesMC < pcbs.size());

	///Analytics
	//Get avg turnaround
	int sumMC = 0;
	for (int i = 0; i < pcbsMC.size(); i++) {
		sumML += pcbsMC[i].get_turnaround();
	}
	double avgTMC = sumMC / pcbsMC.size();

	//Get avg response
	sumMC = 0;
	for (int i = 0; i < pcbsMC.size(); i++) {
		sumMC += pcbsMC[i].get_response();
	}
	double avgRMC = sumMC / pcbsMC.size();
	//Get avg wait
	sumMC = 0;
	for (int i = 0; i < pcbsMC.size(); i++) {
		sumMC += pcbsMC[i].get_wait();
	}
	double avgWMC = sumMC / pcbsMC.size();

	cout << avgTMC << endl << avgRMC << endl << avgWMC << endl << endl;

	//Create the file
	WriteFile(pcbsMC, avgTMC, avgRMC, avgWMC, "MC");

	//*/
	/*
	///Tests
	//Run FCFC
	FCFS(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "First Come First Served:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);
	//RoundRobin(pcbs, 2, 10);

	//Run Round Robin with time quantum = 5, context switch time = 2
	RoundRobin(pcbs, 2, 5);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Round Robin TQ: 5:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);
	*/
	//avgTurnaround = getAvgTurnaround(pcbs);
	//avgResponse = getAvgResponse(pcbs);
	//avgWait = getAvgWait(pcbs);
	//cout << "Round Robin TQ: 10:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);
	/*
	//Run Round Robin with time quantum = 20, context switch time = 2
	RoundRobin(pcbs, 2, 20);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Round Robin TQ: 20:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);

	//Run SPN
	SPN(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Shortest Process Next:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);

	//Run Multicore
	Multicore(pcbs);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Multicore:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);

	//Run multilevel feedback queue with context switch time = 2, and time quantums 10, 20 and 30
	MultiLevel_FQ(pcbs, 2, 10, 20, 30);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Multilevel Feedback Queue TQs: 10, 20, 30:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	//reset(pcbs);

	//Run multilevel feedback queue with context switch time = 2, and time quantums 5, 10 and 15
	MultiLevel_FQ(pcbs, 2, 5, 10, 15);
	avgTurnaround = getAvgTurnaround(pcbs);
	avgResponse = getAvgResponse(pcbs);
	avgWait = getAvgWait(pcbs);
	cout << "Multilevel Feedback Queue TQs: 5, 10, 15:: Average Turnaround" << avgTurnaround << " Average Response " << avgResponse << " Average Wait: " << avgWait << endl;
	*/

	return 0;
}
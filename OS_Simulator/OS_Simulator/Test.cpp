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
	WriteFile(pcbs, "Other");

	/*vector<PCB> pcbsFCFS;
	vector<PCB> pcbsSPN;
	int k = 0;
	for ( k; k < pcbs.size(); k++){
		pcbsFCFS.push_back(pcbs[k]);
		pcbsSPN.push_back(pcbs[k]);
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
					if (pcbsSPN[i].get_estimated_burst() > pcbsSPN[i + 1].get_estimated_burst()) {
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

			//if (current_PCBSPN->get_estimated_io() != current_PCBSPN->get_io()) {
				//current_PCBSPN->update_io();
				//std::cout << "Ran process " << current_PCBSPN->get_PID() << " with an IO device." << std::endl;
				//When IO is finished, place it in the io queue
				//io_vectorSPN.push_back(current_PCBSPN);
			//}
			else {
				//Update processed time by the remaining amount of processing time required
				int execution = current_PCBSPN->get_remaining_time();
				current_PCBSPN->update_cpu(execution);
				//std::cout << "Ran process " << current_PCBSPN->get_PID() << " for " << execution << std::endl;

				current_timeSPN += execution;

				//Finalize values for the current process and do not put it back in queue: process is completed
				current_PCBSPN->update_wait(current_time);
				current_PCBSPN->set_turnaround();

				//std::cout << "Process " << current_PCBSPN->get_PID() << " completed." << std::endl;
				completed_processesSPN++;
			}
			//In this case switches it to false, so not running
			current_PCBSPN->set_running();
		}

	} while (completed_processesSPN < pcbsSPN.size());

	///Analytics
	//Get avg turnaround
	int sumSPN = 0;
	for (int i = 0; i < pcbsSPN.size(); i++) {
		sumSPN += pcbsSPN[i].get_turnaround();
	}
	double avgTSPN = sum / pcbsSPN.size();

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

	//Create the file
	WriteFile(pcbsSPN, avgTSPN, avgRSPN, avgWSPN, "SPN");
	
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
				current_PCB->update_wait(current_timeFCFS);
				current_PCBFCFS->set_turnaround();

				//std::cout << "Process " << current_PCB->get_PID() << " completed." << std::endl;
				completed_processesFCFS++;
			}
			//In this case switches it to false, so not running
			current_PCBFCFS->set_running();
		}


	} while (completed_processesFCFS < pcbsFCFS.size());

	///Analytics
	//Get avg turnaround
	int sumFCFS = 0;
	for (int i = 0; i < pcbsFCFS.size(); i++) {
		sumFCFS += pcbsFCFS[i].get_turnaround();
	}
	double avgTFCFS = sum / pcbsFCFS.size();

	//Get avg response
	sumFCFS = 0;
	for (int i = 0; i < pcbsFCFS.size(); i++) {
		sumSPN += pcbsFCFS[i].get_response();
	}
	double avgRFCFS = sumFCFS / pcbsFCFS.size();
	cout << "Here" << endl;
	//Get avg wait
	sumFCFS = 0;
	for (int i = 0; i < pcbsFCFS.size(); i++) {
		sumFCFS += pcbsFCFS[i].get_wait();
	}
	double avgWFCFS = sumFCFS / pcbsFCFS.size();

	//Create the file
	WriteFile(pcbsFCFS, avgTFCFS, avgRFCFS, avgWFCFS, "FCFS");
	*/
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
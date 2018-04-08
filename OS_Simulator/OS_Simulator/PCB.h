////////////////////////////////////
// Authors: Tristan Hildahl
// Operating System Simulator
// This is the PCB object that represents all information for a process
// Last Modified: 3/21/2018
////////////////////////////////////

#ifndef PCB_H_
#define PCB_H_

#include <vector>
using std::vector;

class PCB {
private:
	int PID;
	bool running;			//1 means process is running (0 it is waiting)

	///These are the cumulative times the process is spent running and waiting
	int cpu_burst;			//This is the total amount of time spent running thus far by the cpu
	int io_burst;			//Total time running by I/O
	int wait;				//Amount of time spent waiting in queues (total time thus far spent not running)

	//This is used only on the multilevel feedback queue to keep track of how often a process has been run in the same queue before changing
	int round_count;

	//Should be unique to process
	int arrival_time;		//This is the time when it gets brought into queue for processing
	int estimated_burst;	//Amount of burst time required to complete the process: process is completed when cpu_burst = this
	int estimated_io;		//Amount of required io burst time 
	int estimated_cpu;		//Amount of required cpu burst time


	int turnaround;			//Time from submission to completion
	int response;			//Time when the process is first responded to

public:
	///Constructors
	//Initially sets all values to 0, then they will be randomized to create unique processes with Randomize()
	PCB();

	//Constructor that builds a PCB from information in a file
	//PCB(......);

	//This randomizes the variables to create a new randomized process
	//param ids is a vector of all previously used ids to ensure a unique one is created
	void Randomize(vector<int> &ids);


	///Getters and Setters
	int get_PID() { return PID; }
	bool get_running() { return running; }
	int get_wait() { return wait; }
	int get_io() { return io_burst; }
	//int get_last_time() { return last_time; }
	int get_arrival() { return arrival_time; }
	int get_turnaround() { return turnaround; }
	int get_response() { return response; }
	int get_estimated_burst() { return estimated_burst; }
	int get_estimated_io() { return estimated_io; }
	int get_estimated_cpu() { return estimated_cpu; }


	//When process first created, set arrival time
	//Will be part of randomize
	//void set_arrival(int time) { arrival_time = time; }


	//Switches the state of the process from running and waiting(ready)
	void set_running() { running = !running; }
	//When first reacted to, set response time
	void set_response(int time) { response = time - arrival_time; }

	//Returns the remaining burst time, so processes will not be sent excess time to complete
	int get_remaining_time();

	//increases burst time by given amount of time (time spent processing)
	//param time is the amount of time alloted by the CPU
	void update_cpu(int time);
	//increases IO time by given amount of time (time spent processing)
	//param time is the amount of time alloted by the I/O device
	void update_io();
	//updates wait time by sending in the current total clock time and subtracting burst time and arrival time
	//Should only be called once when a process execution is completed
	void update_wait(int time);

	//When process is completed, update turnaround time
	//Should only be called once when a process execution is completed
	void set_turnaround();

	//For multilevel feedback queue only, adjust round_count
	void update_round();
	int get_round();
};

#endif
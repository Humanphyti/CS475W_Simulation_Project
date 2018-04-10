////////////////////////////////////
// Authors: Tristan Hildahl
// Operating System Simulator
// This is the PCB object implementation that represents all information for a process
// Last Modified: 3/21/2018
////////////////////////////////////

#include "PCB.h"

//Initially sets all values to 0, then they will be randomized to create unique processes with Randomize()
PCB::PCB()
{
	PID = 0;
	running = 0;
	cpu_burst = 0;
	io_burst = 0;
	wait = 0;

	arrival_time = 0;
	estimated_burst = 0;
	estimated_io = 0;

	turnaround = 0;
	response = -1;
}

//This randomizes the variables to create a new randomized process
void PCB::Randomize(vector<int> &ids) {
	
	//set PID
	int id;
	bool valid_id;
	//When a new id is created loop until a unique id value is found
	do {
		id = rand() % 10000000;	//Creates a random 7 digit number
		valid_id = 1;			//Assume the new id is a valid number

		//Check all past ids, and if the new id has already been used, loop to assign a new id
		for (int i = 0; i < ids.size(); i++) {
			if (ids[i] == id) {
				valid_id = 0;
				break;
			}
		}
	} while (!valid_id);
	
	PID = id;
	ids.push_back(id);

	running = 0;
	cpu_burst = 0;
	io_burst = 0;
	wait = 0;

	//set arrival time
	set_arrival_time();
	arrival_time = get_arrival();

	//Set execution time
	int range = rand() % 100;	//Assigns a random total execution time, with a 75% chance of being a shorter process
	if (range < 75)
		estimated_burst = (rand() % 50) + 1;
	else
		estimated_burst = (rand() % 150) + 50;

	//Io time must be less than the total execution time, but it is randomized
	estimated_io = rand() % estimated_burst;

	estimated_cpu = estimated_burst - estimated_io;

	turnaround = 0;
	response = -1;
}

///Methods
//Returns the remaining burst time, so processes will not be sent excess time to complete
int PCB::get_remaining_time()
{
	int remaining;
	remaining = estimated_burst - cpu_burst - io_burst;

	return remaining;
}

//updates wait time by sending in the current total clock time and subtracting burst time and arrival time
//Should only be called once when a process execution is completed
void PCB::update_wait(int time) 
{
	wait = time - arrival_time - estimated_cpu; 
}

//increases burst time by given amount of time (time spent processing)
//param time is the amount of time alloted by the CPU
void PCB::update_cpu(int time)
{
	cpu_burst += time;
}

//increases IO time by given amount of time (time spent processing)
//param time is the amount of time alloted by the I/O device
void PCB::update_io()
{
	io_burst = estimated_io;
}

//When process is completed, update turnaround time
//Should only be called once when a process execution is completed
void PCB::set_turnaround() {
	turnaround = estimated_burst + wait;
}

//For multilevel feedback queue only, adjust round_count
void PCB::update_round()
{
	//The first time through a RR queue will be 0, then count up
	//Desired number of times through queue before changing queues will be integer compared below
	if (round_count <= 2)
		round_count++;
	//Once it hits designated times through through, must change queues so reset to 0 when updated
	else
		round_count = 0;
}

int PCB::get_round()
{
	return round_count;
}

//This allows us to sort processes by execution time for shortest process next
PCB PCB::operator<(PCB p) {
	if (this->get_estimated_burst() < p.get_estimated_burst())
		return *this;
	else
		return p;
}

//A method for clearing the data values on a process so it can be used on a different simulation
void PCB::reset() {
	cpu_burst = 0;
	io_burst = 0;
	wait = 0;
	turnaround = 0;
	response = -1;
}
# CS475W_Simulation_Project

Process generator
PID#
State(ready/Running)
CPU Burst (Accu.)
I/O Burst (Accu.)
Arrival Time
Estimated CPU Burst

Analytic Metrics
Throughput
Turnaround
Response

PCB
All the info generated (list of P’s)

Uniprocessor Algorithms
R.R.		Queues that schedule their own version of the PCB (Time Quanta: 15, 30, 50)
FCFS		Based off process info and how they schedule
SPN		Sort of create virtual Gantt Charts that add up wait times as they are being sorted
As they are scheduling add up to analytics & print to a text doc

Multi-core Distribution
Load sharing
Queue of processes, next available core takes the next process in the queue. Divide work across 8 cores

Multilevel Feedback Queue
Three round robin queues followed by an FCFS queue. For the Round robins, the time quanta increases as we get deeper into the multilevel feedback queue. After 2 runs of a queue the processes that still remain will be moved to the lower queues until they reach the bottom FCFS. That guarantees that every queue is run through at least once.

Work Breakdown:
Group: Process Generator
Tristan: RR, MLFQ
Eben: SPN, Analytics Aggregation
Andrew: FCFS, Multi-Core Distribution

Deliverable Dates
Process Generator: March 23
Uniprocessor Algorithms: March 30 + Weekend
Multi-core, MLFQ, Analytics: April 6

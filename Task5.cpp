#include <iostream>      
#include <vector>        
#include <queue>         // used for Round Robin scheduling
#include <algorithm>     
#include <fstream>       // used to read from and write to files
#include <iomanip>       // used for formatting output
using namespace std;     
struct Process          // structure to store process details
{
    int pid;          
    int arrival;       
    int burst;         
};

// FCFS Scheduling Function
double fcfs(vector<Process> processes)   // This line takes process list and returns the average waiting time
{
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) // this sorts by the arrival time
    {
        if (a.arrival == b.arrival)      // if arrival times are equal
            return a.pid < b.pid;        // sort by process ID
        return a.arrival < b.arrival;    // otherwise sort by arrival time
    });

    int currentTime = 0;                 
    double totalWaitingTime = 0;         // stores total waiting time

    for (const auto &p : processes)      
    {
        if (currentTime < p.arrival)     // if CPU is idle
            currentTime = p.arrival;     // jump to arrival time

        int waitingTime = currentTime - p.arrival; // waiting time calculation
        totalWaitingTime += waitingTime;           // add to total waiting time

        currentTime += p.burst;          
    }

    return totalWaitingTime / processes.size(); 
}

// SJF  (Non-preemptive)
double sjf(vector<Process> processes)   // takes process list and returns avg waiting time
{
    int n = processes.size();           
    vector<bool> completed(n, false);   // track completed processes
    int completedCount = 0;             
    int currentTime = 0;                
    double totalWaitingTime = 0;        

    while (completedCount < n)          
    {
        int idx = -1;                   // index of selected process
        int shortestBurst = 1e9;        // very large number

        for (int i = 0; i < n; i++)     
        {
            if (!completed[i] && processes[i].arrival <= currentTime) // if process is ready
            {
                if (processes[i].burst < shortestBurst) // find shortest burst
                {
                    shortestBurst = processes[i].burst; // update shortest
                    idx = i;                            // store index
                }
            }
        }

        if (idx == -1)                  // if no process is ready
        {
            currentTime++;              // move time forward
            continue;                  // continue loop
        }

        int waitingTime = currentTime - processes[idx].arrival; // calculate waiting time
        totalWaitingTime += waitingTime;                        // add to total

        currentTime += processes[idx].burst; // execute process
        completed[idx] = true;               // mark as completed
        completedCount++;                   // increase completed count
    }

    return totalWaitingTime / n; // return average waiting time
}

// Round Robin Scheduling Function
double roundRobin(vector<Process> processes, int quantum) // takes process list and quantum
{
    int n = processes.size();             

    vector<int> remainingBurst(n);        // store remaining burst times
    vector<int> completionTime(n, 0);     // store completion times

    for (int i = 0; i < n; i++)          // initialize remaining burst times
        remainingBurst[i] = processes[i].burst; // copy burst times

    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) // sort by arrival
    {
        return a.arrival < b.arrival;    // sort ascending
    });

    queue<int> q;                        // queue for scheduling
    int currentTime = 0;                 
    int index = 0;                       // index to track arrivals
    int completed = 0;                   // completed processes count

    while (completed < n)               
    {
        while (index < n && processes[index].arrival <= currentTime) // add arrived processes
        {
            q.push(index);              // push into queue
            index++;                    // move to next process
        }

        if (q.empty())                 // if queue is empty
        {
            currentTime = processes[index].arrival; // jump to next arrival
            continue;                 // continue loop
        }

        int i = q.front();            // get front process
        q.pop();                      // remove it from queue

        int execTime = min(quantum, remainingBurst[i]); // decide execution time
        remainingBurst[i] -= execTime;                  // reduce remaining burst
        currentTime += execTime;                        // update time

        while (index < n && processes[index].arrival <= currentTime) // add new arrivals
        {
            q.push(index);            // push into queue
            index++;                 // move index
        }

        if (remainingBurst[i] > 0)   // if process not finished
        {
            q.push(i);               // push back into queue
        }
        else                        // if process finished
        {
            completionTime[i] = currentTime; // store completion time
            completed++;                     // increase completed count
        }
    }

    double totalWaitingTime = 0;     // total waiting time

    for (int i = 0; i < n; i++)      // calculate waiting times
    {
        int turnaround = completionTime[i] - processes[i].arrival; // turnaround time
        int waiting = turnaround - processes[i].burst;             // waiting time
        totalWaitingTime += waiting;                               // add to total
    }

    return totalWaitingTime / n;     // return average waiting time
}

int main()
{
    ifstream file("processes.txt");   // open input file

    if (!file)                        // check if file failed to open
    {
        cout << "Error opening file!" << endl; 
        return 1;                     // exit program
    }

    int n;                            // number of processes
    file >> n;                        // read number of processes

    vector<Process> processes(n);     // create process list

    for (int i = 0; i < n; i++)       // read each process
    {
        processes[i].pid = i + 1;     // assign process ID
        file >> processes[i].arrival >> processes[i].burst; // read arrival and burst
    }

    int quantum;                      // time quantum
    file >> quantum;                  // read quantum

    file.close();                     // close file

    double avgFCFS = fcfs(processes); // calculate FCFS result
    double avgSJF = sjf(processes);   // calculate SJF result
    double avgRR = roundRobin(processes, quantum); // calculate RR result

    cout << fixed << setprecision(2); // format output to 2 decimal places

    cout << "Average Waiting Time Results" << endl; // title
    cout << "FCFS        : " << avgFCFS << endl;     // print FCFS
    cout << "SJF         : " << avgSJF << endl;      // print SJF
    cout << "Round Robin : " << avgRR << endl;       // print RR

    ofstream outFile("results.csv");  // create output file

    outFile << "Algorithm,AverageWaitingTime\n"; 
    outFile << "FCFS," << avgFCFS << endl;;       
    outFile << "SJF," << avgSJF << endl;        
    outFile << "RoundRobin," << avgRR << endl;;   
    outFile.close();                  // close file
    cout << "Results saved to results.csv" << endl; // confirmation

    return 0;                         
}

#include <iostream> 
#include <fstream> // allows the program to read data from a file
#include <vector> 
using namespace std; 

int main() { 
    string filename; 
    cout << "Enter input file name: "; 
    cin >> filename; 

    ifstream file(filename); // open the file so the program can read its contents

    if (!file) { // check if the file failed to open
        cout << "Error opening file." << endl; // print an error message if the file cannot be accessed
        return 1; 
    }

    int processes, resources; // variables storing number of processes and number of resource types
    file >> processes >> resources; // read these two values from the file

    vector<int> E(resources); // vector representing total existing resources of each type
    vector<vector<int>> C(processes, vector<int>(resources)); // matrix storing currently allocated resources
    vector<vector<int>> R(processes, vector<int>(resources)); // matrix storing remaining resource requests
    vector<int> A(resources); // vector representing currently available resources
    vector<bool> finished(processes, false); // tracks which processes have successfully completed

    for (int i = 0; i < resources; i++) { 
        file >> E[i]; // read the total number of that resource from the file
    }

    for (int i = 0; i < processes; i++) { /
        for (int j = 0; j < resources; j++) { // loop through each resource type
            file >> C[i][j]; // read how many resources of type j are allocated to process i
        }
    }

    for (int i = 0; i < processes; i++) { 
        for (int j = 0; j < resources; j++) { // loop through each resource type
            file >> R[i][j]; // read the remaining resource requests for process i
        }
    }

    for (int j = 0; j < resources; j++) { 
        int totalAllocated = 0; 

        for (int i = 0; i < processes; i++) { // go through all processes
            totalAllocated += C[i][j]; // add how much of resource j each process currently holds
        }

        A[j] = E[j] - totalAllocated; // compute how many resources of type j are currently available
    }

    bool progress = true; // flag used to track if any process finishes during an iteration

    while (progress) { // repeat the search while progress is still possible
        progress = false; // assume no process can finish unless proven otherwise

        for (int i = 0; i < processes; i++) { // check every process in the system

            if (!finished[i]) { // only evaluate processes that have not finished yet

                bool canFinish = true; // assume the process can finish unless a request cannot be satisfied

                for (int j = 0; j < resources; j++) { // check each resource request

                    if (R[i][j] > A[j]) { // if the process requests more resources than are available
                        canFinish = false; // mark that the process cannot currently finish
                        break; 
                    }
                }

                if (canFinish) { // if all requested resources can be satisfied

                    for (int j = 0; j < resources; j++) { // iterate through all resource types
                        A[j] += C[i][j]; // release the process’s allocated resources back into availability
                    }

                    finished[i] = true; // mark the process as completed
                    progress = true; // indicate that progress was made during this iteration
                }
            }
        }
    }
    bool deadlock = false; // variable used to determine if deadlock exists

    for (int i = 0; i < processes; i++) { // examine every process
        if (!finished[i]) { // if a process was never completed
            deadlock = true; // a deadlock condition exists
            break; //when deadlock is confirmed
        }
    }
    if (deadlock) { // if deadlock was detected
        cout << "Deadlock detected." << endl; // notify the user

        cout << "Deadlocked processes: "; // print header for the list

        for (int i = 0; i < processes; i++) { // loop through all processes again
            if (!finished[i]) { // any unfinished process is part of the deadlock
                cout << "P" << i << " "; 
            }
        }
        cout << endl; 
    }

    else { // if no deadlock was found
        cout << "No deadlock detected." << endl; 
    }

    file.close(); 
    return 0;
}

#include <bits/stdc++.h> 
using namespace std; 

struct Frame { // structure representing a page frame in the memory
    int page; // storing variable for page number
    unsigned long long age; // register for aging algorithm
    long long lastUsed; // time of last use used to break ties between the pages

    Frame() : page(-1), age(0), lastUsed(0) {} // constructor that shows frame is still empty
};

vector<int> readReferences(const string& path) { // function that reads page references from a file
    ifstream in(path); 
    if (!in) { // incase the file failed to open
        throw runtime_error("Cannot open input file"); // error message that appears if the file fails to open
    }

    vector<int> refs; // vector to store page references
    int x;

    while (in >> x) { // reads all integers in the file
        refs.push_back(x); // adds each read page number to the vector variable
    }

    return refs; 
}

long long simulateAging(const vector<int>& refs, int numFrames, int bits) { // function that simulates the aging paging algorithm
    unsigned long long MSB = 1ULL << (bits - 1); // creates a mask to set the most important bit

    vector<Frame> frames(numFrames); // creates the list of frames based on the frame count
    long long faults = 0; // variable that counts the number of faults
    long long time = 0; // variable that tracks the current time 

    for (int page : refs) { 
        time++; 

        for (auto &f : frames) { 
            f.age >>= 1; // shifts the aging register right by one bit
        }

        int hitIndex = -1; // for if a page hit occurs

        for (int i = 0; i < numFrames; i++) { 
            if (frames[i].page == page) { // checks if the page is already in memory
                hitIndex = i; // stores the index where the page was found
                break; // breaks the loop so it stops the search once it was found
            }
        }

        if (hitIndex != -1) { 
            frames[hitIndex].age |= MSB; // sets the most significant bit to mark recent use
            frames[hitIndex].lastUsed = time; // updates the last used time of that frame
            continue; // skips the rest of the loop since it was a hit
        }

        faults++; 

        int emptyIndex = -1; 

        for (int i = 0; i < numFrames; i++) { 
            if (frames[i].page == -1) { // checks if the frame is empty
                emptyIndex = i; // stores the empty frame index
                break; // breaks the loop once the empty index is found
            }
        }

        int victimIndex; 

        if (emptyIndex != -1) { // checks if an empty frame exists
            victimIndex = emptyIndex; // uses the empty frame instead of replacing one
        } else { // runs if all frames are already full
            victimIndex = 0; 

            for (int i = 1; i < numFrames; i++) { 
                if (frames[i].age < frames[victimIndex].age) { // compares aging register values
                    victimIndex = i; // selects the frame with the smallest age value
                } 
                else if (frames[i].age == frames[victimIndex].age) { 
                    if (frames[i].lastUsed < frames[victimIndex].lastUsed) { // checks which frame was used earlier
                        victimIndex = i; // selects the least recently used frame
                    }
                }
            }
        }

        frames[victimIndex].page = page; // replaces the victim frame with the new page
        frames[victimIndex].age = MSB; // sets the MSB to mark it as recently used
        frames[victimIndex].lastUsed = time; // updates the last used time of the frame
    }

    return faults; 
}

int main(int argc, char* argv[]) { 
    if (argc < 3) { 
        cout << "Usage: ./program inputfile maxFrames\n"; // prints usage instructions
        return 1; 
    }

    string filename = argv[1]; 
    int maxFrames = stoi(argv[2]); // converts the second argument to an integer for frame count
    int bits = 8; 

    vector<int> refs = readReferences(filename); // reads the page references from the file

    int totalRefs = refs.size(); // calculates the total number of memory references

    cout << "Frames,Faults per 1000 references\n"; 

    for (int f = 1; f <= maxFrames; f++) { 
        long long faults = simulateAging(refs, f, bits); // runs the simulation for the current frame count

        double faultsPer1000 = (double)faults / totalRefs * 1000; // calculates faults per 1000 references

        cout << f << "," << faultsPer1000 << endl; 
    }

    return 0; 
}

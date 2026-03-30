#include <iostream>              
#include <filesystem>            // for working with files and directories
#include <map>                   // for storing histogram data
using namespace std;             
namespace fs = std::filesystem;  // shorten filesystem name

int main(int argc, char* argv[]) 
{
    if (argc != 3)               // check if user gave correct inputs
    {
        cout << "Usage: ./program <directory> <bin_size>" << endl; // show correct usage
        return 1;                // stop program
    }

    string folder = argv[1];     // get starting directory from user
    int binSize = stoi(argv[2]); // convert bin size from string to integer

    map<int, int> histogram;     // key = bin start, value = number of files

    for (auto& entry : fs::recursive_directory_iterator(folder)) 
    {
        if (fs::is_regular_file(entry)) // check if it's a file 
        {
            int size = fs::file_size(entry); // get file size in bytes

            int bin = (size / binSize) * binSize; // calculate which bin it belongs to

            histogram[bin]++; 
        }
    }

    cout << endl;               
    cout << "Histogram:" << endl; 

    for (auto& pair : histogram) 
    {
        int start = pair.first;            
        int end = start + binSize - 1;     
        int count = pair.second;           // number of files in the bin

        cout << start << "-" << end << ": "; // print range

        for (int i = 0; i < count; i++)    // print stars for visualization
        {
            cout << "*";                  // print one star per file
        }

        cout << " (" << count << ")" << endl; // show count and move to next line
    }

    return 0; 
}

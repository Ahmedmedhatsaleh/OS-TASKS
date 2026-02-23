#include <iostream>
#include <fstream> //this is used to read from files
#include <vector>
#include <thread> // this allows for the program to run multiple threads
#include <map> // used to count and store word frequencies
#include <cctype>

using namespace std;

// Function to clean words
string wordcleaning(string word) // removes punctiuation and converts letters to lowercase
{
    string cleaned = ""; // stores the words after it gets cleaned

    for (char c : word)
    {
        if (isalnum(c)) // to check whether the character is a letter or a number to remove commas, exclamation marks, etc
        {
            cleaned += tolower(c); // after cleaning the character, it adds it to the string "cleaned"
        }
    }

    return cleaned;
}

// Function executed by each thread
void countingwords(vector<string> &words, int start, int end, map<string, int> &localmap) 
{
    for (int i = start; i < end; i++)
    {
        string w = wordcleaning(words[i]); // Clean the current word using the cleaning function

        if (w != "") // to ignore any empty words
        {
            localmap[w]++; // to increase the count of words
        }
    }
}

int main()
{
    string filename = "Task 2";
    int N = 4; // number of threads

    ifstream file(filename); // open the file for reading

    if (!file) // incase the file does not open
    {
        cout << "Error opening file: " << filename << endl;  // stops the program since an error occurs
        return 1;
    }

    vector<string> words;  // stores the words from the selected file
    string word; // a temporary variable

    // Read all words
    while (file >> word)
    {
        words.push_back(word); // add each word to the vector
    }

    int total = words.size(); // number of words in the file
    int segment = total / N; // divides the number of words by the number of threads

    vector<thread> threads;  // stores thread objects
    vector<map<string, int>> threadmaps(N); // map for each thread

    // Create threads
    for (int i = 0; i < N; i++)
    {
        int start = i * segment; // to know where the thread starts
        int end;

        if (i == N - 1)
            end = total;
        else
            end = start + segment;

        threads.push_back(thread(countingwords,
                                 ref(words),
                                 start,
                                 end,
                                 ref(threadmaps[i])));  // creates the threads and runs the function "counting words"
    }

    // Wait for threads to finish
    for (auto &t : threads)
    {
        t.join();
    }

    map<string, int> finalcount; // stores all the results combined

    cout << endl;
    cout << "Function output: " << endl;

    // Print thread results
    for (int i = 0; i < N; i++)
    {
        cout << endl;
        cout << "Thread " << i + 1 << ":" << endl;

        for (auto &p : threadmaps[i])
        {
            cout << p.first << " : " << p.second << endl;
            finalcount[p.first] += p.second;
        }
    }

    cout << endl;
    cout << "Final Word Frequency:" << endl;

    for (auto &p : finalcount) // Loop through each word in that thread's map
    {
        cout << p.first << " : " << p.second << endl;
    }

    return 0;
}
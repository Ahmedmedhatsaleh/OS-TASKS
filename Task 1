#include <iostream>      
#include <unistd.h>      // for getcwd(), chdir(), fork(), execvp()
#include <dirent.h>      // for reading the directories 
#include <sys/types.h>   // for system defined data types
#include <sys/wait.h>    // for waitpid()
#include <fcntl.h>       // for use of file control function
#include <vector>        
#include <sstream>       
#include <cstring>       // for C string functions
#include <cstdlib>       // for exit(), setenv()
#include <fstream>       
#include <limits>        

using namespace std;

// prompt display:

// This function is responsible for showing the command prompt in the shell and ending with " > "
void promptdisplay() {
    char cwd[1024];  // Buffer to store the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // getcwd gets current directory
        cout << cwd << " > ";  // Print prompt
    } else {
        perror("getcwd failed");  // output error if getcwd fails
    }
}

// CD command:

// this function changes the current directory or prints current in the case that there are no argument 
void directorychange(vector<string> &args) {
    char cwd[1024];
    if (args.size() == 1) {  // No argument, so it will print current directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) cout << cwd << endl;
        else perror("getcwd");  // error trying to get the directory
        return;
    }
    // Try to change directory to args[1]
    if (chdir(args[1].c_str()) != 0) perror("cd failed");  // prints error if failed to change or print directory
    else if (getcwd(cwd, sizeof(cwd)) != NULL) setenv("PWD", cwd, 1);  // Update the PWD if there is no error
}

// DIR command:

// This function lists all files and folders of the directory specified in args[1] and prints tehir names line by line if they exist

void directorylist(vector<string> &args, ostream &out = cout) {
    if (args.size() < 2) { out << "dir: missing directory argument" << endl; return; }
    DIR *dir = opendir(args[1].c_str());  // Opens the directory
    if (!dir) { perror("dir failed"); return; }  // Error if the directory cannot open
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) out << entry->d_name << endl;  // Prints each file or folder in the directory
    closedir(dir);  // Closes the directory stream
}

// environ command:

// Prints all environment variables
void environlisting(ostream &out = cout) {
    extern char **environ;  // Pointer to environment variables
    for (int i = 0; environ[i] != nullptr; i++) out << environ[i] << endl;
}

// Set command:

// This function creates a newo enviroment variable or updates an environment variable that already exists
void set_variable(vector<string> &args) {
    if (args.size() < 3) { cerr << "set: missing VARIABLE or VALUE" << endl; return; }
    if (setenv(args[1].c_str(), args[2].c_str(), 1) != 0) perror("set failed"); // setenv updates or creates
}

// Echo command:

//This function displays the text provided by the user, separating words by spaces and ending with a newline
void commandecco(vector<string> &args, ostream &out = cout) {
    if (args.size() < 2) { out << endl; return; }  // Nothing to print so it goes top a new line
    for (size_t i = 1; i < args.size(); i++) {
        out << args[i];  // Prints each argument
        if (i != args.size()-1) out << " ";  // Add space between words
    }
    out << endl;  
}

// Help command: 

// This function prints a manual that shows all the shell's functions
void commandhelp(ostream &out = cout) {
    out <<
        "Custom Shell Help:\n"
        "1. cd - Change directory or show current\n"
        "2. dir  - List contents of the directory\n"
        "3. environ - List all environment variables\n"
        "4. set - Set/change environment variable\n"
        "5. echo - Display comment\n"
        "6. help - Display this help manual\n"
        "7. pause - Pause shell until Enter is clicked\n"
        "8. quit - Quit the shell\n" << endl;
}

// Pause command:

// Pauses the shell until Enter is pressed
void commandpause() {
    cout << "Shell paused. Press Enter to continue..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Wait for Enter to be pressed
}

// external program execution: 

// This function runs non-built-in programs by creating a child process, supporting input and output redirection and background execution
void externalexecution(vector<string> &args, bool background, string inputfile, string outputfile, bool append) {
    pid_t pid = fork();  // Fork child process
    if (pid < 0) { perror("fork failed"); return; }  // Fork failed
    else if (pid == 0) {
        // Input redirection
        if (!inputfile.empty()) {
            int fd = open(inputfile.c_str(), O_RDONLY);  // Open input file
            if (fd < 0) { perror("Input redirection failed"); exit(1); }
            dup2(fd, STDIN_FILENO);  // Redirect stdin
            close(fd);  // Close original file descriptor
        }

        // Output redirection
        if (!outputfile.empty()) {
            int fd;
            if (append)
                fd = open(outputfile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            else
                fd = open(outputfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("Output redirection failed"); exit(1); }
            dup2(fd, STDOUT_FILENO);  // Redirect stdout
            close(fd);  // Close original file descriptor
        }
        // the shell converts the command and its arguments from C++ strings into a C style array of pointers (char*), which execvp requires to run the program
        vector<char*> c_args;
        for (auto &a : args) c_args.push_back(&a[0]);
        c_args.push_back(nullptr);  // Null terminated array

        if (execvp(c_args[0], c_args.data()) < 0) {
            perror("exec failed");  // outputs error if program cannot run
            exit(1);
        }
    } else {
        // Parent process
        if (!background) waitpid(pid, nullptr, 0);  // Wait for child if foreground
        else cout << "Process running in background with PID: " << pid << endl;  // Show PID
    }
}

// Parse and execute function:

// This function reads a user input line, breaks it into arguments, handles redirection and background execution, and then runs the command either as an internal function or an external program
void parsecommand(string &line, bool batch_mode = false) {
    stringstream ss(line);
    string word;
    vector<string> args;
    string inputfile = "", outputfile = "";
    bool append = false, background = false;

    // Tokenize input and handle special characters
    while (ss >> word) {
        if (word == "<") { ss >> inputfile; continue; }       // Input redirection
        if (word == ">" || word == ">>") {                    // Output redirection
            append = (word == ">>");
            ss >> outputfile;
            continue;
        }
        if (word == "&") { background = true; continue; }    // Background execution
        args.push_back(word);  
    }

    if (args.empty()) return;  // Nothing to execute

    // Prepare output stream for redirection of commands
    ofstream ofs;
    ostream* outstream = &cout;
    if (!outputfile.empty()) {
        if (append) ofs.open(outputfile, ios::out | ios::app);
        else ofs.open(outputfile, ios::out | ios::trunc);
        if (!ofs) { cerr << "Cannot open output file: " << outputfile << endl; return; }
        outstream = &ofs;
    }

    // Handling the commands
    if (args[0] == "cd") directorychange(args);
    else if (args[0] == "dir") directorylist(args, *outstream);
    else if (args[0] == "environ") environlisting(*outstream);
    else if (args[0] == "set") set_variable(args);
    else if (args[0] == "echo") commandecco(args, *outstream);
    else if (args[0] == "help") commandhelp(*outstream);
    else if (args[0] == "pause") commandpause();
    else if (args[0] == "quit") exit(0);  // Quit shell
    else externalexecution(args, background, inputfile, outputfile, append);  // External program
}
int main(int argc, char* argv[]) {
    bool batch_mode = false;
    ifstream batchfile;

    // Check if batch file is provided as command line argument

    if (argc > 1) {
        batchfile.open(argv[1]);  // Open batch file
        if (!batchfile) { cerr << "Cannot open batch file: " << argv[1] << endl; return 1; }
        batch_mode = true;
    }
    string line;
    while (true) {
        if (!batch_mode) promptdisplay();  // Display prompt if interactive mode
        if (batch_mode) {
            if (!getline(batchfile, line)) break;  // End of batch file so exit
            cout << line << endl;  // Echo batch command to screen
        } else {
            if (!getline(cin, line)) break;  // Exit on EOF
        }
        parsecommand(line, batch_mode);  // Parse and execute the command line
    }
    return 0;
}

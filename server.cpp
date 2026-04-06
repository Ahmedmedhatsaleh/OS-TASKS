#include <iostream>          
#include <cstring>           // Used for string functions like strcmp and memset
#include <unistd.h>          // Used for close() function
#include <arpa/inet.h>       // Used for internet operations (IP, htons, etc.)
#include <sys/socket.h>      // Contains socket programming functions
using namespace std;         

int main()
{
    int serverSocket, clientSocket;   // variable for the server Socket which is the main server and a socket for the client connected to the main server.
    sockaddr_in serverAddr, clientAddr; // Structures to store address info
    socklen_t clientSize = sizeof(clientAddr); // Size of the client address

    char buffer[1024];        // Array to store messages with a maximum of 1024 characters

    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP

    if (serverSocket < 0)     // Check if the socket creation failed
    {
        cout << "Error creating socket." << endl; // Prints the error message
        return 1;             
    }

    
    serverAddr.sin_family = AF_INET;           // Use IPv4
    serverAddr.sin_port = htons(5000);         // Port number 
    serverAddr.sin_addr.s_addr = INADDR_ANY;   // Accept connections from any IP

    
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "Error in binding." << endl;   // Print error if binding fails
        return 1;                             
    }
    
    if (listen(serverSocket, 1) < 0)           // 1 is the maximum number of waiting clients
    {
        cout << "Error in listening." << endl; 
        return 1;                             
    }

    cout << "Server is waiting for a client on port 5000..." << endl; // Status message
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientSize);

    if (clientSocket < 0)                      // Check if the client connection failed
    {
        cout << "Error accepting client." << endl;
        return 1;
    }

    cout << "Client connected." << endl;       // Confirms the connection

    while (true)                              // Infinite loop for continuous chat between the client and server
    {
        memset(buffer, 0, sizeof(buffer));    // Clear buffer before receiving new data

        // Receive message from client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)               // If nothing received or connection closed
        {
            cout << "Client disconnected." << endl;
            break;                            
        }

        cout << "Client: " << buffer << endl; // Display client's message

        // Exit the condition
        if (strcmp(buffer, "exit") == 0)      // Compare message with "exit"
        {
            cout << "Chat ended by client." << endl;
            break;                           
        }
        
        cout << "Server: ";                   
        cin.getline(buffer, sizeof(buffer));  // Read full line input

        send(clientSocket, buffer, strlen(buffer), 0); // Send message to client

        if (strcmp(buffer, "exit") == 0)      // Check if server typed "exit"
        {
            cout << "Chat ended by server." << endl;
            break;                            
        }
    }
    close(clientSocket);      
    close(serverSocket);     
    return 0;                
}

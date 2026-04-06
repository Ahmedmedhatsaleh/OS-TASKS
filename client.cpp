#include <iostream>          
#include <cstring>           // For string functions
#include <unistd.h>          // For close()
#include <arpa/inet.h>       // For IP address functions
#include <sys/socket.h>      // For socket functions
using namespace std;         

int main()
{
    int clientSocket;        // Socket variable for the client
    sockaddr_in serverAddr;  // Structure to store server address
    char buffer[1024];       // Buffer to store messages

    clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket

    if (clientSocket < 0)    // Check if socket creation failed
    {
        cout << "Error creating socket." << endl;
        return 1;
    }

    
    serverAddr.sin_family = AF_INET;               // IPv4
    serverAddr.sin_port = htons(5000);             // Same port as the server
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost IP

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "Connection failed." << endl;      // If the connection fails
        return 1;
    }

    cout << "Connected to server." << endl;        // Confirms the connection

    while (true)                                  // Loop for continuous chat between the client and the server
    {
        
        cout << "Client: ";                       // Prompt for the user
        cin.getline(buffer, sizeof(buffer));      // Read the input of the client

        send(clientSocket, buffer, strlen(buffer), 0); // Send message

        if (strcmp(buffer, "exit") == 0)          // If the user types exit
        {
            cout << "Chat ended by client." << endl;
            break;                                // Exit the loop
        }

        memset(buffer, 0, sizeof(buffer));        // Clear buffer

        
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)                   // If server disconnects
        {
            cout << "Server disconnected." << endl;
            break;
        }

        cout << "Server: " << buffer << endl;     // Display server message

        if (strcmp(buffer, "exit") == 0)          // If server ends chat
        {
            cout << "Chat ended by server." << endl;
            break;
        }
    }

    close(clientSocket);      // Closes the socket variable

    return 0;                 
}

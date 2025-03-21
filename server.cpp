#include <iostream>       // Standard input/output for printing messages
#include <unistd.h>       // Provides close() function
#include <sys/socket.h>   // For socket functions like socket(), bind(), listen(), accept()
#include <netinet/in.h>   // Contains sockaddr_in struct (Internet address)
#include <cstring>        // For memset() to initialize memory to 0

// Define the port the server will run on (you can change this if needed)
#define SERVER_PORT 8080

// Function to handle the client's HTTP request and send a response
void handle_client_request(int client_socket) {
    char received_data[1024] = {0};  // Buffer to store received data (up to 1024 bytes)

    // Step 1: Read the data sent by the client
    read(client_socket, received_data, 1024);  // Reads from the socket into the buffer

    // Display the received request in the console (for debugging purposes)
    std::cout << "Client request:\n" << received_data << std::endl;

    // Step 2: Prepare the HTTP response (a simple "200 OK" response with plain text)
    std::string http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello, this is your server response!";  // Custom message

    // Step 3: Send the HTTP response back to the client
    send(client_socket, http_response.c_str(), http_response.length(), 0);

    // Step 4: Close the client connection after sending the response
    close(client_socket);
}

int main() {
    int server_socket, client_socket;  
    struct sockaddr_in server_address;  // Structure to hold server's address info
    int address_length = sizeof(server_address);  // Store the size of the address structure

    //  Create a TCP socket (IPv4, stream type)
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);  // Exit if socket creation fails
    }

    //  Set the server's address (IPv4, port 8080)
    server_address.sin_family = AF_INET;                 // Use IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;         // Allow connections from any IP address
    server_address.sin_port = htons(SERVER_PORT);        // Convert port to network byte order

    //  Bind the socket to the server's IP address and port
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);  // Exit if binding the socket fails
    }

    //  Start listening for incoming connections (max 10 clients in the queue)
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);  // Exit if listening for connections fails
    }

    // Server is now listening for connections on the specified port
    std::cout << "Server is listening on port " << SERVER_PORT << "..." << std::endl;

    // Main loop to accept and handle client connections
    while (true) {
        // Accept a new client connection (blocking call)
        client_socket = accept(server_socket, (struct sockaddr*)&server_address, (socklen_t*)&address_length);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;  // Continue listening for other clients if accept fails
        }

        // Step 6: Handle the client's request by calling the handle_client_request function
        handle_client_request(client_socket);
    }

    
    close(server_socket);
    return 0;  // End of the program
}

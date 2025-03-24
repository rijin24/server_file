#include <iostream>       // Standard input/output for printing messages
#include <unistd.h>       // Provides close() function
#include <sys/socket.h>   // For socket functions like socket(), bind(), listen(), accept()
#include <netinet/in.h>   // Contains sockaddr_in struct (Internet address)
#include <cstring>        // For memset() to initialize memory to 0
#include <sstream>   // For stringstream and istringstream
#include <fstream>   // For ifstream (file input)

// Define the port the server will run on (you can change this if needed)
#define SERVER_PORT 8080

// Function to handle the client's HTTP request and send a response
void handle_client_request(int client_socket) {
    char buffer[2048] = {0};  
    read(client_socket, buffer, 2048);  
    std::cout << "Client request:\n" << buffer << std::endl;

    std::istringstream request(buffer);
    std::string method, path, version;
    request >> method >> path >> version;

    std::string response;

    // Check if it's a GET request and the path is the root "/"
    if (method == "GET" && path == "/") {
        // Serve the index.html file
        std::ifstream file("index.html");  // Open the file
        if (file) {
            std::stringstream file_content;
            file_content << file.rdbuf();  // Read file content
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + file_content.str();
        } else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found!";
        }
    } else {
        response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMethod not supported!";
    }

    // Send the response to the client
    send(client_socket, response.c_str(), response.length(), 0);
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

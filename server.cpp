#include <iostream>       // Standard input/output for printing messages
#include <unistd.h>       // Provides close() function
#include <sys/socket.h>   // For socket functions like socket(), bind(), listen(), accept()
#include <netinet/in.h>   // Contains sockaddr_in struct (Internet address)
#include <cstring>        // For memset() to initialize memory to 0
#include <sstream>   // For stringstream and istringstream
#include <fstream>   // For ifstream (file input)
#include <algorithm>

// Define the port the server will run on (you can change this if needed)
#define SERVER_PORT 8080

// Function to handle the client's HTTP request and send a response
void handle_client_request(int client_socket) {
    char buffer[4096] = {0}; 
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    std::cout << "Received Request:\n" << buffer << std::endl;  // Debug: Log raw request

    std::istringstream request(buffer);
    std::string method, path, version;
    request >> method >> path >> version;

    std::cout << "Method: " << method << "\n";  // Debug: Check extracted method

        if (method == "POST") {
        // Find the body (data sent after the headers)
        std::string body;
        std::string header(buffer);
        size_t body_pos = header.find("\r\n\r\n");

        if (body_pos != std::string::npos) {
            body = header.substr(body_pos + 4); // Get everything after the headers

            std::replace(body.begin(), body.end(), '+', ' ');
            // Save the message to a file
            std::string message;
            size_t pos = body.find("message="); 
            if (pos != std::string::npos) {
                message = body.substr(pos + 8);
            }

            // Save the message to a file
            std::ofstream outFile("messages.txt", std::ios::app);
            if (outFile) {
                outFile << message << std::endl; 
            }
            outFile.close();

            // Send a response to the client
            std::string response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 20\r\n"
                "\r\n"
                "Message receiveddd!";
            send(client_socket, response.c_str(), response.length(), 0);
        } else {
            // If the body is missing or there's an issue with the request
            std::string response = "HTTP/1.1 400 Bad Request\r\n\r\nNo body found in POST request!";
            send(client_socket, response.c_str(), response.length(), 0);
        }

    } else if (method == "GET") {
        std::ifstream file("index.html");
        if (!file) {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\nFile not found!";
            send(client_socket, response.c_str(), response.length(), 0);
        } else {
            std::stringstream file_content;
            file_content << file.rdbuf();
            file.close();
            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(file_content.str().size()) + "\r\n"
                "\r\n" +
                file_content.str();
            send(client_socket, response.c_str(), response.length(), 0);
        }
    } else {
        std::cout << "Unsupported method detected: " << method << std::endl;  // Debug
        std::string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\nMethod not supporting!";
        send(client_socket, response.c_str(), response.length(), 0);
    }

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

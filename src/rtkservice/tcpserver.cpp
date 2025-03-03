#include "tcpserver.h"

#define BACKLOG 10
#define BUFFER_SIZE 1024

TCPServer::TCPServer(int port) : port(port) {
    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, BACKLOG) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << std::endl;
}

TCPServer::~TCPServer() {
    close(server_socket);
}

void TCPServer::start() {
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int client_socket;

    // Main loop to accept and handle client connections
    while (true) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);
        if (client_socket == -1) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "Connection received from " << inet_ntoa(client_addr.sin_addr) << std::endl;

        // Read the first byte to check if it starts with '%'
        char initial_byte;
        ssize_t bytes_received = recv(client_socket, &initial_byte, 1, MSG_PEEK);
        if (bytes_received == -1) {
            std::cerr << "recv() error: " << strerror(errno) << std::endl;
            close(client_socket);
            continue;
        }

        if (initial_byte == '%') {
            // Handle the client in the same process
            std::cout << "Startup message received from server" << std::endl;
            close(client_socket);
        } else {
            // Fork a new process to handle the client
            pid_t pid = fork();
            if (pid == -1) {
                std::cerr << "Fork failed: " << strerror(errno) << std::endl;
                close(client_socket);
                continue;
            } else if (pid == 0) {
                // Child process
                close(server_socket); // Close the listening socket in the child process
                handle_client(client_socket);
                exit(EXIT_SUCCESS);
            } else {
                // Parent process
                close(client_socket); // Close the client socket in the parent process
            }
        }
    }
}

void TCPServer::handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Communicate with the client
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Received: " << buffer << std::endl;

        // Echo the message back to the client
        send(client_socket, buffer, bytes_received, 0);

        // Run the function in a new process
        run_function();
    }

    if (bytes_received == -1) {
        std::cerr << "recv() error: " << strerror(errno) << std::endl;
    }

    close(client_socket);
    std::cout << "Client disconnected." << std::endl;
}

void TCPServer::run_function() {
    // Implement the function you want to run in a new process
    std::cout << "Running function in a new process." << std::endl;
}


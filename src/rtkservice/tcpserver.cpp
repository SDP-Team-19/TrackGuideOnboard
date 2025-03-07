#include "tcpserver.h"
#include "buttons.h"

#define BACKLOG 10
#define BUFFER_SIZE 1024


TCPServer::TCPServer(int port, LEDControl& led_controller, std::atomic<SystemState>& systemState) : ledController_(led_controller), systemState_(systemState) {
    // Create a socket
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
        close_server();
        exit(EXIT_FAILURE);
    }

    // Configure server address
    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(port);
    serverAddr_.sin_addr.s_addr = INADDR_ANY;
    memset(&(serverAddr_.sin_zero), '\0', 8);

    // Bind the socket
    if (bind(serverSocket_, (struct sockaddr *)&serverAddr_, sizeof(struct sockaddr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close_server();
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket_, BACKLOG) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close_server();
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << std::endl;
}


TCPServer::~TCPServer() {
    close_server();
}

void TCPServer::start(std::atomic<bool>& shutdown_requested) {
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int client_socket;

    // Main loop to accept and handle client connections
    while (!shutdown_requested.load(std::memory_order_acquire)) {

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(serverSocket_, &read_fds);

        struct timeval timeout = {1, 0};  // 1 second timeout
        std::cout << "Waiting for activity..." << std::endl;
        int activity = select(serverSocket_ + 1, &read_fds, NULL, NULL, &timeout);
        if (activity == -1) {
            if (errno == EINTR) continue; // Retry if interrupted by signal
            std::cerr << "select() failed: " << strerror(errno) << std::endl;
            return;
        }
        if (activity == 0) continue;  // Timeout expired, check shutdown_requested


        client_socket = accept(serverSocket_, (struct sockaddr *)&client_addr, &sin_size);

        if (client_socket == -1) {
            if (errno == EINTR) return; // Allow shutdown
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        if (shutdown_requested.load(std::memory_order_acquire)) {
            close(client_socket);
            return; // Exit loop if shutdown is requested
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
            ledController_.indicate_startup_message();
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
                close_server(); // Close the listening socket in the child process
                handle_client(client_socket);
                exit(EXIT_SUCCESS);
            } else {
                // Parent process
                close(client_socket); // Close the client socket in the parent process
            }
        }
    }
    std::cout << "Server shutting down..." << std::endl;
    close_server();
}

void TCPServer::handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    SystemState currentState = systemState_.load(std::memory_order_relaxed);

    // Communicate with the client
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Received: " << buffer << std::endl;

        // Echo the message back to the client
        send(client_socket, buffer, bytes_received, 0);

        // Run the function in a new process
        if (currentState == SystemState::RECORDING) {
            run_record_function(buffer);
        }else if (currentState == SystemState::PLAYING)
        {
            /* code */
        }else if (currentState == SystemState::RESETTING)
        {

            systemState_.store(SystemState::STANDBY, std::memory_order_relaxed);
        }
        
        
    }

    if (bytes_received == -1) {
        std::cerr << "recv() error: " << strerror(errno) << std::endl;
    }

    close(client_socket);
    std::cout << "Client disconnected." << std::endl;
}

void TCPServer::run_record_function(const char* content) {
    // Implement the function you want to run in a new process
    std::cout << "Running record function in a new process." << std::endl;

    // Example content received
    // Extract the latitude and longitude values
    std::istringstream iss(content);
    std::string date, time;
    double latitude, longitude;

    iss >> date >> time >> latitude >> longitude;

    // Use a mutex to avoid race conditions when writing to the file
    static std::mutex file_mutex;
    std::lock_guard<std::mutex> lock(file_mutex);

    // Open the CSV file in append mode
    std::ofstream outfile("coordinates.csv", std::ios_base::app);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open coordinates.csv" << std::endl;
        return;
    }

    // Write the latitude and longitude to the CSV file
    outfile << latitude << ", " << longitude << std::endl;

    // Close the file
    outfile.close();

    std::cout << "Latitude: " << latitude << ", Longitude: " << longitude << " saved to coordinates.csv" << std::endl;
}

void TCPServer::run_play_function(const char* content) {
    // Implement the function you want to run in a new process
    std::cout << "Running play function in a new process." << std::endl;
}

void TCPServer::run_reset_function() {
    // Implement the function you want to run in a new process
    std::cout << "Running reset function in a new process." << std::endl;
}

void TCPServer::close_server() {
    if (serverSocket_ != -1) {
        close(serverSocket_);
    }
    serverSocket_ = -1;
    std::cout << "Server closed." << std::endl;
}

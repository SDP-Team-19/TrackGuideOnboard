#include "tcpserver.h"
#include "buttons.h"
#include <sys/mman.h>   // For shm_open, mmap, etc.
#include <fcntl.h>      // For O_* constants
#include <unistd.h>     // For ftruncate
#include <sys/stat.h>   // For mode constants
#include <semaphore.h>  // For semaphores
#include <semaphoreguard.h>
#include <thread>       // For std::this_thread::sleep_for

std::ostream& operator<<(std::ostream& os, const SystemState& state) {
    switch (state) {
        case SystemState::STANDBY:
            os << "STANDBY";
            break;
        case SystemState::RECORDING:
            os << "RECORDING";
            break;
        case SystemState::PLAYING:
            os << "PLAYING";
            break;
        case SystemState::RESETTING:
            os << "RESETTING";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}

#define BACKLOG 10
#define BUFFER_SIZE 1024
#define SHM_NAME "/tcpserver_shm"
#define SEM_NAME "/tcpserver_sem"
#define SHM_SIZE sizeof(SharedMemory)

TCPServer::TCPServer(int port, LEDControl& led_controller, States& states, SharedMemory* shared_memory, sem_t* semaphore) 
    : ledController_(led_controller), states_(states), shared_memory_(shared_memory), semaphore_(semaphore) {
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
    std::future<void> animation_future;
    std::atomic<bool> stop_animation(false);

    // Start animation in background
    auto run_animation = [&stop_animation, this]() {
        while (!stop_animation.load(std::memory_order_acquire)) {
            ledController_.led_location_bounce_animation(Color::BLUE, 3);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    };

    // Start animation immediately
    animation_future = std::async(std::launch::async, run_animation);

    // Main loop to accept and handle client connections
    while (!shutdown_requested.load(std::memory_order_acquire)) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(serverSocket_, &read_fds);

        struct timeval timeout = {1, 0};  // 1 second timeout
        int activity = select(serverSocket_ + 1, &read_fds, NULL, NULL, &timeout);
        
        if (activity == -1) {
            if (errno == EINTR) continue;
            std::cerr << "select() failed: " << strerror(errno) << std::endl;
            return;
        }

        if (activity == 0) {
            // If no connection and animation isn't running, restart it
            if (!animation_future.valid()) {
                animation_future = std::async(std::launch::async, run_animation);
            }
            continue;
        }

        client_socket = accept(serverSocket_, (struct sockaddr *)&client_addr, &sin_size);

        if (client_socket == -1) {
            if (errno == EINTR) return;
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        if (shutdown_requested.load(std::memory_order_acquire)) {
            close(client_socket);
            break;
        }

        std::cout << "Connection received from " << inet_ntoa(client_addr.sin_addr) << std::endl;

        char initial_byte;
        if (recv(client_socket, &initial_byte, 1, MSG_PEEK) == -1) {
            std::cerr << "recv() error: " << strerror(errno) << std::endl;
            close(client_socket);
            continue;
        }

        if (initial_byte == '%') {
            std::cout << "Startup message received from server" << std::endl;
            close(client_socket);
        } else {
            if (animation_future.valid()) {
                std::cout << "Stopping animation" << std::endl;
                stop_animation.store(true, std::memory_order_release);
                animation_future.wait(); // Wait for the animation thread to complete
                animation_future = std::future<void>();
            }
            std::cout << "Handling Client" << std::endl;
            handle_client(client_socket, shutdown_requested);
        }
    }

    // Ensure animation thread is stopped
    if (animation_future.valid()) {
        stop_animation.store(true, std::memory_order_release);
        animation_future.wait(); // Wait for the animation thread to complete
        animation_future = std::future<void>();
    }
    ledController_.clear();
    std::cout << "Server shutting down..." << std::endl;
    close_server();
}


void TCPServer::handle_client(int client_socket, std::atomic<bool>& shutdown_requested) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Communicate with the client
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0 && !shutdown_requested.load(std::memory_order_acquire)) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Received: " << buffer << std::endl;

        // Echo the message back to the client
        send(client_socket, buffer, bytes_received, 0);

        // Acquire the semaphore before accessing shared memory
        SemaphoreGuard semaphore_guard(semaphore_);
        if (!semaphore_guard.acquired()) {
            close(client_socket);
            return;
        }
        std::cout << "semaphore claimed" << std::endl;

        // Run the function in a new process
        if (shared_memory_->state == SystemState::RECORDING) {
            std::cout << "Recording" << std::endl;
            states_.run_record_function(buffer);
        } else if (shared_memory_->state == SystemState::PLAYING) {
            std::cout << "Playing" << std::endl;
            states_.run_play_function(buffer);
        } else if (shared_memory_->state == SystemState::RESETTING) {
            std::cout << "Resetting" << std::endl;
            shared_memory_->state = SystemState::STANDBY;
            states_.run_reset_function();
        } else if (shared_memory_->state == SystemState::STANDBY)
        {
            states_.run_standby_function();
        }

        // Release the semaphore after accessing shared memory
        if (sem_post(semaphore_) == -1) {
            std::cerr << "sem_post() failed: " << strerror(errno) << std::endl;
            close(client_socket);
            return;
        }
    }

        if (bytes_received == -1) {
        std::cerr << "recv() error: " << strerror(errno) << std::endl;
    }

    close(client_socket);
    std::cout << "Client disconnected." << std::endl;
}

void TCPServer::close_server() {
    if (serverSocket_ != -1) {
        close(serverSocket_);
    }
    serverSocket_ = -1;

    std::cout << "Server closed." << std::endl;
}

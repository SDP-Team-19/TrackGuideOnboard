#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sstream>
#include <fstream>
#include <mutex>
#include "ledcontrol.h"
#include "buttons.h"
#include <atomic>
#include "boundarylogic.h"
#include "states.h"
#include <sys/mman.h>   // For shm_open, mmap, etc.
#include <fcntl.h>      // For O_* constants
#include <sys/stat.h>   // For mode constants
#include <semaphore.h>  // For semaphores

#define BUFFER_SIZE 1024

struct SharedMemory;

class TCPServer {
public:
    TCPServer(int port, LEDControl ledController, States& states, SharedMemory* shared_memory, sem_t* semaphore);
    ~TCPServer();
    void start(std::atomic<bool>& shutdown_requested);

private:
    int serverSocket_;
    LEDControl ledController_;
    States& states_;
    struct sockaddr_in serverAddr_;

    SharedMemory* shared_memory_;
    sem_t* semaphore_;

    void handle_client(int clientSocket, std::atomic<bool>& shutdown_requested);
    void close_server();
};

#endif // TCP_SERVER_H

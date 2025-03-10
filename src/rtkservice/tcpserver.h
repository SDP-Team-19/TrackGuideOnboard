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

class TCPServer {
public:
    TCPServer(int port, LEDControl ledController, std::atomic<SystemState>& systemState, States& states);
    ~TCPServer();
    void start(std::atomic<bool>& shutdown_requested);

private:
    int serverSocket_;
    LEDControl ledController_;
    std::atomic<SystemState>& systemState_;
    States& states_;
    struct sockaddr_in serverAddr_;

    void handle_client(int clientSocket);
    void close_server();
};

#endif // TCP_SERVER_H
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

class TCPServer {
public:
    TCPServer(int port, LEDControl ledController, std::atomic<SystemState>& systemState);
    ~TCPServer();
    void start(std::atomic<bool>& shutdown_requested);

private:
    int serverSocket_;
    LEDControl ledController_;
    std::atomic<SystemState>& systemState_;
    struct sockaddr_in serverAddr_;

    void handle_client(int clientSocket);
    void run_record_function(const char* content);
    void run_play_function(const char* content);
    void run_reset_function();
};

#endif // TCP_SERVER_H
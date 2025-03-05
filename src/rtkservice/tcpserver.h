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

class TCPServer {
public:
    TCPServer(int port, LEDControl led_controller);
    ~TCPServer();
    void start();

private:
    int _server_socket;
    LEDControl _led_controller;
    struct sockaddr_in _server_addr;

    void handle_client(int client_socket);
    void run_function(const char* content);
};

#endif // TCP_SERVER_H
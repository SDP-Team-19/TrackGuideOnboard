#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class TCPServer {
public:
    TCPServer(int port);
    ~TCPServer();
    void start();

private:
    int server_socket;
    int port;
    struct sockaddr_in server_addr;

    void handle_client(int client_socket);
    void run_function();
};

#endif // TCP_SERVER_H
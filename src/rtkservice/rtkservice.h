#ifndef RTKSERVICE_H
#define RTKSERVICE_H

#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class RTKService {
public:
    RTKService(const std::string& config_file);
    void start_server() const;
    int shutdown_server() const;

private:
    std::string config_file_;
};

#endif // RTKSERVICE_H
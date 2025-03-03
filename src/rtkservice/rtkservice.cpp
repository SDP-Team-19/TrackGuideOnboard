#include "rtkservice.h"
#define PORT 12346

RTKService::RTKService(const std::string& config_file) : config_file_(config_file) {}

void RTKService::start_server() const {
    std::string command = "rtkrcv -o " + config_file_ + " -t 0 -s -nc --deamon -w \"\" -p " + std::to_string(PORT);
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to start RTK server with command: " << command << std::endl;
    }
}

int RTKService::shutdown_server() const {
    std::string command = "(echo -e \"shutdown\\r\"; sleep 1) | telnet localhost " + std::to_string(PORT);
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to shutdown RTK server with command: " << command << std::endl;
        return -1;
    }
    return 0;
}
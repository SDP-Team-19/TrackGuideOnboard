#include "rtkservice.h"

RTKService::RTKService(const std::string& config_file) : config_file_(config_file) {}

void RTKService::start_server() const {
    std::string command = "rtkrcv -o " + config_file_ + " -t 0 -s -nc --deamon";
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to start RTK server with command: " << command << std::endl;
    }
}

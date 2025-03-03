#include "rtkservice.h"
#define PORT 12346

RTKService::RTKService(const std::string& config_file) : config_file_(config_file) {}

void RTKService::start_server() const {
    std::string command = "rtkrcv -o " + config_file_ + " -t 0 -s -nc --deamon -p" + std::to_string(PORT) + " -w \"\"";
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to start RTK server with command: " << command << std::endl;
    }
}

int RTKService::shutdown_server() const {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // Port number
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    const char *shutdown_cmd = "shutdown\r\n";
    if (send(sockfd, shutdown_cmd, strlen(shutdown_cmd), 0) == -1) {
        perror("Attempt to issue the shutdown command failed");
        close(sockfd);
        return 1;
    }
    std::cout << "RTK server has been shutdown gracefully" << std::endl;
    close(sockfd);
    return 0;
}
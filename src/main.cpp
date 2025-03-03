#include "tcpserver.h"
#include "rtkservice.h"
#define PORT 12345

int main() {
    RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv_no_logs.conf");
    rtk_service.start_server();
    TCPServer server(PORT);
    server.start();
    return EXIT_SUCCESS;
}
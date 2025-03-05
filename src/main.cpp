#include <csignal>
#include "tcpserver.h"
#include "rtkservice.h"
#include "ledcontrol.h"
#define PORT 12345

RTKService* rtk_service_ptr = nullptr;
LEDControl* led_control_ptr = nullptr;

#include <iostream>

void signal_handler(int signal) {
    if (signal == SIGINT && rtk_service_ptr) {
        std::cout << "Shutting down RTK service..." << std::endl;
        led_control_ptr->clear();
        if (rtk_service_ptr->shutdown_server() == 1) {
            std::cerr << "Failed to shut down RTK service.  Please check if the server is running. Try to shut it down gracefully with `telnet localhost 12346` and type `shutdown` (If that does not work, `ps aux | grep [r]tkrcv`, and kill the process)" << std::endl;
        }
    }
    exit(EXIT_SUCCESS);
}

int main() {
    LEDControl led_control(12, 30);
    led_control_ptr = &led_control;
    led_control.indicate_all(Color::GREEN);
    RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv_no_logs.conf");
    rtk_service_ptr = &rtk_service;
    std::signal(SIGINT, signal_handler);

    rtk_service.start_server();
    TCPServer server(PORT, led_control);
    server.start();
    return EXIT_SUCCESS;
}
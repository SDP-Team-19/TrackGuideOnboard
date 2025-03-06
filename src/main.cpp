#include <csignal>
#include "tcpserver.h"
#include "rtkservice.h"
#include "ledcontrol.h"
#include "buttons.h"
#include <pigpio.h>
#include <iostream>
#include <thread>
#define PORT 12345

RTKService* rtk_service_ptr = nullptr;
LEDControl* led_control_ptr = nullptr;

std::atomic<SystemState> system_state(SystemState::STANDBY);

std::thread button_thread;


void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Shutting down RTK service..." << std::endl;
        if (rtk_service_ptr)
        {
            if (rtk_service_ptr->shutdown_server() == 1) {
                std::cerr << "Failed to shut down RTK service.  Please check if the server is running. Try to shut it down gracefully with `telnet localhost 12346` and type `shutdown` (If that does not work, `ps aux | grep [r]tkrcv`, and kill the process)" << std::endl;
            }
        }
        if (led_control_ptr) {
            led_control_ptr->clear();
        }
        if (button_thread.joinable()) {
            button_thread.join();
        }
        gpioTerminate();
    }
    exit(EXIT_SUCCESS);
}

int main() {
    std::signal(SIGINT, signal_handler);
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed." << std::endl;
        return 1;
    }

    LEDControl led_control(12, 30);
    led_control_ptr = &led_control;
    led_control.indicate_all(Color::GREEN);
    usleep(3000000);
    RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv_no_logs.conf");
    rtk_service_ptr = &rtk_service;
    rtk_service.start_server();
    Buttons buttons(16, 20, 21);
    std::thread button_thread(&Buttons::monitor_button, &buttons);
    button_thread.detach();
    TCPServer server(PORT, led_control, system_state);
    server.start();
    return EXIT_SUCCESS;
}
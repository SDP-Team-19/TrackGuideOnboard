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
std::atomic<bool> shutdown_requested(false);  // Atomic flag

void signal_handler(int signal) {
    if (signal == SIGINT) {
        shutdown_requested.store(true, std::memory_order_relaxed);
        std::cout << "Interrupt received. Shutting down..." << std::endl;
    }else if (signal == SIGCHLD) {
        std::cout << "Reaping child" << std::endl;
        // Prevent zombie processes
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGCHLD, signal_handler);

    LEDControl led_control(12, 30);
    led_control_ptr = &led_control;
    led_control.indicate_all(Color::GREEN);
    usleep(3000000);

    RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv_no_logs.conf");
    rtk_service_ptr = &rtk_service;
    rtk_service.start_server();

    Buttons buttons(16, 20, 21);
    std::thread button_thread(&Buttons::monitor_button, &buttons, std::ref(shutdown_requested));

    TCPServer server(PORT, led_control, system_state);
    server.start(shutdown_requested);

    std::cout << "Shutting down safely..." << std::endl;

    if (rtk_service_ptr) {
        std::cout << "Shutting down rtk service" << std::endl;
        rtk_service_ptr->shutdown_server();
    }
    if (led_control_ptr) {
        std::cout << "Shutting down led control" << std::endl;
        led_control_ptr->clear();
    }
    if (button_thread.joinable()) {
        button_thread.join();
    }
    gpioTerminate();

    return EXIT_SUCCESS;
}
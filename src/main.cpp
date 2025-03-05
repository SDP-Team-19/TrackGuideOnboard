#include <csignal>
#include "tcpserver.h"
#include "rtkservice.h"
#include "ledcontrol.h"
#include <pigpio.h>
#define PORT 12345

RTKService* rtk_service_ptr = nullptr;
LEDControl* led_control_ptr = nullptr;

#include <iostream>

// void signal_handler(int signal) {
//     if (signal == SIGINT && rtk_service_ptr) {
//         std::cout << "Shutting down RTK service..." << std::endl;
//         led_control_ptr->clear();
//         if (rtk_service_ptr->shutdown_server() == 1) {
//             std::cerr << "Failed to shut down RTK service.  Please check if the server is running. Try to shut it down gracefully with `telnet localhost 12346` and type `shutdown` (If that does not work, `ps aux | grep [r]tkrcv`, and kill the process)" << std::endl;
//         }
//     }
//     exit(EXIT_SUCCESS);
// }

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Shutting down GPIO service..." << std::endl;
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

    int recordButtonPin = 16; // GPIO16
    int resetButtonPin = 20; // GPIO20
    int playButtonPin = 21; // GPIO21
    gpioSetMode(recordButtonPin, PI_INPUT);
    gpioSetPullUpDown(recordButtonPin, PI_PUD_DOWN);
    gpioSetMode(resetButtonPin, PI_INPUT);
    gpioSetPullUpDown(resetButtonPin, PI_PUD_DOWN);
    gpioSetMode(playButtonPin, PI_INPUT);
    gpioSetPullUpDown(playButtonPin, PI_PUD_DOWN);

    while (true) {
        int recordButtonState = gpioRead(recordButtonPin);
        if (recordButtonState == PI_HIGH) {
            std::cout << "Record Button Pressed" << std::endl;
        }

        int resetButtonState = gpioRead(resetButtonPin);
        if (resetButtonState == PI_HIGH) {
            std::cout << "Reset Button Pressed" << std::endl;
        } else {
            std::cout << "Reset Button Released" << std::endl;
        }

        int playButtonState = gpioRead(playButtonPin);
        if (playButtonState == PI_HIGH) {
            std::cout << "Play Button Pressed" << std::endl;
        } else {
            std::cout << "Play Button Released" << std::endl;
        }

        sleep(1); // Delay for 1 second
        
    }
    gpioTerminate();
    return 0;

    // LEDControl led_control(12, 30);
    // led_control_ptr = &led_control;
    // led_control.indicate_all(Color::GREEN);
    // usleep(3000000);
    // RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv_no_logs.conf");
    // rtk_service_ptr = &rtk_service;
    // std::signal(SIGINT, signal_handler);

    // rtk_service.start_server();
    // TCPServer server(PORT, led_control);
    // server.start();
    // return EXIT_SUCCESS;
}
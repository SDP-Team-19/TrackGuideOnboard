#include <csignal>
#include "tcpserver.h"
#include "rtkservice.h"
#include "ledcontrol.h"
#include "buttons.h"
#include "states.h"
#include <pigpio.h>
#include <iostream>
#include <thread>
#include <sys/mman.h>   // For shm_open, mmap, etc.
#include <fcntl.h>      // For O_* constants
#include <unistd.h>     // For ftruncate
#include <sys/stat.h>   // For mode constants
#include <semaphore.h>  // For semaphores
#include <apiclient.h>

#define PORT 12345
#define SHM_NAME "/tcpserver_shm"
#define SEM_NAME "/tcpserver_sem"
#define SHM_SIZE sizeof(SharedMemory)

RTKService* rtk_service_ptr = nullptr;
LEDControl* led_control_ptr = nullptr;

std::atomic<SystemState> system_state(SystemState::STANDBY);
std::atomic<bool> shutdown_requested(false);  // Atomic flag

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        shutdown_requested.store(true, std::memory_order_release);
        std::cout << "Interrupt received. Shutting down..." << std::endl;
    } else if (signal == SIGCHLD) {
        std::cout << "Reaping child" << std::endl;
        // Prevent zombie processes
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}

int main() {
    // std::signal(SIGINT, signal_handler);
    // std::signal(SIGTERM, signal_handler);
    // std::signal(SIGCHLD, signal_handler);

    // gpioCfgSetInternals(1 << 10);

    // // Create shared memory
    // int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    // if (shm_fd == -1) {
    //     std::cerr << "Failed to create shared memory: " << strerror(errno) << std::endl;
    //     return EXIT_FAILURE;
    // }
    // if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    //     std::cerr << "Failed to set size of shared memory: " << strerror(errno) << std::endl;
    //     return EXIT_FAILURE;
    // }
    // SharedMemory* shared_memory = static_cast<SharedMemory*>(mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    // if (shared_memory == MAP_FAILED) {
    //     std::cerr << "Failed to map shared memory: " << strerror(errno) << std::endl;
    //     return EXIT_FAILURE;
    // }

    // // Create semaphore
    // sem_t* semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    // if (semaphore == SEM_FAILED) {
    //     std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
    //     return EXIT_FAILURE;
    // }

    LEDControl led_control(19, 27);
    led_control_ptr = &led_control;
    led_control.indicate_all(Color::GREEN);
    usleep(3000000);
    led_control.clear()

    // Test different distances from 0.0 to 1.0
    for (float distance = 0.0; distance <= 100.0; distance += 0.5) {
        led_control.test_interpolate(distance, Color::RED, Color::GREEN);
        usleep(500000);  // Wait 500ms between each test
    }

    // Buttons buttons(16, 20, 21, shared_memory, semaphore);
    // std::thread button_thread(&Buttons::monitor_button, &buttons, std::ref(shutdown_requested));

    // BoundaryLogic boundary_logic;
    // States states(led_control, boundary_logic);

    // RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv.conf");
    // rtk_service_ptr = &rtk_service;
    // rtk_service.start_server();
    
    // TCPServer server(PORT, led_control, states, shared_memory, semaphore);
    // server.start(shutdown_requested);

    // std::cout << "Shutting down safely..." << std::endl;

    // if (rtk_service_ptr) {
    //     std::cout << "Shutting down rtk service" << std::endl;
    //     rtk_service_ptr->shutdown_server();
    //     rtk_service_ptr = nullptr;
    // }
    // if (led_control_ptr) {
    //     std::cout << "Shutting down led control" << std::endl;
    //     led_control_ptr->clear();
    //     led_control_ptr = nullptr;
    // }
    // if (button_thread.joinable()) {
    //     button_thread.join();
    // }
    // gpioTerminate();

    // // Cleanup shared memory and semaphore
    // munmap(shared_memory, SHM_SIZE);
    // close(shm_fd);
    // shm_unlink(SHM_NAME);
    // sem_close(semaphore);
    // sem_unlink(SEM_NAME);

    // ApiClient api_client;
    // while (!shutdown_requested.load(std::memory_order_acquire)) {
    //     nlohmann::json location_request = api_client.createLocationRequest(37.7749, -122.4194);
    //     nlohmann::json mode_request = api_client.createModeRequest(0.5, "test_mode");
    //     nlohmann::json location_response = api_client.sendPostRequest(location_request);
    //     // nlohmann::json mode_response = api_client.sendPostRequest("frontend-computer:8081", mode_request);
    //     // std::cout << "Location Response: " << location_response.dump(4) << std::endl;
    //     // std::cout << "Mode Response: " << mode_response.dump(4) << std::endl;

    //     std::this_thread::sleep_for(std::chrono::seconds(3));
    // }

    return EXIT_SUCCESS;
}

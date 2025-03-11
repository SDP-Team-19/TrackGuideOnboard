// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/inputoutput/buttons.cpp
#include "buttons.h"
#include <pigpio.h>
#include <stdexcept>
#include <iostream>
#include <sys/mman.h>   // For shm_open, mmap, etc.
#include <fcntl.h>      // For O_* constants
#include <unistd.h>     // For ftruncate
#include <sys/stat.h>   // For mode constants
#include <semaphore.h>  // For semaphores
#include <tcpserver.h>

Buttons::Buttons(uint8_t recordPin, uint8_t resetPin, uint8_t playPin, SharedMemory* shared_memory, sem_t* semaphore)
    : recordPin_(recordPin), resetPin_(resetPin), playPin_(playPin), shared_memory_(shared_memory), semaphore_(semaphore) {
    prevPlayButtonState_ = ButtonState::RELEASED;
    prevRecordButtonState_ = ButtonState::RELEASED;
    prevResetButtonState_ = ButtonState::RELEASED;
    if (gpioInitialise() < 0) {
        // Initialization failed
        throw std::runtime_error("pigpio initialization failed");
    }
    std::cout << "pigpio initialized" << std::endl;
    gpioSetMode(recordPin_, PI_INPUT);
    gpioSetMode(resetPin_, PI_INPUT);
    gpioSetMode(playPin_, PI_INPUT);
    gpioSetPullUpDown(recordPin_, PI_PUD_UP);
    gpioSetPullUpDown(resetPin_, PI_PUD_UP);
    gpioSetPullUpDown(playPin_, PI_PUD_UP);
}

void Buttons::monitor_button(std::atomic<bool>& shutdown_requested) {
    ButtonState recordState = ButtonState::RELEASED;
    ButtonState resetState = ButtonState::RELEASED;
    ButtonState playState = ButtonState::RELEASED;
    while (!shutdown_requested.load(std::memory_order_acquire)) {
        recordState = get_button_state(recordPin_);
        resetState = get_button_state(resetPin_);
        playState = get_button_state(playPin_);

        // Lock the semaphore to access shared memory
        sem_wait(semaphore_);
        SystemState currentSystemState = shared_memory_->state;

        if (recordState == ButtonState::PRESSED && prevRecordButtonState_ == ButtonState::RELEASED) {
            // Record button pressed
            if (currentSystemState == SystemState::RECORDING) {
                shared_memory_->state = SystemState::STANDBY;
                std::cout << "Record button pressed, running in standby" << std::endl;
            } else {
                shared_memory_->state = SystemState::RECORDING;
                std::cout << "Record button pressed, running in record" << std::endl;
            }
        }

        if (resetState == ButtonState::PRESSED && prevResetButtonState_ == ButtonState::RELEASED) {
            // Reset button pressed
            shared_memory_->state = SystemState::RESETTING;
            std::cout << "Reset button pressed" << std::endl;
        }

        if (playState == ButtonState::PRESSED && prevPlayButtonState_ == ButtonState::RELEASED) {
            // Play button pressed
            if (currentSystemState == SystemState::PLAYING) {
                shared_memory_->state = SystemState::STANDBY;
                std::cout << "Play button pressed, running in standby" << std::endl;
            } else {
                shared_memory_->state = SystemState::PLAYING;
                std::cout << "Play button pressed, running in play" << std::endl;
            }
        }

        // Unlock the semaphore
        sem_post(semaphore_);

        prevRecordButtonState_ = recordState;
        prevResetButtonState_ = resetState;
        prevPlayButtonState_ = playState;

        // Add a small delay to prevent high CPU usage
        gpioDelay(200000); // 100 milliseconds
    }
    std::cout << "Button monitoring thread shutting down..." << std::endl;
}

ButtonState Buttons::get_button_state(uint8_t pin) {
    if (gpioRead(pin) == PI_LOW) {
        return ButtonState::PRESSED;
    } else {
        return ButtonState::RELEASED;
    }
}
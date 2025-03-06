// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/inputoutput/buttons.cpp
#include "buttons.h"
#include <pigpio.h>
#include <stdexcept>
#include <iostream>

Buttons::Buttons(uint8_t recordPin, uint8_t resetPin, uint8_t playPin)
    : recordPin_(recordPin), resetPin_(resetPin), playPin_(playPin), systemState_(SystemState::STANDBY) {
    prevPlayButtonState_ = ButtonState::RELEASED;
    prevRecordButtonState_ = ButtonState::RELEASED;
    prevResetButtonState_ = ButtonState::RELEASED;
    if (gpioInitialise() < 0) {
        // Initialization failed
        throw std::runtime_error("pigpio initialization failed");
    }
    gpioSetMode(recordPin_, PI_INPUT);
    gpioSetMode(resetPin_, PI_INPUT);
    gpioSetMode(playPin_, PI_INPUT);
    gpioSetPullUpDown(recordPin_, PI_PUD_UP);
    gpioSetPullUpDown(resetPin_, PI_PUD_UP);
    gpioSetPullUpDown(playPin_, PI_PUD_UP);
}

void Buttons::monitor_button() {
    ButtonState recordState = ButtonState::RELEASED;
    ButtonState resetState = ButtonState::RELEASED;
    ButtonState playState = ButtonState::RELEASED;
    while (true) {
        recordState = get_button_state(recordPin_);
        resetState = get_button_state(resetPin_);
        playState = get_button_state(playPin_);
        SystemState currentSystemState = systemState_.load(std::memory_order_relaxed);

        if (recordState == ButtonState::PRESSED && prevRecordButtonState_ == ButtonState::RELEASED) {
            // Record button pressed
            if (currentSystemState == SystemState::RECORDING) {
                systemState_.store(SystemState::STANDBY, std::memory_order_relaxed);
                std::cout << "Record button pressed" << std::endl;
            } else {
                systemState_.store(SystemState::RECORDING, std::memory_order_relaxed);
                std::cout << "Record button pressed" << std::endl;
            }
        }

        if (resetState == ButtonState::PRESSED && prevResetButtonState_ == ButtonState::RELEASED) {
            // Reset button pressed
            systemState_.store(SystemState::RESETTING, std::memory_order_relaxed);
            std::cout << "Reset button pressed" << std::endl;
        }

        if (playState == ButtonState::PRESSED && prevPlayButtonState_ == ButtonState::RELEASED) {
            // Play button pressed
            if (currentSystemState == SystemState::PLAYING) {
                systemState_.store(SystemState::STANDBY, std::memory_order_relaxed);
                std::cout << "Play button pressed" << std::endl;
            } else {
                systemState_.store(SystemState::PLAYING, std::memory_order_relaxed);
                std::cout << "Play button pressed" << std::endl;
            }
            
        }
        prevRecordButtonState_ = recordState;

        // Add a small delay to prevent high CPU usage
        gpioDelay(10000); // 10 milliseconds
    }
}

ButtonState Buttons::get_button_state(uint8_t pin) {
    if (gpioRead(pin) == PI_LOW) {
        return ButtonState::PRESSED;
    } else {
        return ButtonState::RELEASED;
    }
}
#ifndef BUTTON_H
#define BUTTON_H

#include <atomic>
#include <cstdint>
#include <semaphore.h>
#include "tcpserver.h"

enum class SystemState {
    STANDBY,
    RECORDING,
    PLAYING,
    RESETTING
};

enum class ButtonState {
    PRESSED,
    RELEASED
};

class Buttons {
public:
    Buttons(uint8_t recordPin, uint8_t resetPin, uint8_t playPin, SharedMemory* shared_memory, sem_t* semaphore);
    void monitor_button(std::atomic<bool>& shutdown_requested);

private:
    uint8_t recordPin_;
    uint8_t resetPin_;
    uint8_t playPin_;
    SharedMemory* shared_memory_;
    sem_t* semaphore_;
    ButtonState prevRecordButtonState_;
    ButtonState prevResetButtonState_;
    ButtonState prevPlayButtonState_;
    ButtonState get_button_state(uint8_t pin);
};

#endif // BUTTON_H
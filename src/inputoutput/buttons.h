#ifndef BUTTON_H
#define BUTTON_H

#include <atomic>
#include <cstdint>

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
    Buttons(uint8_t recordPin, uint8_t resetPin, uint8_t playPin);
    void monitor_button(std::atomic<bool>& shutdown_requested);

private:
    uint8_t recordPin_;
    uint8_t resetPin_;
    uint8_t playPin_;
    std::atomic<SystemState> systemState_;
    ButtonState prevRecordButtonState_;
    ButtonState prevResetButtonState_;
    ButtonState prevPlayButtonState_;
    ButtonState get_button_state(uint8_t pin);
};

#endif // BUTTON_H
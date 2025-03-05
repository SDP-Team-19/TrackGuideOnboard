#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <cstdint>
#include <iostream>
extern "C" {
    #include <ws2811.h>
}

enum class Color {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    WHITE,
    OFF
};

class LEDControl {
public:
    LEDControl(uint8_t gpioPin, uint16_t stripLength);
    void indicate_left(Color color);
    void indicate_right(Color color);
    void indicate_both(Color color);
    void indicate_all(Color color);
    void clear();

private:
    uint16_t _stripLength;
    ws2811_t _ledstring;
    ws2811_led_t map_color(Color color);
};

#endif // LEDCONTROL_H
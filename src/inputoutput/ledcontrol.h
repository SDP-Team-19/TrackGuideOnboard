#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <cmath>
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

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorChannels;



class LEDControl {
public:
    LEDControl(uint8_t gpioPin, uint16_t stripLength);
    void indicate_left(Color color);
    void indicate_right(Color color);
    void indicate_both(Color color);
    void indicate_all(Color color);
    void indicate_startup_message();
    void update_leds(float distance);
    void clear();

private:
    uint16_t _stripLength;
    ws2811_t _ledstring;
    ws2811_led_t map_color(Color color);
    float mapDistanceToRatio(float distance, float minDistance, float maxDistance);
    ColorChannels interpolateColor(ColorChannels startColor, ColorChannels endColor, float ratio);
};

#endif // LEDCONTROL_H
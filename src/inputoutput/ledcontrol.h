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
    LEDControl(uint8_t gpioPin, uint16_t stripLength, double maxDistance);
    void indicate_left(Color color);
    void indicate_right(Color color);
    void indicate_both(Color color);
    void indicate_all(Color color);
    void indicate_startup_message();
    void indicate_record_startup();
    void test_interpolate(double distance, Color startColor, Color endColor);
    void clear();
    void set_led_location(double distance, Color color);

private:
    uint16_t _stripLength;
    ws2811_t _ledstring;
    double _maxDistance;
    ws2811_led_t map_color(Color color);
    float mapDistanceToRatio(double distance, double minDistance, double maxDistance);
    ColorChannels interpolateColor(ColorChannels startColor, ColorChannels endColor, float ratio);
    ws2811_led_t map_color_channels(ColorChannels color);
};

#endif // LEDCONTROL_H
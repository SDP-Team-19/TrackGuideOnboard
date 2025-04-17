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
    LEDControl(uint8_t gpioPin, uint16_t stripLength, double maxDistance, double red_threshold = 3, double yellow_threshold = 3);
    void indicate_left(Color color);
    void indicate_right(Color color);
    void indicate_both(Color color);
    void indicate_all(Color color);
    void indicate_startup_message();
    void indicate_record_startup();
    ws2811_led_t get_interpolated_breaking_color(double current_speed, double expected_speed, Color startColor, Color midColor, Color endColor);
    void test_interpolate(double distance, Color startColor, Color endColor);
    void clear();
    void set_led_location(double distance, ws2811_led_t color, int pixel_width = 3);
    ws2811_led_t map_color(Color color);
    void led_location_bounce_animation(Color color, int pixel_width);

private:
    uint16_t _stripLength;
    ws2811_t _ledstring;
    double _maxDistance;
    double _red_threshold;
    double _yellow_threshold;
    float mapDistanceToRatio(double distance, double minDistance, double maxDistance);
    ColorChannels interpolateColor(ColorChannels startColor, ColorChannels endColor, float ratio);
    ws2811_led_t map_color_channels(ColorChannels color);
};

#endif // LEDCONTROL_H
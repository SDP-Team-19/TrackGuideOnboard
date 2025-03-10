// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/ledcontrol/ledcontrol.cpp
#include "ledcontrol.h"

LEDControl::LEDControl(uint8_t gpioPin, uint16_t stripLength)
    : _stripLength(stripLength) {
    std::cout << "LED strip initialized on GPIO pin " << static_cast<int>(gpioPin) 
              << " with length " << stripLength << std::endl;

    // Initialize the ws2811 structure
    _ledstring = {
        .freq = WS2811_TARGET_FREQ,
        .dmanum = 10,
        .channel = {
            [0] = {
                .gpionum = 0,
                .invert = 0,
                .count = 0,
                .brightness = 0,
            },
            [1] = {
                .gpionum = gpioPin,
                .invert = 0,
                .count = stripLength,
                .strip_type = WS2812_STRIP,
                .brightness = 255,
            },
        },
    };

    // Initialize the library
    if (ws2811_init(&_ledstring) != WS2811_SUCCESS) {
        std::cerr << "ws2811_init failed" << std::endl;
    }
}

void LEDControl::indicate_left(Color color) {
    std::cout << "Indicating left with color " << static_cast<int>(color) << std::endl;

    ws2811_led_t led_color = map_color(color);
    for (int i = 0; i < 5; ++i) {
        _ledstring.channel[0].leds[i] = led_color;
    }

    ws2811_render(&_ledstring);
}

void LEDControl::indicate_right(Color color) {
    std::cout << "Indicating right with color " << static_cast<int>(color) << std::endl;

    ws2811_led_t led_color = map_color(color);
    for (int i = _stripLength - 5; i < _stripLength; ++i) {
        _ledstring.channel[0].leds[i] = led_color;
    }

    ws2811_render(&_ledstring);
}

void LEDControl::indicate_both(Color color) {
    std::cout << "Indicating both with color " << static_cast<int>(color) << std::endl;

    ws2811_led_t led_color = map_color(color);
    indicate_left(color);
    indicate_right(color);
    ws2811_render(&_ledstring);
}

void LEDControl::indicate_all(Color color) {
    std::cout << "Indicating all with color " << static_cast<int>(color) << std::endl;

    ws2811_led_t led_color = map_color(color);
    for (int i = 0; i < _stripLength; ++i) {
        _ledstring.channel[0].leds[i] = led_color;
    }

    ws2811_render(&_ledstring);
}

void LEDControl::indicate_startup_message() {
    std::cout << "Indicating startup message" << std::endl;

    for (int i = 0; i < 3; ++i) {
        indicate_all(Color::BLUE);
        usleep(250000);
        indicate_all(Color::OFF);
        usleep(250000);
    }
}

void LEDControl::clear() {
    std::cout << "Clearing the LED strip" << std::endl;

    for (int i = 0; i < _stripLength; ++i) {
        _ledstring.channel[0].leds[i] = 0x00000000;
    }

    ws2811_render(&_ledstring);
}

ws2811_led_t LEDControl::map_color(Color color) {
    switch (color) {
        case Color::RED:    return 0x00200000;
        case Color::GREEN:  return 0x00002000;
        case Color::BLUE:   return 0x00002020;
        case Color::YELLOW: return 0x00202000;
        case Color::WHITE:  return 0x00202020;
        case Color::OFF:    return 0x00000000;
        default:            return 0x00000000;
    }
}
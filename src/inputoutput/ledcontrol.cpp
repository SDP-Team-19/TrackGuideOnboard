// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/ledcontrol/ledcontrol.cpp
#include "ledcontrol.h"
#define LEFTRIGHTSIZE 14

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
    for (int i = 0; i < LEFTRIGHTSIZE; ++i) {
        _ledstring.channel[1].leds[i] = led_color;
    }

    ws2811_render(&_ledstring);
}

void LEDControl::indicate_right(Color color) {
    std::cout << "Indicating right with color " << static_cast<int>(color) << std::endl;

    ws2811_led_t led_color = map_color(color);
    for (int i = _stripLength - LEFTRIGHTSIZE; i < _stripLength; ++i) {
        _ledstring.channel[1].leds[i] = led_color;
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
        _ledstring.channel[1].leds[i] = led_color;
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

void LEDControl::indicate_record_startup() {
    std::cout << "Indicating startup message" << std::endl;

    for (int i = 0; i < 3; ++i) {
        indicate_all(Color::RED);
        usleep(250000);
        indicate_all(Color::OFF);
        usleep(250000);
    }
    indicate_all(Color::RED);
}

void LEDControl::update_leds(double distance) {
    // Define minimum and maximum distances for the gradient
    double minDistance = 0.0f;
    double maxDistance = 100.0f;
    int start = _stripLength-LEFTRIGHTSIZE;
    int end = _stripLength;

    // Define start (green) and end (red) colors
    ColorChannels green = {0, 255, 0};
    ColorChannels red = {255, 0, 0};

    // Calculate the interpolation ratio
    float ratio = mapDistanceToRatio(std::fabs(distance), minDistance, maxDistance);

    // Get the interpolated color
    ColorChannels currentColor = interpolateColor(green, red, ratio);

    // Update the LED strip with the current color
    if (distance > 0)
    {
        start = 0;
        end = LEFTRIGHTSIZE;
    }
    if (start >= 0 && start < _stripLength && end >= 0 && end <= _stripLength) {
        for (int i = start; i < end; ++i) {
            _ledstring.channel[0].leds[i] = (currentColor.r << 16) | (currentColor.g << 8) | currentColor.b;
        }
    } else {
        std::cerr << "Error: Invalid range: start = " << start << ", end = " << end << ", strip length = " << _stripLength << std::endl;
    }

    // Render the updated colors to the LED strip
    ws2811_render(&_ledstring);
}


void LEDControl::test_interpolate(double distance, Color startColor, Color endColor)
{
    std::cout << "Testing interpolation with distance: " << distance << std::endl;

    // Define minimum and maximum distances for the gradient
    double minDistance = 0.0f;
    double maxDistance = 100.0f;

    // Define start and end colors
    ColorChannels start = {0, 0, 0};
    ColorChannels end = {0, 0, 0};

    switch (startColor) {
        case Color::RED: start = {255, 0, 0}; break;
        case Color::GREEN: start = {0, 255, 0}; break;
        case Color::BLUE: start = {0, 0, 255}; break;
        case Color::YELLOW: start = {255, 255, 0}; break;
        case Color::WHITE: start = {255, 255, 255}; break;
        case Color::OFF: start = {0, 0, 0}; break;
    }

    switch (endColor) {
        case Color::RED: end = {255, 0, 0}; break;
        case Color::GREEN: end = {0, 255, 0}; break;
        case Color::BLUE: end = {0, 0, 255}; break;
        case Color::YELLOW: end = {255, 255, 0}; break;
        case Color::WHITE: end = {255, 255, 255}; break;
        case Color::OFF: end = {0, 0, 0}; break;
    }

    

    // Calculate the interpolation ratio
    float ratio = mapDistanceToRatio(std::fabs(distance), minDistance, maxDistance);

    // Get the interpolated color
    ColorChannels currentColor = interpolateColor(start, end, ratio);

    std::cout << "Interpolated color: R=" << static_cast<int>(currentColor.r)
              << ", G=" << static_cast<int>(currentColor.g)
              << ", B=" << static_cast<int>(currentColor.b) << std::endl;

    // Update the LED strip with the current color
    for (int i = _stripLength; i < _stripLength; ++i) {
        _ledstring.channel[1].leds[i] = map_color_channels(currentColor);
    }

    // Render the updated colors to the LED strip
    ws2811_render(&_ledstring);
}


void LEDControl::clear() {
    std::cout << "Clearing the LED strip" << std::endl;

    for (int i = 0; i < _stripLength; ++i) {
        _ledstring.channel[1].leds[i] = 0x00000000;
    }

    ws2811_render(&_ledstring);
}

ColorChannels LEDControl::interpolateColor(ColorChannels startColor, ColorChannels endColor, float ratio) {
    ColorChannels result;
    result.r = startColor.r + ratio * (endColor.r - startColor.r);
    result.g = startColor.g + ratio * (endColor.g - startColor.g);
    result.b = startColor.b + ratio * (endColor.b - startColor.b);
    return result;
}

float LEDControl::mapDistanceToRatio(double distance, double minDistance, double maxDistance) {
    if (minDistance == maxDistance) {
        // Handle the case where the range is zero
        return 0.0f; // or an appropriate value or error code
    }
    if (distance < minDistance) return 0.0f;
    if (distance > maxDistance) return 1.0f;
    return (distance - minDistance) / (maxDistance - minDistance);
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

ws2811_led_t LEDControl::map_color_channels(ColorChannels color) {
    return (color.r << 16) | (color.g << 8) | color.b;
}
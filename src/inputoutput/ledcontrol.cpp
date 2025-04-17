// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/ledcontrol/ledcontrol.cpp
#include "ledcontrol.h"
#include <vector>
#define LEFTRIGHTSIZE 14

LEDControl::LEDControl(uint8_t gpioPin, uint16_t stripLength, double maxDistance, double red_speed, double yellow_speed)
    : _stripLength(stripLength), _maxDistance(maxDistance),
      _red_threshold(red_speed), _yellow_threshold(yellow_speed) {
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

void LEDControl::led_location_bounce_animation(Color color, int pixel_width) {
    std::cout << "Starting LED location slide animation" << std::endl;
    ws2811_led_t mapped_color = map_color(color);

    // Forward Pass
    for (double i = -_maxDistance; i <= _maxDistance; i+=0.1) {
        set_led_location(i, mapped_color, pixel_width);
        usleep(500); // Sleep for 0.5ms
        clear()
    }
    // Backward Pass
    for (double i = _maxDistance; i >= -_maxDistance; i-=0.1) {
        set_led_location(i, mapped_color, pixel_width);
        usleep(500); // Sleep for 0.5ms
        clear();
    }
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

void LEDControl::set_led_location(double distance, ws2811_led_t color, int pixel_width) {
    std::cout << "Setting LED location with distance: " << distance 
              << ", pixel width: " << pixel_width 
              << ", and color: " << static_cast<int>(color) << std::endl;

    // Map the distance to a floating-point LED position
    double ledPosition = (distance + _maxDistance) * (_stripLength - 1) / (2 * _maxDistance);
    std::vector<double> ledBrightness(_stripLength);
    
    // Calculate brightness values for each LED based on pixel_width
    for (int i = 0; i < _stripLength; i++) {
        double distanceFromCenter = std::abs(i - ledPosition);
        if (distanceFromCenter <= pixel_width / 2.0) {
            // Create a triangular brightness distribution
            ledBrightness[i] = 1.0 - (2.0 * distanceFromCenter / pixel_width);
            ledBrightness[i] = std::max(0.0, std::min(1.0, ledBrightness[i]));
        }
    }
    
    // Extract RGB components
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // Apply brightness to each LED
    for (int i = 0; i < _stripLength; i++) {
        if (ledBrightness[i] > 0.0) {
            _ledstring.channel[1].leds[i] = ((static_cast<uint32_t>(r * ledBrightness[i]) << 16) |
                                           (static_cast<uint32_t>(g * ledBrightness[i]) << 8) |
                                           static_cast<uint32_t>(b * ledBrightness[i]));
        } else {
            _ledstring.channel[1].leds[i] = 0;
        }
    }

    // Render the updated colors to the LED strip
    ws2811_render(&_ledstring);
}

ws2811_led_t LEDControl::get_interpolated_breaking_color(double current_speed, double expected_speed, Color startColor, Color midColor, Color endColor){
    std::cout << "Interpolating breaking color with current velocity: " << current_speed 
              << ", expected velocity: " << expected_speed << std::endl;

    // Define start and end colors
    ColorChannels start = {0, 0, 0};
    ColorChannels mid = {0, 0, 0};
    ColorChannels end = {0, 0, 0};

    switch (startColor) {
        case Color::RED: start = {255, 0, 0}; break;
        case Color::GREEN: start = {0, 255, 0}; break;
        case Color::BLUE: start = {0, 0, 255}; break;
        case Color::YELLOW: start = {255, 255, 0}; break;
        case Color::WHITE: start = {255, 255, 255}; break;
        case Color::OFF: start = {0, 0, 0}; break;
    }

    switch (midColor)
    {
        case Color::RED: mid = {255, 0, 0}; break;
        case Color::GREEN: mid = {0, 255, 0}; break;
        case Color::BLUE: mid = {0, 0, 255}; break;
        case Color::YELLOW: mid = {255, 255, 0}; break;
        case Color::WHITE: mid = {255, 255, 255}; break;
        case Color::OFF: mid = {0, 0, 0}; break;
    }

    switch (endColor) {
        case Color::RED: end = {255, 0, 0}; break;
        case Color::GREEN: end = {0, 255, 0}; break;
        case Color::BLUE: end = {0, 0, 255}; break;
        case Color::YELLOW: end = {255, 255, 0}; break;
        case Color::WHITE: end = {255, 255, 255}; break;
        case Color::OFF: end = {0, 0, 0}; break;
    }

    // Calculate the difference between the current and expected velocities
    double difference = current_speed - expected_speed;
    float ratio;
    ColorChannels currentColor;

    if (difference <= 0) {
        // Under expected velocity - show green
        currentColor = start;
    } else if (difference <= _yellow_threshold) {
        // Interpolate between green and yellow
        ratio = difference / _yellow_threshold;
        currentColor = interpolateColor(start, mid, ratio);
    } else if (difference <= _yellow_threshold + _red_threshold) {
        // Interpolate between yellow and red
        ratio = (difference - _yellow_threshold) / _red_threshold;
        currentColor = interpolateColor(mid, end, ratio);
    } else {
        // Above thresholds - show red
        currentColor = end;
    }

    std::cout << "Interpolated color: R=" << static_cast<int>(currentColor.r)
              << ", G=" << static_cast<int>(currentColor.g)
              << ", B=" << static_cast<int>(currentColor.b) << std::endl;

    return map_color_channels(currentColor);
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
    for (int i = 0; i < _stripLength; ++i) {
        std::cout << "Updating LED " << i << " to color: R=" << static_cast<int>(currentColor.r)
                  << ", G=" << static_cast<int>(currentColor.g)
                  << ", B=" << static_cast<int>(currentColor.b) << std::endl;
        _ledstring.channel[1].leds[i] = map_color_channels(currentColor);
    }

    // Render the updated colors to the LED strip
    std::cout << "Rendering the LED strip" << std::endl;
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
    std::cout << "Mapping RGB: ("
              << static_cast<int>(color.r) << ", "
              << static_cast<int>(color.g) << ", "
              << static_cast<int>(color.b) << ")\n";
    
    ws2811_led_t mapped = (color.r << 16) | (color.g << 8) | color.b;
    std::cout << "Mapped value: 0x" << std::hex << mapped << std::dec << "\n";
    
    return mapped;
}
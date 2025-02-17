#include <iostream>
#include <cstring>

class ZEDF9P {
public:
    bool getModuleInfo() {
        // Implementation to get module info
        return true; // Placeholder
    }

    uint8_t getFirmwareVersionHigh() {
        // Implementation to get firmware version high
        return 1; // Placeholder
    }

    uint8_t getFirmwareVersionLow() {
        // Implementation to get firmware version low
        return 25; // Placeholder
    }

    const char* getFirmwareType() {
        // Implementation to get firmware type
        return "HPG"; // Placeholder
    }

    void confirmFirmware() {
        if (getModuleInfo()) {
            std::cout << "FWVER: " << (int)getFirmwareVersionHigh() << "." << (int)getFirmwareVersionLow() << std::endl;
            std::cout << "Firmware: " << getFirmwareType() << std::endl;

            if (strcmp(getFirmwareType(), "HPG") == 0) {
                if ((getFirmwareVersionHigh() == 1) && (getFirmwareVersionLow() < 30)) {
                    std::cout << "Your module is running old firmware which may not support SPARTN. Please upgrade." << std::endl;
                }
            }

            if (strcmp(getFirmwareType(), "HPS") == 0) {
                if ((getFirmwareVersionHigh() == 1) && (getFirmwareVersionLow() < 21)) {
                    std::cout << "Your module is running old firmware which may not support SPARTN. Please upgrade." << std::endl;
                }
            }
        } else {
            std::cout << "Error: could not read module info!" << std::endl;
        }
    }
};
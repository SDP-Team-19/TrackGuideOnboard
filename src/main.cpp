#include "MaCORS/MaCORS.h"

int main() {
    MaCORS request("MitchellSylvia", "myrkoz-tivpef-hibrA5");
    if (request.sendRequest()) {
        std::cout << "Request sent successfully" << std::endl;
    } else {
        std::cout << "Failed to send request" << std::endl;
    }
    return 0;
}
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "rtklib.h"

int main() {
    rtksvr_t server;
    rtksvrinit(&server);
    if (!rtksvrstart(&server)) {
        std::cerr << "failed to start server" << std::endl;
    }
    return 0;
}

#include "Example.h"

Example::Example() {
    std::cout << "Example object created!" << std::endl;
}

void Example::sayHello() const {
    std::cout << "Hello from Example class!" << std::endl;
}
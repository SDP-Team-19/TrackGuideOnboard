// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/inputoutput/semaphoreguard.cpp
#include "semaphoreguard.h"

SemaphoreGuard::SemaphoreGuard(sem_t* semaphore) : semaphore_(semaphore), acquired_(false) {
    if (sem_wait(semaphore_) == 0) {
        acquired_ = true;
    } else {
        std::cerr << "sem_wait() failed: " << strerror(errno) << std::endl;
    }
}

SemaphoreGuard::~SemaphoreGuard() {
    if (acquired_) {
        if (sem_post(semaphore_) == -1) {
            std::cerr << "sem_post() failed: " << strerror(errno) << std::endl;
        }
    }
}

bool SemaphoreGuard::acquired() const {
    return acquired_;
}
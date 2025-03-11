#ifndef SEMAPHOREGUARD_H
#define SEMAPHOREGUARD_H

#include <semaphore.h>
#include <iostream>
#include <cerrno>
#include <cstring>

class SemaphoreGuard {
public:
    SemaphoreGuard(sem_t* semaphore);
    ~SemaphoreGuard();
    bool acquired() const;

private:
    sem_t* semaphore_;
    bool acquired_;
};

#endif // SEMAPHOREGUARD_H

#ifndef RTKSERVICE_H
#define RTKSERVICE_H

#include <string>
#include <iostream>
#include <cstdlib>

class RTKService {
public:
    RTKService(const std::string& config_file);
    void start_server() const;

private:
    std::string config_file_;
};

#endif // RTKSERVICE_H
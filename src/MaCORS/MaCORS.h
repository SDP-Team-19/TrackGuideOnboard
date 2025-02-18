// filepath: /Users/mitchellsylvia/TrackGuideOnboard/src/MaCORS/MaCORS.h
#ifndef MACORS_H
#define MACORS_H

#include <iostream>
#include <string>
#include <curl/curl.h>

class MaCORS {
public:
    MaCORS(const std::string& user, const std::string& password, const std::string& area);
    MaCORS(const std::string& user, const std::string& password);
    bool sendRequest();

private:
    std::string user_;
    std::string password_;
    std::string area_;
};

#endif // MACORS_H
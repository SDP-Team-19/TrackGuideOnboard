#ifndef API_H
#define API_H

#include <string>
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <iostream> // Add this line
#include <nlohmann/json.hpp> // Include the JSON library (https://github.com/nlohmann/json)

class ApiClient {
public:
    // Constructor
    ApiClient();

    // Destructor
    ~ApiClient();

    // Create a JSON request with latitude and longitude
    nlohmann::json create_request(double latitude, double longitude, double threshold, const std::string& mode);

    // Send a JSON request to the server and return the response
    std::string send_post_request(const nlohmann::json& requestBody);

private:

    CURL* curlHandle;

    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp);

};

#endif // API_H
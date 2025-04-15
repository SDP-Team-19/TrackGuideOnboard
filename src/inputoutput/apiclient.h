#ifndef API_H
#define API_H

#include <string>
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <nlohmann/json.hpp> // Include the JSON library (https://github.com/nlohmann/json)

class ApiClient {
public:
    // Constructor
    ApiClient();

    // Destructor
    ~ApiClient();

    // Create a JSON request with latitude and longitude
    nlohmann::json createLocationRequest(float latitude, float longitude);

    // Create a JSON request with threshold and mode
    nlohmann::json createModeRequest(float threshold, const std::string& mode);

    // Send a JSON request to the server and return the response
    std::string sendPostRequest(const nlohmann::json& requestBody);

private:

    CURL* curlHandle;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

    void parser(std::string input);

};

#endif // API_H
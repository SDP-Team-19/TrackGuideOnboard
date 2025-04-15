#ifndef API_H
#define API_H

#include <string>
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
    nlohmann::json sendRequest(const nlohmann::json& requestBody);

private:
    // HTTP client instance created in the constructor
    std::unique_ptr<HttpClient> httpClient;
    // Add any private members or helper functions if needed
};

#endif // API_H
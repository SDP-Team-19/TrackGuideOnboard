#include "apiclient.h"

// Constructor
ApiClient::ApiClient() : httpClient(std::make_shared<HttpClient>()) {
    if (!httpClient->connect("frontend-computer", 80)) {
        throw std::runtime_error("Failed to connect to HTTP client");
    }
}

// Destructor
ApiClient::~ApiClient() {
    if (httpClient) {
        httpClient->disconnect(); // Ensure the client is properly closed
    }
}

// Create a JSON request with latitude and longitude
nlohmann::json ApiClient::createLocationRequest(float latitude, float longitude) {
    nlohmann::json request;
    request["type"] = "location";
    request["data"]["latitude"] = latitude;
    request["data"]["longitude"] = longitude;
    return request;
}

// Create a JSON request with threshold and mode
nlohmann::json ApiClient::createModeRequest(float threshold, const std::string& mode) {
    nlohmann::json request;
    request["type"] = "mode";
    request["data"]["threshold"] = threshold;
    request["data"]["mode"] = mode;
    return request;
}
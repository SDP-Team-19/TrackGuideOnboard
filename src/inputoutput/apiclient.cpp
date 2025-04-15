#include "apiclient.h"
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

// Constructor
ApiClient::ApiClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curlHandle = curl_easy_init();
    if (!curlHandle) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

// Destructor
ApiClient::~ApiClient() {
    if (curlHandle) {
        curl_easy_cleanup(curlHandle);
    }
    curl_global_cleanup();
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

// Send a POST request with JSON data
std::string ApiClient::sendPostRequest(const std::string& url, const nlohmann::json& jsonData) {
    if (!curlHandle) {
        throw std::runtime_error("CURL handle is not initialized");
    }

    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        if (userp) {
            static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
            return size * nmemb;
        }
        return 0;
    });
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, std::string* userp) -> size_t {
        userp->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curlHandle);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }

    return response;
}
#include "apiclient.h"

// Constructor
ApiClient::ApiClient() {
    // Initialize CURL
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
nlohmann::json ApiClient::create_request(double latitude, double longitude, double threshold, const std::string& mode) {
    nlohmann::json request;
    request["mode"] = mode;
    request["threshold"] = threshold;
    request["latitude"] = latitude;
    request["longitude"] = longitude;
    return request;
}

std::future<void> ApiClient::send_post_request(const nlohmann::json& jsonData) {
    return std::async(std::launch::async, [this, jsonData]() {
        std::cout << "sending the message: " << jsonData.dump() << std::endl;

        CURL* localHandle = curl_easy_init();
        if (!localHandle) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        curl_easy_setopt(localHandle, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(localHandle, CURLOPT_URL, "http://frontend-computer:8081");
        curl_easy_setopt(localHandle, CURLOPT_TIMEOUT, 1L); // 1 second timeout
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(localHandle, CURLOPT_HTTPHEADER, headers);
        
        std::string jsonString = jsonData.dump();
        curl_easy_setopt(localHandle, CURLOPT_POSTFIELDS, jsonString.c_str());

        curl_easy_perform(localHandle);
        curl_slist_free_all(headers);
        curl_easy_cleanup(localHandle);
    });
}


size_t ApiClient::write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}
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
nlohmann::json ApiClient::create_location_request(float latitude, float longitude) {
    nlohmann::json request;
    request["latitude"] = latitude;
    request["longitude"] = longitude;
    return request;
}

// Create a JSON request with threshold and mode
nlohmann::json ApiClient::create_mode_request(float threshold, const std::string& mode) {
    nlohmann::json request;
    request["threshold"] = threshold;
    request["mode"] = mode;
    return request;
}

std::string ApiClient::send_post_request(const nlohmann::json& jsonData) {
    std::string responseString;

    curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curlHandle, CURLOPT_URL, "http://frontend-computer:8081");
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseString);
    
    std::string jsonString = jsonData.dump();
    curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, jsonString.c_str());


    CURLcode response = curl_easy_perform(curlHandle);
    if (response != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(response) << std::endl;
    }

    curl_slist_free_all(headers); // Free the headers list

    return responseString;
}


size_t ApiClient::write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}
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

std::string ApiClient::sendPostRequest(const nlohmann::json& jsonData) {
    std::string responseString;

    curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curlHandle, CURLOPT_URL, "https://frontend-computer:8081");
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseString);
    
    std::string jsonString = jsonData.dump();
    curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, jsonString.c_str());

    // Handle SSL with proper verification
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curlHandle, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");

    CURLcode response = curl_easy_perform(curlHandle);
    if (response != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(response) << std::endl;
    }

    parser(responseString);

    curl_slist_free_all(headers); // Free the headers list

    return responseString;
}


size_t ApiClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

void ApiClient::parser(std::string input)   {
    std::cout << "Parsing JSON response: " << input << std::endl;
    auto json = nlohmann::json::parse(input);
    std::string type = json["type"];
    if (type == "location") {
        float latitude = json["data"]["latitude"];
        float longitude = json["data"]["longitude"];
        std::cout << "Latitude: " << latitude << ", Longitude: " << longitude << std::endl;
    } else if (type == "mode") {
        float threshold = json["data"]["threshold"];
        std::string mode = json["data"]["mode"];
        std::cout << "Threshold: " << threshold << ", Mode: " << mode << std::endl;
    } else {
        std::cerr << "Unknown type: " << type << std::endl;
    }
}
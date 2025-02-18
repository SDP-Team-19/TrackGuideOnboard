#include "MaCORS.h"

MaCORS::MaCORS(const std::string& user, const std::string& password, const std::string& area)
    : user_(user), password_(password), area_(area) {}

MaCORS::MaCORS(const std::string& user, const std::string& password)
    : user_(user), password_(password), area_("") {}

bool MaCORS::sendRequest() {
    CURL* curl;
    CURLcode res;
    std::string url = "http://macorsrtk.massdot.state.ma.us:10000/" + area_;
    std::string userpwd = user_ + ":" + password_;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        // This set of curl commands work on Mac with curl 8.9.1
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTP09_ALLOWED, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
        curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return true;
}

#ifndef STATES_H
#define STATES_H
#include "ledcontrol.h"
#include "boundarylogic.h"
#include "apiclient.h"
// #include "kinesisStream.h

class States {
public:
    States(LEDControl& ledController, BoundaryLogic& boundaryLogic, ApiClient& apiClient);
    
    void run_record_function(const char* content);
    void run_play_function(const char* content);
    void run_reset_function();
    void run_standby_function();
private:
    LEDControl ledController_;
    BoundaryLogic& boundaryLogic_;
    bool track_loaded_;  // Flag to indicate whether the track has been loaded
    bool is_recording_;
    float previous_time_;
    double previous_latitude_;
    double previous_longitude_; 
    ApiClient& apiClient_;
    std::mutex file_mutex;
};

#endif // STATES_H
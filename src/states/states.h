#ifndef STATES_H
#define STATES_H
#include "ledcontrol.h"
#include "boundarylogic.h"
// #include "kinesisStream.h

class States {
public:
    States(LEDControl ledController, BoundaryLogic& boundaryLogic);
    
    void run_record_function(const char* content);
    void run_play_function(const char* content);
    void run_reset_function();
private:
    LEDControl ledController_;
    BoundaryLogic& boundaryLogic_;
    bool track_loaded_;  // Flag to indicate whether the track has been loaded
    bool is_recording_;
    std::mutex file_mutex;
};

#endif // STATES_H
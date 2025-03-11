#ifndef STATES_H
#define STATES_H
#include "ledcontrol.h"
#include "boundarylogic.h"
#include "kinesis.h"

class States {
public:
    States(LEDControl ledController, BoundaryLogic& boundaryLogic, KinesisStream& kinesisStream);
    
    void run_record_function(const char* content);
    void run_play_function(const char* content);
    void run_reset_function();
private:
    LEDControl ledController_;
    BoundaryLogic& boundaryLogic_;
    KinesisStream& kinesisStream_;
    bool track_loaded_;  // Flag to indicate whether the track has been loaded
    std::mutex file_mutex;
};

#endif // STATES_H
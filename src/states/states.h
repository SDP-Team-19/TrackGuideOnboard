#ifndef STATES_H
#define STATES_H

class TCPServer {
public:
    void run_record_function(const char* content);
    void run_play_function(const char* content);
    void run_record_function();
};

#endif // STATES_H
// #include <csignal>
// #include "tcpserver.h"
// #include "rtkservice.h"
// #include "ledcontrol.h"
// #include "buttons.h"
// #include "states.h"
// #include "kinesis.h"
// #include <pigpio.h>
// #include <iostream>
// #include <thread>
// #include <sys/mman.h>   // For shm_open, mmap, etc.
// #include <fcntl.h>      // For O_* constants
// #include <unistd.h>     // For ftruncate
// #include <sys/stat.h>   // For mode constants
// #include <semaphore.h>  // For semaphores

// #define PORT 12345
// #define SHM_NAME "/tcpserver_shm"
// #define SEM_NAME "/tcpserver_sem"
// #define SHM_SIZE sizeof(SharedMemory)

// RTKService* rtk_service_ptr = nullptr;
// LEDControl* led_control_ptr = nullptr;

// std::atomic<SystemState> system_state(SystemState::STANDBY);
// std::atomic<bool> shutdown_requested(false);  // Atomic flag

// void signal_handler(int signal) {
//     if (signal == SIGINT || signal == SIGTERM) {
//         shutdown_requested.store(true, std::memory_order_release);
//         std::cout << "Interrupt received. Shutting down..." << std::endl;
//     } else if (signal == SIGCHLD) {
//         std::cout << "Reaping child" << std::endl;
//         // Prevent zombie processes
//         while (waitpid(-1, NULL, WNOHANG) > 0);
//     }
// }

// int main() {
//     std::signal(SIGINT, signal_handler);
//     std::signal(SIGTERM, signal_handler);
//     std::signal(SIGCHLD, signal_handler);

//     Aws::SDKOptions options;
//     Aws::InitAPI(options);  //init AWS SDK

//     KinesisClient kinesisClient;
//     std::cout << "Stream setup" << std::endl;

//     gpioCfgSetInternals(1 << 10);

//     // Create shared memory
//     int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
//     if (shm_fd == -1) {
//         std::cerr << "Failed to create shared memory: " << strerror(errno) << std::endl;
//         return EXIT_FAILURE;
//     }
//     if (ftruncate(shm_fd, SHM_SIZE) == -1) {
//         std::cerr << "Failed to set size of shared memory: " << strerror(errno) << std::endl;
//         return EXIT_FAILURE;
//     }
//     SharedMemory* shared_memory = static_cast<SharedMemory*>(mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
//     if (shared_memory == MAP_FAILED) {
//         std::cerr << "Failed to map shared memory: " << strerror(errno) << std::endl;
//         return EXIT_FAILURE;
//     }

//     // Create semaphore
//     sem_t* semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
//     if (semaphore == SEM_FAILED) {
//         std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
//         return EXIT_FAILURE;
//     }

//     LEDControl led_control(19, 28);
//     led_control_ptr = &led_control;
//     led_control.indicate_all(Color::GREEN);
//     usleep(3000000);

//     Buttons buttons(16, 20, 21, shared_memory, semaphore);
//     std::thread button_thread(&Buttons::monitor_button, &buttons, std::ref(shutdown_requested));

//     BoundaryLogic boundary_logic;
//     KinesisStream kinesisStream("");
//     States states(led_control, boundary_logic, kinesisStream);

//     RTKService rtk_service("/home/team19/RTK_CONFIG/rtkrcv.conf");
//     rtk_service_ptr = &rtk_service;
//     rtk_service.start_server();
    
//     TCPServer server(PORT, led_control, states, shared_memory, semaphore);
//     server.start(shutdown_requested);

//     std::cout << "Shutting down safely..." << std::endl;

//     if (rtk_service_ptr) {
//         std::cout << "Shutting down rtk service" << std::endl;
//         rtk_service_ptr->shutdown_server();
//         rtk_service_ptr = nullptr;
//     }
//     if (led_control_ptr) {
//         std::cout << "Shutting down led control" << std::endl;
//         led_control_ptr->clear();
//         led_control_ptr = nullptr;
//     }
//     if (button_thread.joinable()) {
//         button_thread.join();
//     }
//     gpioTerminate();

//     // Cleanup shared memory and semaphore
//     munmap(shared_memory, SHM_SIZE);
//     close(shm_fd);
//     shm_unlink(SHM_NAME);
//     sem_close(semaphore);
//     sem_unlink(SEM_NAME);

//     return EXIT_SUCCESS;
// }


#include <aws/core/Aws.h>
#include <aws/kinesis/KinesisClient.h>
#include <aws/kinesis/model/PutRecordRequest.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace Aws;
using namespace Aws::Kinesis;
using namespace Aws::Kinesis::Model;
using namespace Aws::Utils::Json;
using namespace std;

//in /build
//cmake .. -DCMAKE_PREFIX_PATH=~/Desktop/TrackGuide-App-trackguide-map/aws-sdk-install
int first = 0;
//generate fake coords
JsonValue generate_coordinates(double latitude, double longitude) {
    cout << "Generating coords" << endl;
    if(first == 0){
        double threshold = 0.00001;
        Aws:String jsonString = R"({"mode":"replay"})";
        JsonValue threshjson(jsonString);
        threshjson.WithDouble("threshold", threshold);
        first = first+1;
        return threshjson;
    }
    first += 1;

    if (first < 20){
        latitude += 0.000005;
    }
    else if(first < 40) {
        longitude -= 0.000005;
    }
    else if(first < 60){
        latitude -= 0.000005;
    }
    else{
        longitude +=0.000005;
    }

    JsonValue json;
    json.WithDouble("latitude", latitude);
    json.WithDouble("longitude", longitude);
    
    return json;
}

//message one (on event[button change])
// thresh, mode(record, play, bound reset, line reset)

//continually send message 2
//lat, long


int main() {
    JsonValue jsonPayload;

    //Aws:String jsonString = R"({"longitude":12.2554, "latitude":32.421})";
    //Aws:String jsonString = R"({"threshold": 12.3, "mode":"replay"})";

    //JsonValue json(jsonString);
    Aws::SDKOptions options;
    Aws::InitAPI(options);  //init AWS SDK
    cout << "Creating client" << endl;
    KinesisClient kinesisClient;
    cout << "Client Created" << endl;

    string streamName = "CoordinatesStream";
    double latitude = 42.393489;
    double longitude = -72.529097;

    while (true) {
        //convert coords to JSON
         //gen new coordinates
         cout << "Looping" << endl;
        JsonValue jsonResult = generate_coordinates(latitude, longitude);
        Aws::String jsonString = jsonResult.View().WriteReadable();


        if (jsonResult.WasParseSuccessful()){
            JsonView jsonView = jsonResult.View();

            if (jsonView.ValueExists("threshold")){
                jsonPayload = JsonValue(); //clear payload
                jsonPayload.WithString("threshold", to_string(jsonView.GetDouble("threshold")));
                jsonPayload.WithString("mode", jsonView.GetString("mode"));
            }
            else{
                jsonPayload = JsonValue(); //clear payload
                jsonPayload.WithString("latitude", to_string(jsonView.GetDouble("latitude")));
                jsonPayload.WithString("longitude", to_string(jsonView.GetDouble("longitude")));

                latitude = jsonView.GetDouble("latitude");
                longitude = jsonView.GetDouble("longitude");
            }

            Aws::String jsonStr = jsonPayload.View().WriteCompact();

            //prepare kinesis
            PutRecordRequest request;
            request.SetStreamName(streamName);
            request.SetData(Aws::Utils::ByteBuffer((unsigned char*)jsonStr.c_str(), jsonStr.length()));
            request.SetPartitionKey("partition-1");

            //send data to kinesis
            auto outcome = kinesisClient.PutRecord(request);


            if (outcome.IsSuccess()) {
                cout << "Sent: " << jsonStr << endl;
            }else{
                cout << "Error: " << outcome.GetError().GetMessage() << endl;
            }
        }
        this_thread::sleep_for(chrono::seconds(1));  // Wait 2 seconds
    }
    

    return 0;
}
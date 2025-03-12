#include "kinesis.h"

KinesisStream::KinesisStream(const std::string& streamName) : streamName(streamName) {
    std::cout << "kinesis stream created" << std::endl;
}

KinesisStream::~KinesisStream() {
    Aws::ShutdownAPI(options);
}
bool KinesisStream::sendModeData(const std::string& mode, const double threshold) {
    JsonValue payload = serializeModeData(mode, threshold);
    std::string payloadStr = payload.View().WriteCompact();

    PutRecordRequest request;
    request.SetStreamName(streamName);
    request.SetPartitionKey(Aws::Utils::UUID::RandomUUID());
    request.SetData(Aws::Utils::ByteBuffer((unsigned char*)payloadStr.c_str(), payloadStr.length()));

    auto outcome = kinesisClient.PutRecord(request);
    if (!outcome.IsSuccess()) {
        std::cerr << "Failed to send mode data to Kinesis: " << outcome.GetError().GetMessage() << std::endl;
        return false;
    }
    return true;
}

bool KinesisStream::sendPositionData(const double latitude, const double longitude) {
    JsonValue payload = serializePositionData(latitude, longitude);
    std::string payloadStr = payload.View().WriteCompact();

    PutRecordRequest request;
    request.SetStreamName(streamName);
    request.SetPartitionKey(Aws::Utils::UUID::RandomUUID());
    request.SetData(Aws::Utils::ByteBuffer((unsigned char*)payloadStr.c_str(), payloadStr.length()));

    auto outcome = kinesisClient.PutRecord(request);
    if (!outcome.IsSuccess()) {
        std::cerr << "Failed to send position data to Kinesis: " << outcome.GetError().GetMessage() << std::endl;
        return false;
    }
    return true;
}

JsonValue KinesisStream::serializeModeData(const std::string& mode, const double threshold) {
    JsonValue jsonPayload;
    jsonPayload.WithString("mode", mode);
    jsonPayload.WithDouble("threshold", threshold);

    return jsonPayload;
}

JsonValue KinesisStream::serializePositionData(const double latitude, const double longitude){
    JsonValue jsonPayload;
    jsonPayload.WithDouble("latitude", latitude);
    jsonPayload.WithDouble("longitude", longitude);

    return jsonPayload;
}
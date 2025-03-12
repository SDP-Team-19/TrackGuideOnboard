#ifndef KINESIS_H
#define KINESIS_H

#include <aws/kinesis/KinesisClient.h>
#include <aws/kinesis/model/PutRecordRequest.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/UUID.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <memory> // For std::make_unique
#include <string>
#include <sstream>
#include <iostream>

using namespace Aws::Utils::Json;
using namespace Aws::Kinesis;
using namespace Aws::Kinesis::Model;

class KinesisStream {
public:
    KinesisStream(const std::string& streamName);
    ~KinesisStream();

    bool sendModeData(const std::string& mode, const double threshold);
    bool sendPositionData(const double latitude, const double longitude);

private:
    JsonValue serializeModeData(const std::string& mode, const double threshold);
    JsonValue serializePositionData(const double latitude, const double longitude);
    Aws::SDKOptions options;
    std::unique_ptr<Aws::Kinesis::KinesisClient> kinesisClient;
    std::string streamName;
};;

#endif // KINESIS_H
#include "ITTI/types.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using cu_ue_id_t = uint32_t;
using sdu_size_t = size_t;

struct custom_pdu_t
{
    uint8_t *payload;
    sdu_size_t size;

    void serialize(rapidjson::Value &obj, rapidjson::Document::AllocatorType &allocator) const
    {
        obj.AddMember("size", static_cast<int>(size), allocator);

        rapidjson::Value payloadArray(rapidjson::kArrayType);
        for (size_t i = 0; i < size; ++i)
        {
            payloadArray.PushBack(payload[i], allocator);
        }

        obj.AddMember("payload", payloadArray, allocator);
    }

    void deserialize(const rapidjson::Value &obj)
    {
        if (obj.HasMember("size") && obj["size"].IsUint())
        {
            size = obj["size"].GetUint();
        }

        if (obj.HasMember("payload") && obj["payload"].IsArray())
        {
            const rapidjson::Value &payloadArray = obj["payload"];
            payload = new uint8_t[size];
            for (rapidjson::SizeType i = 0; i < payloadArray.Size(); ++i)
            {
                payload[i] = payloadArray[i].GetUint();
            }
        }
    }
};

struct UeCapabilityInfoIndication
{
    cu_ue_id_t         cp_ue_id;
    custom_pdu_t       ue_radio_cap;

    void serialize(rapidjson::Document &config) const
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("cp_ue_id", cp_ue_id, config.GetAllocator());

        rapidjson::Value radioCapObj(rapidjson::kObjectType);
        ue_radio_cap.serialize(radioCapObj, config.GetAllocator());

        obj.AddMember("ue_radio_cap", radioCapObj, config.GetAllocator());

        config.AddMember("UeCapabilityInfoIndication", obj, config.GetAllocator());
    }

    void deserialize(const rapidjson::Value &config)
    {
        if (config.HasMember("cp_ue_id") && config["cp_ue_id"].IsUint())
        {
            cp_ue_id = config["cp_ue_id"].GetUint();
        }

        if (config.HasMember("ue_radio_cap") && config["ue_radio_cap"].IsObject())
        {
            ue_radio_cap.deserialize(config["ue_radio_cap"]);
        }
    }
};

int main() {
    UeCapabilityInfoIndication uci{};
    uci.cp_ue_id = 123;
    uci.ue_radio_cap.size = 3;
    uci.ue_radio_cap.payload = new uint8_t[uci.ue_radio_cap.size]{1, 2, 3};

    rapidjson::Document doc;
    uci.serialize(doc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    UeCapabilityInfoIndication newUci{};
    newUci.deserialize(doc);

    return 0;
}
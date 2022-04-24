#include "packet.h"
#include "common.h"

static Packet make_packet(Opts opts, uint64_t to_who, uint8_t response, const void* data)
{
    Packet packet = {
        .opts = opts,
        .to_who = to_who, 
        .id = random_8_id(),
        .response = response,
        .data = {0}
    };
    if (data != nullptr)
        memcpy(packet.data, data, DATA_SIZE);

    return packet;
}

Packet create_packet(Opts opts, uint64_t to_who, uint8_t response, const void* data)
{
    return make_packet(opts, to_who, response, data);
}

Packet create_packet(Opts opts, const void* data)
{
    return make_packet(opts, 0, 0, data);
}
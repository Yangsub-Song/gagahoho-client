#ifndef RFPACKET_H
#define RFPACKET_H

#include <string.h>
#include <stdint.h>

#define RF_PACKET_PADS 0xEE
#define RF_PACKET_SOP 0xFF

#define RF_PACKET_DATA_SIZE 44

// | Padding * 2 | SOP | type | GRP | Length | Data
class RFPacket {

public:
    RFPacket() : type(0), grp(0), len(0) {
        memset(data, 0, RF_PACKET_DATA_SIZE);
    }

    uint8_t type;
    uint8_t grp;
    uint8_t len;
    uint8_t data[RF_PACKET_DATA_SIZE];
};

#endif // RFPACKET_H

#include <stdint.h>

enum entry_type {
  EMPTY = 255
}

struct logentry {
    enum entry_type tag; //1
    uint32_t ts; //4
    union { //4
        struct sale { //4
            uint8_t user;
            uint8_t itemid;
            int16_t amount;
        };

        struct credit { //3
            uint8_t user;
            int16_t amount_cent;
        };
    }
    uint16_t crc; //2
}

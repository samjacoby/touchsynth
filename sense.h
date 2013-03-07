#ifndef __sense_h_
#define __sense_h_

#include <CapSense.h>

typedef struct {
    CapSense sensor[];
    uint16_t val;
    uint16_t last_val;
}


#endif //__sense_h_

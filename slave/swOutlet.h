/* Andy Sayler
 * SmartWall Project
 * SmartWall Slave Device Interface - Outlet
 * swOutlet.h
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
 * ---
 */

#ifndef SWOUTLET_H
#define SWOUTLET_H

/* Local Includes */
#include "../com/SmartWall.h"
#include "../com/SmartWallSockets.h"
#include "swSlave.h"

/* Types */
typedef long outletChanState_t;
typedef double outletChanPower_t;

/* Unions */
union outletChanArg{
    outletChanState_t chanState;
    outletChanPower_t chanPower;
};

/* Structs */
struct outletDeviceState{
    struct SWDeviceInfo* myDev;
    outletChanState_t* chState;
    outletChanPower_t* chPower;
};

/* Functions */
#define NUMOUTLETPROCESSORS 1
enum processorState outletChnDevHandeler(const swOpcode_t chnOpcode,
                                         const msgType_t msgType,
                                         const struct SWChannelData* input,
                                         struct SWChannelData* output,
                                         struct outletDeviceState* deviceState,
                                         msgScope_t* errorScope,
                                         swOpcode_t* errorOpcode);

enum processorState outletChnMasterHandeler(const swOpcode_t chnOpcode,
                                            const msgType_t msgType,
                                            const struct SWChannelData* input);

/* Defines */
#define OUTLET_CHAN_OFF 0
#define OUTLET_CHAN_ON 1

/* Opcodes */
#define OUTLET_CH_OP_STATE   0x0010
#define OUTLET_CH_OP_VOLTAGE 0x0020
#define OUTLET_CH_OP_CURRENT 0x0021
#define OUTLET_CH_OP_POWER   0x0022
#define OUTLET_CH_OP_FREQ    0x0023
#define OUTLET_CH_OP_PHASE   0x0024

#endif

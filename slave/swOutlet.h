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

/* Outside Includes */
#include "swSlave.h"

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

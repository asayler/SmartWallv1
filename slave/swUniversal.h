/* Andy Sayler
 * SmartWall Project
 * SmartWall Slave Device Interface - Outlet
 * swUniversal.h
 * Created 3/12/11
 *
 * Change Log:
 * 03/12/11 - Created
 * ---
 */

#ifndef SWUNIVERSAL_H
#define SWUNIVERSAL_H

/* Outside Includes */
#include "swSlave.h"

/* Defines */

/* Device Opcodes */
#define UNIVERSAL_DEV_OP_ERROR_TYPEMISMATCH   0x8000
#define UNIVERSAL_DEV_OP_ERROR_BADOPCODE      0x8001
#define UNIVERSAL_DEV_OP_ERROR_BADMSGTYPE     0x8002

/* Channel Opcodes */
#define UNIVERSAL_CHN_OP_ERROR_BADCHN         0x8000
#define UNIVERSAL_CHN_OP_ERROR_BADOPCODE      0x8001
#define UNIVERSAL_CHN_OP_ERROR_BADMSGTYPE     0x8002

#endif

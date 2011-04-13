 /* Andy Sayler
 * SmartWall Project
 * SmartWall Slave Outlet Sim
 * slaveOutlet.c
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
 * ---
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <stdint.h>

#include "../com/SmartWall.h"
#include "../com/SmartWallSockets.h"
#include "../com/comTools.h"
#include "swOutlet.h"
#include "swUniversal.h"

/* For Random Seeding */
#include <time.h>
#define CH0POWER 13
#define CH1POWER 60

#define SENDPORT SWINPORT
#define LISTENPORT SWOUTPORT

#define MYSWUID 0x0001000000000011ull
#define MYSWGROUP 0x01u
#define MYSWADDRESS 0x0011u
#define MYSWTYPE SW_TYPE_OUTLET | SW_TYPE_UNIVERSAL
#define MYSWCHAN 0x02u

#define CHAN0_INITSTATE OUTLET_CHAN_ON;
#define CHAN1_INITSTATE OUTLET_CHAN_ON;

static int updateState(struct outletDeviceState* state);

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;

    /* Seed RNG */
    srand(time(NULL));

    /* Local Temp Vars */
    unsigned int i = 0;

    /* Setup Temporary Processor Storage Vars */
    /* Chan Storage */
    struct SWChannelEntry tmpChnEntries[MYSWCHAN];
    memset(&tmpChnEntries, 0, sizeof(tmpChnEntries));
    struct SWChannelData tmpChnData;
    memset(&tmpChnData, 0, sizeof(tmpChnData));
    union outletChanArg tmpChanArgs[MYSWCHAN];
    memset(&tmpChanArgs, 0, sizeof(tmpChanArgs));
    tmpChnData.data = tmpChnEntries;
    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tmpChnData.data[i].chanValue = &(tmpChanArgs[i]);
    }
    struct SWChannelLimits limits;
    limits.maxNumChan = MYSWCHAN;
    limits.maxDataLength = sizeof(*tmpChanArgs);
    
    /* Setup Processors */
    struct SWDevProcessor processors[NUMOUTLETPROCESSORS];
    memset(processors, 0, sizeof(processors));
    processors[0].processorScope = SW_SCP_CHANNEL;
    processors[0].data = &tmpChnData;
    processors[0].dataLimits = &limits;
    processors[0].decoder = (readSWBody)readSWChannelBody;
    processors[0].handeler = (swDevHandeler)outletChnDevHandeler;
    processors[0].encoder = (writeSWBody)writeSWChannelBody;
    
    /* Setup State Vars */
    struct outletDeviceState myState;
    memset(&myState, 0, sizeof(myState));
    outletChanState_t chState[MYSWCHAN];
    memset(&chState, 0, sizeof(chState));
    outletChanPower_t chPower[MYSWCHAN];
    memset(&chPower, 0, sizeof(chPower));
    struct SWDeviceInfo myDevice;
    memset(&myDevice, 0, sizeof(myDevice));
    myState.myDev = &myDevice;
    myState.chState = chState;
    myState.chPower = chPower;
    enum SWReceiverState machineState = RST_SETUP;
    
    /* Setup SW Vars */
    struct SWDeviceInfo tgtDevice;
    myState.myDev->devInfo.swAddr = MYSWADDRESS;
    myState.myDev->devInfo.devTypes = MYSWTYPE;
    myState.myDev->devInfo.numChan = MYSWCHAN;
    myState.myDev->devInfo.version = SW_VERSION;
    myState.myDev->devInfo.uid = MYSWUID;
    myState.myDev->devInfo.groupID = MYSWGROUP;    
    
    /* My IP */
    myState.myDev->devIP.sin_family = AF_INET;
    myState.myDev->devIP.sin_port = htons(LISTENPORT);
    myState.myDev->devIP.sin_addr.s_addr = htonl(INADDR_ANY);
    tgtDevice.devIP.sin_family = AF_INET;
    tgtDevice.devIP.sin_port = htons(SENDPORT);
    tgtDevice.devIP.sin_addr.s_addr = htonl(INADDR_ANY);
    
    while(1){
#ifdef SWDEBUG
        fprintf(stderr, "slaveOutlet: start machineState: %d\n",
                machineState);
#endif
        machineState = swReceiverStateMachine(machineState,
                                              myState.myDev, &tgtDevice,
                                              &myState,
                                              processors, NUMOUTLETPROCESSORS);
        if(updateState(&myState)){
            fprintf(stderr, "Error updating state\n");
        }
#ifdef SWDEBUG
        fprintf(stderr, "slaveOutlet: end machineState: %d\n", machineState);
#endif

    }

    return 0;
}

static int updateState(struct outletDeviceState* state){
    
    /* Update Power */
    rand();
    if(state->chState[0]){
        if(rand() % 2){
            state->chPower[0] = CH0POWER + ((rand() % 1024) / 1024.0);
        }
        else{
            state->chPower[0] = CH0POWER - ((rand() % 1024) / 1024.0);
        }
    }
    else{
        state->chPower[0] = 0;
    }
    
    if(state->chState[1]){
        if(rand() % 2){
            state->chPower[1] = CH1POWER + ((rand() % 1024) / 1024.0);
        }
        else{
            state->chPower[1] = CH1POWER - ((rand() % 1024) / 1024.0);
        }
    }
    else{
        state->chPower[1] = 0;
    }
        
    return 0;
}

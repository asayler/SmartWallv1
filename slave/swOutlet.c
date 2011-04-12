/* Andy Sayler
 * SmartWall Project
 * SmartWall Slave Device Implmentation - Outlet
 * swOutlet.c
 * Created 3/12/11
 *
 * Change Log:
 * 03/12/11 - Created
 * ---
 */

#include "swOutlet.h"

enum processorState outletChnHandeler(const swOpcode_t chnOpcode,
                                      const msgType_t msgType,
                                      const struct SWChannelData* input,
                                      struct SWChannelData* output,
                                      struct outletDeviceState* deviceState,
                                      msgScope_t* errorScope,
                                      swOpcode_t* errorOpcode){

    /* Local vars */
    unsigned int i;
    numChan_t chn;
    union outletChanArg* tempArg;
    *errorScope = 0;
    *errorOpcode = 0;

    /* Switch on Opcode */
    switch(chnOpcode){
    case OUTLET_CH_OP_STATE:
        {
            /* Switch on Message Type: SET OR QUERY */
            switch(msgType){
            case SW_MSG_SET:
                /* Set State */
                for(i = 0; i < input->header.numChan; i++){
                    chn = input->data[i].chanTop.chanNum;
                    if(chn < deviceState->myDev->devInfo.numChan){
                        /* TODO: Check valid state */
                        tempArg = input->data[i].chanValue;
                        deviceState->chState[chn] = tempArg->chanState;
#ifdef SWDEBUG
                        fprintf(stdout, "Ch %u Set to %lu\n",
                                chn, deviceState->chState[chn]);
#endif
                    }
                    else{
                        fprintf(stderr,
                                "%s: Invalid chanNum\n", "outletChnHandeler");
                        *errorScope = SW_SCP_CHANNEL;
                        *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADCHN;
                        return PROCESSOR_SUCCESS; 
                    }
                }
            case SW_MSG_QUERY:
                /* Report State */
                output->header = input->header;
                for(i = 0; i < input->header.numChan; i++){
                    chn = input->data[i].chanTop.chanNum;
                    if(chn < deviceState->myDev->devInfo.numChan){
                        output->data[i].chanTop.chanNum = chn;
                        tempArg = output->data[i].chanValue;
                        tempArg->chanState = deviceState->chState[chn];
                    }
                    else{
                        fprintf(stderr,
                                "%s: Invalid chanNum. chn=%u, num=%u\n",
                                "outletChnHandeler", chn,
                                deviceState->myDev->devInfo.numChan);
                        *errorScope = SW_SCP_CHANNEL;
                        *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADCHN;
                        return PROCESSOR_SUCCESS; 
                    }
                }
                return PROCESSOR_SUCCESS;
                break;
            default:
                fprintf(stderr, "%s: Unhandeled msgType\n",
                        "outletChnHandeler");
                *errorScope = SW_SCP_CHANNEL;
                *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADMSGTYPE;
                return PROCESSOR_SUCCESS;
            }
        }
    default:
        {
            fprintf(stderr, "%s: Unhandeled Opcode\n",
                    "outletChnHandeler");
            *errorScope = SW_SCP_CHANNEL;
            *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADOPCODE;
            return PROCESSOR_SUCCESS;
        }
    }
    return PROCESSOR_ERROR;
}

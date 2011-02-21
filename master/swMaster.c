/* Andy Sayler
 * SmartWall Project
 * SmartWall Master Device Functions
 * swMaster.h
 * Created 2/15/11
 *
 * Change Log:
 * 02/15/11 - Created
  * ---
 */

#include "swMaster.h"

/* Public Functions */
extern int buildDevFileName(char* filename){
    (void) filename;

    return -1;
}

extern int readDevice(struct SWDeviceEntry* device, FILE* devFile){
    /* Function Vars */
    const int errorVal = -1;
    int count = 0;

    /* Temp Vars */
    swAddress_t swAddrTmp = 0;
    in_addr_t ipAddrTmp = 0;
    devType_t devTypeTmp = 0;
    numChan_t numChanTmp = 0;
    swVersion_t versionTmp = 0;
    devUID_t uidTmp = 0;
    int numScn = 0;

    /* Read From File */
    /* Get SW Address */
    if((numScn = fscanf(devFile, "%" SCNxSWAddr, &swAddrTmp)) != 1){
        fprintf(stderr, "Bad data near first pos\n"
                "or extra line return at end of file.\n");
        return errorVal;
    }
    else {
        count += numScn;
    }
    /* Get IP Address */
    if((numScn = fscanf(devFile, "%" SCNxIPAddr, &ipAddrTmp)) != 1){
        fprintf(stderr, "Bad data near second pos.\n");
        return errorVal;
    }
    else {
        count += numScn;
    }
    /* Get Device Type */
    if((numScn = fscanf(devFile, "%" SCNxDevType, &devTypeTmp)) != 1){
        fprintf(stderr, "Bad data near third pos.\n");
        return errorVal;
    }
    else {
        count += numScn;
    }
    /* Get Number of Device Channels */
    if((numScn = fscanf(devFile, "%" SCNxNumChan, &numChanTmp)) != 1){
        fprintf(stderr, "Bad data near fourth pos.\n");
        return errorVal;
    }
    else {
        count += numScn;
    }
    /* Get SW Protocol version */
    if((numScn = fscanf(devFile, "%" SCNxSWVer, &versionTmp)) != 1){
        fprintf(stderr, "Bad data near fifth pos.\n");
        return errorVal;
    }
    else {
        count += numScn;
    }
    /* Get Device Unique ID */
    if((numScn = fscanf(devFile, "%" SCNxDevUID, &uidTmp)) != 1){
        fprintf(stderr, "Bad data near sixth pos.\n");
        return errorVal;
    }
    else {
        count += numScn;
    }
    
    /* Add device info to struct*/
    if(device != NULL){
        device->swAddr = swAddrTmp;
        device->ipAddr = ipAddrTmp;
        device->devTypes = devTypeTmp;
        device->numChan = numChanTmp;
        device->version = versionTmp;
        device->uid = uidTmp;
    }
    else {
        fprintf(stderr, "'devices' must not be NULL.\n");
        return errorVal;
    }
    
    return count;
}

extern int writeDevice(const struct SWDeviceEntry* device, FILE* devFile){
    /* Function Vars */
    const int errorVal = -1;
    int count = 0;

    /* Temp Vars */
    int numPri = 0;

    /* input check */
    if(device == NULL){
        fprintf(stderr, "'devices' must not be NULL.\n");
        return errorVal;
    }

    /* Write To File */
    /* Start New Line */
    numPri = fprintf(devFile, "\n");    
    if(numPri < 1){
        fprintf(stderr, "Could not write new line\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write SW Address */
    numPri = fprintf(devFile, "%4.4" PRIxSWAddr " ", device->swAddr);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near first pos\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write IP Address */
    numPri = fprintf(devFile, "%8.8" PRIxIPAddr " ", device->ipAddr);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near second pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write Device Type */
    numPri = fprintf(devFile, "%2.2" PRIxDevType " ", device->devTypes);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near third pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write Number of Device Channels */
    numPri = fprintf(devFile, "%2.2" PRIxNumChan " ", device->numChan);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near fourth pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write SW Protocol version */
    numPri = fprintf(devFile, "%1.1" PRIxSWVer " ", device->version);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near fifth pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write Device Unique ID */
    numPri = fprintf(devFile, "%16.16" PRIxDevUID, device->uid);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near sixth pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    
    return count;;
}

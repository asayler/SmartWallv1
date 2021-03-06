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

const int errorVal = -1;

/* Public Functions */
extern int buildDevFileName(char* filename, const int maxLength){
 
    /* Local Vars */
    int length = 0;
    
    /* Check Input */
    if(filename == NULL){
        fprintf(stderr, "buildDevFileName: 'filename' must not be NULL.\n");
        return errorVal;
    }

    /* Test Length */
    length = strlen(MASTER_DEVICE_FILE_BASE) +
        strlen(MASTER_DEVICE_FILE_EXTENSION) + 1;
    if(length > maxLength){
        fprintf(stderr, "buildDevFileName: Filename exceeds max length.\n");
        return errorVal;
    }

    /* Build Filename */
    strncpy(filename, MASTER_DEVICE_FILE_BASE, maxLength - 1);
    strncat(filename, MASTER_DEVICE_FILE_EXTENSION,
            maxLength - strlen(filename) - 1);

    return strlen(filename);
}

extern FILE* openDevFile(char* filename){
    
    /* Local Vars */
    FILE* devFile = NULL;
    
    /* Check Input */
    if(filename == NULL){
        fprintf(stderr, "openDevFile: 'filename' must not be NULL.\n");
        return NULL;
    }
    
    /* TODO: Add Semephore access control to common SW state file */
    
    /* Open File */
    devFile = fopen(filename, "r+");
    if(devFile == NULL){
        fprintf(stderr, "openDevFile: Could not open devFile\n");
        perror("openDevFile: fopen:");
        /* TODO: Return Semaphore on failure */
    }
 
    return devFile;

}

extern int closeDevFile(FILE* devFile){

    /* Check Input */
    if(devFile == NULL){
        fprintf(stderr, "closeDevFile: 'devFile' must not be NULL.\n");
        return errorVal;
    }

    /* TODO: Add Semephore access control to common SW state file */

    int val = fclose(devFile);

    if(val != 0){
        fprintf(stderr, "closeDevFile: Could not close devFile\n");
        return errorVal;
    }
    
    return val; 

}


extern int readDevice(struct SWDeviceEntry* device, FILE* devFile,
                      int lineNum){
    
    /* Local Vars */
    int count = 0;

    /* Temp Vars */
    swAddress_t swAddrTmp = 0;
    in_addr_t ipAddrTmp = 0;
    devType_t devTypeTmp = 0;
    numChan_t numChanTmp = 0;
    groupID_t grpIDTmp = 0;
    swVersion_t versionTmp = 0;
    devUID_t uidTmp = 0;
    int numScn = 0;

    /* input check */
    if(device == NULL){
        fprintf(stderr, "readDevice: 'devices' must not be NULL.\n");
        return errorVal;
    }
    if(devFile == NULL){
        fprintf(stderr, "readDevice: 'devFile' must not be NULL.\n");
        return errorVal;
    }    

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
    /* Get SW Group ID */
    if((numScn = fscanf(devFile, "%" SCNxGrpID, &grpIDTmp)) != 1){
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
        device->devInfo.swAddr = swAddrTmp;
        device->ipAddr = ipAddrTmp;
        device->devInfo.devTypes = devTypeTmp;
        device->devInfo.numChan = numChanTmp;
        device->devInfo.groupID = grpIDTmp;
        device->devInfo.version = versionTmp;
        device->devInfo.uid = uidTmp;
        device->lineNum = lineNum;
    }
    else {
        fprintf(stderr, "'devices' must not be NULL.\n");
        return errorVal;
    }
    
    return count;
}

extern int writeDevice(const struct SWDeviceEntry* device, FILE* devFile){
    
    /* Local Vars */
    int count = 0;

    /* Temp Vars */
    int numPri = 0;

    /* input check */
    if(device == NULL){
        fprintf(stderr, "writeDevice: 'devices' must not be NULL.\n");
        return errorVal;
    }
    if(devFile == NULL){
        fprintf(stderr, "writeDevice: 'devFile' must not be NULL.\n");
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
    numPri = fprintf(devFile, "%4.4" PRIxSWAddr " ", device->devInfo.swAddr);  
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
    numPri = fprintf(devFile, "%16.16" PRIxDevType " ",
                     device->devInfo.devTypes);    
    if(numPri < 1){
        fprintf(stderr, "Could not write near third pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write Number of Device Channels */
    numPri = fprintf(devFile, "%2.2" PRIxNumChan " ", device->devInfo.numChan);
    if(numPri < 1){
        fprintf(stderr, "Could not write near fourth pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write SW Group ID */
    numPri = fprintf(devFile, "%2.2" PRIxGrpID " ", device->devInfo.groupID);
    if(numPri < 1){
        fprintf(stderr, "Could not write near fifth pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write SW Protocol version */
    numPri = fprintf(devFile, "%2.2" PRIxSWVer " ", device->devInfo.version);
    if(numPri < 1){
        fprintf(stderr, "Could not write near sixth pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    /* Write Device Unique ID */
    numPri = fprintf(devFile, "%16.16" PRIxDevUID, device->devInfo.uid);
    if(numPri < 1){
        fprintf(stderr, "Could not write near seventh pos.\n");
        return errorVal;
    }
    else {
        count += numPri;
    }
    
    return count;;
}

/* Function to return an array of structs describing
 * active SmartWall devices */
int getDevices(struct SWDeviceEntry* devices, const int maxDevices,
               FILE* devFile){

    /* Function Vars */
    int devCnt = 0;    

    /* Temp Vars */
    struct SWDeviceEntry deviceTmp;

    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "getDevices: 'devices' must not be NULL.\n");
        return errorVal;
    }
    if(devFile == NULL){
        fprintf(stderr, "getDevices: 'devFile' must not be NULL.\n");
        return errorVal;
    }
    if(maxDevices <= 0){
        fprintf(stderr, "getDevices: 'maxDevices' must be > 0.\n");
        return errorVal;
    }

    /* Reset File Pointer */
    rewind(devFile);
    
    /* Read File */
    while(!feof(devFile)){
        /* Read From File */
        if(readDevice(&deviceTmp, devFile, devCnt) < 0){
            fprintf(stderr, "Line %d of devFile has bad data.\n",
                    devCnt);
            devCnt = errorVal;
            break;
        }

        /* Add device info to struct in array*/
        if(devCnt < maxDevices){
            memcpy(&(devices[devCnt]), &deviceTmp, sizeof(devices[devCnt]));
        }
        else{
            fprintf(stderr, "Max number of devices exceeded.\n");
            devCnt = errorVal;
            break;
        }

        /* Increment */
        devCnt++;
    }
    
    return devCnt;
}

extern FILE* updateDevices(struct SWDeviceEntry* devices,
                           int* numDevices, 
                           FILE* devFile){
    
    /* Local Vars */
    char filename[MAX_FILENAME_LENGTH];
    int i = 0;

    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "updateDevices: 'devices' must not be NULL.\n");
        return NULL;
    }
    if(devFile == NULL){
        fprintf(stderr, "updateDevices: 'devFile' must not be NULL.\n");
        return NULL;
    }
    if(*numDevices <= 0){
        fprintf(stderr, "updateDevices: 'numDevices' must be > 0.\n");
        return NULL;
    }

    /* Reset File Pointer */
    rewind(devFile);
    
    /* Close file and overwrite */
    if(fclose(devFile) != 0){
        fprintf(stderr, "updateDevices: Could not close devFile.\n");
        return NULL;
    }
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) < 0){
        fprintf(stderr, "updateDevices: Could not create filename.\n");
        return NULL;
    }
    devFile = fopen(filename, "w+");
    if(devFile == NULL){
        fprintf(stderr, "updateDevices: Could not open devFile.\n");
        return NULL;
    }

    /* Write Out Data */
    for(i = 0; i < *numDevices; i++){
        if(devices[i].lineNum >= 0){
            if(writeDevice(&devices[i], devFile) < 0){
                fprintf(stderr, "removeDevices: Error writing device to "
                        "line %d.\n", devices[i].lineNum);
                return NULL;
            }
        }
    }
    
    /* Reset File Pointer */
    rewind(devFile);

    /* Reread Devices */
    *numDevices = getDevices(devices, *numDevices, devFile);
    return devFile;
}

extern int sortDevices(struct SWDeviceEntry* devices, const int numDevices){
    
    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "sortDevices: devices must not be NULL.\n");
        return errorVal;
    }
    if(numDevices <= 0){
        fprintf(stderr, "sortDevices: numDevices must be > 0.\n");
        return errorVal;
    }

    /* Perform Quick Sort*/
    qsort(devices, numDevices, sizeof(*devices),
          (int(*)(const void*, const void*)) compSWDeviceEntryBySWAddr);
    
    return 0;

}

/* Input devices must be sorted to work properly*/
extern struct SWDeviceEntry* findDevice(const swAddress_t swAddress,
                                        const struct SWDeviceEntry* devices,
                                        const int numDevices){
    
    /* Local vars */
    struct SWDeviceEntry* tmpDev = NULL;

    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "findDevice: devices must not be NULL.\n");
        return NULL;
    }
    if(numDevices <= 0){
        fprintf(stderr, "findDevices: numDevices must be > 0.\n");
        return NULL;
    }
    
    /* Temp Key Device */
    struct SWDeviceEntry deviceKey;
    deviceKey.devInfo.swAddr = swAddress;

    /* Perform Binary Search */
    tmpDev = (struct SWDeviceEntry*) bsearch (&deviceKey, devices,
                                              numDevices, sizeof(*devices),
                                              (int(*)(const void*,const void*))
                                              compSWDeviceEntryBySWAddr);

    /* Check Output and Copy Value If Appropriate*/
    if(tmpDev == NULL){
        fprintf(stderr, "findDevices: Could not locate device with address "
                "0x%4.4" PRIxSWAddr ".\n", swAddress);
        return NULL;
    }
    else{
        return tmpDev;
    }
}

extern int compSWDeviceEntryBySWAddr(const struct SWDeviceEntry* d1,
                                     const struct SWDeviceEntry* d2){
    /* Check Input */
    if(d1 == NULL){
        fprintf(stderr, "compSWDeviceEntry: d1 NULL, returning -1\n");
        return errorVal;
    }
    if(d2 == NULL){
        fprintf(stderr, "compSWDeviceEntry: d2 NULL, returning -1\n");
        return errorVal;
    }

    /* Compare */
    return (d1->devInfo.swAddr - d2->devInfo.swAddr);
}

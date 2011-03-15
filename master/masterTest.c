/* Andy Sayler
 * SmartWall Project
 * SmartWall Master
 * masterTest.c
 * Created 3/15/11
 *
 * Change Log:
 * 03/15/11 - Created
 * ---
 */

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;

    /* Get Device Filename */
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) <= 0){
        fprintf(stderr, "printDevices: Could not create devFile name.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Open Device File */
    devFile = openDevFile(filename, "r");
    if(devFile == NULL){
        fprintf(stderr, "printDevices: Could not open devFile.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES, devFile);
    if(devCnt < 0){
        fprintf(stderr, "printDevices: "
                "Error gettign device list: getDevices returned %d\n",
                devCnt);
        exit(EXIT_FAILURE);
    }
    
    /* Close Device File */
    if(closeDevFile(devFile) != 0){
        fprintf(stderr, "printDevices: Could not close devFile.\n");
        exit(EXIT_FAILURE);
    }
    devFile = NULL;
        
    return 0;
}

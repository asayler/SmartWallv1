/* Andy Sayler
 * SmartWall Project
 * User Monitor Test Main
 * testMain.c
 * Created 2/5/11
 *
 * Change Log:
 * 02/05/11 - Created
 * ---
 */

#include <stdio.h>
#include "usermon.h"

#define MAXDEVICES MASTER_MAXDEVICES

int main(int argc, char* argv[]){

    (void) argc;
    (void) argv;

    fprintf(stdout, "\n");

    fprintf(stdout, "\nSW User Mon Test: printDevicesHex returned %d\n\n",
            printDevicesHex(stdout));

    fprintf(stdout, "\nSW User Mon Test: printDevices returned %d\n\n",
            printDevices(stdout));

    return 0;
}

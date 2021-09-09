#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>  
#include <dlfcn.h>
#include <stdint.h>
#include "NxrMoudle.h"
#include "INxrSensorClient.h"

using namespace nxr_client;
using namespace android;

static uint64_t currentTime = 0;
bool status = false;
INxrSensorClient *client = NULL; 

// static inline bool canPrintf(){
//     uint64_t time = getTicksNanos();
//     if((time-currentTime) >= 1000000000){
//         currentTime = time;
//         return true;
//     }
//     return false;
// }

int selectMain(int args){
    int ret = 1;
    int i = 0;
    if(client != NULL && status){
        switch(args){
            case 1:
                NxrSharedData tracking;
                while(i++ <= 1000*30){
                    client->getTrackingData(&tracking);
                    // printf("Tracking Data %s\n",floatAryToString(tracking.rotation,4).data());
                }
                break;
            case 2:
                NxrRawData sensor;
                while(i++ <= 1000*30){
                    client->getNxrRawData(&sensor);
                    // printf("Raw Data acc %s gyro %s mag %s\n",floatAryToString(sensor.acc,3).data(),floatAryToString(sensor.gyro,3).data(),floatAryToString(sensor.mag,3).data());
                }
                break;
            case 3:
                client->enableExternalData(true);
                while(true){
                    float rotation[4];
                    float translation[3];
                    float gyro[3];
                    client->updateExternalData(rotation,translation,gyro);
                }
                break;
            case 4:
                ret = 0;
                break;
        }
    }
    return ret;
}

int main(void){
    NxrMoudle moudle;
    moudle.open("libnxrsensor_client.so");
    client = (INxrSensorClient *)moudle.getNxrClient();
    // client = new NxrSensorClient();
    status = client->init();
    static const char meun_main[]={
        "\n\nnibiru interface demo\n"
        "------------------------------\n"
        "1 : Get Tracking Data\n"
        "2 : Get Raw Data\n"
        "3 : Get Raw Data\n"
        "4 : Exit program\n"
        "------------------------------\n"
        "enter select:"
    };

    while (1){
        printf("%s\n", meun_main);
        int a;
        if(scanf("%d",&a) > 0){
            if(a == 0) continue;
            if(selectMain(a) == 0) break;
        }
        usleep(1000000);
    }
    client->destroy();
    client = NULL;
	return EXIT_SUCCESS;
}





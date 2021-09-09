#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>  
#include <dlfcn.h>
#include <stdint.h>
#include <cutils/log.h>
#include "NxrMoudle.h"
#include "INxrSensorClient.h"

using namespace nxr_client;
using namespace std;
using namespace android;

static uint64_t currentTime = 0;
INxrSensorClient *vsc = NULL;
bool status = false;
// StreamType type = STREAM_NONE;
int count2 = 0;
// bool plane = false;
int fisheyeFPS = 0;
int rgbFPS = 0;
int eyeFPS = 0;
int tofFPS = 0;
int thermalFPS = 0;


static inline int64_t getTicksNanos() {
    struct timespec tp;
    const int status = clock_gettime(CLOCK_MONOTONIC, &tp);
    const int64_t result =
            (int64_t) tp.tv_sec * (int64_t) (1000 * 1000 * 1000) + (int64_t) (tp.tv_nsec);
    return result;
}

static inline bool canPrintf(){
    uint64_t time = getTicksNanos();
    // printf("step %lu\n",(time-currentTime));
    if((time-currentTime) >= 1000000000){
        currentTime = time;
        return true;
    }
    return false;
}

static inline string charAryToString(unsigned char *ary, int len) {
    string s = "[ ";
    if (!ary) return "[]";
    for (int i = 0; i < len; i++) {
        s += to_string(ary[i]);
        if (i < len - 1) s += ",";
    }
    s += "]";
    return s;
}

static inline string shortAryToString(short *ary, int len) {
    string s = "[ ";
    if (!ary) return "[]";
    for (int i = 0; i < len; i++) {
        s += to_string(ary[i]);
        if (i < len - 1) s += ",";
    }
    s += "]";
    return s;
}

void onStreamDataV2(StreamType type,NxrStreamV2 *data){
    if(type == STREAM_FISHEYE){
        fisheyeFPS++;
    }else if(type == STREAM_RGB){
        rgbFPS++;
    }else if(type == STREAM_TOF){
        tofFPS++;
    }else if(type == STREAM_EYETRACKING){
        eyeFPS++;
    }else if(type == STREAM_THERMAL){
        thermalFPS++;
    }
    uint64_t time = getTicksNanos();
    // printf("step %lu\n",(time-currentTime));
    if((time-currentTime) >= 1000000000){
        printf("fishEyeFps %d EyeFps %d RgbFps %d TofFps %d ThermamlFps %d\n",fisheyeFPS,eyeFPS,rgbFPS,tofFPS,thermalFPS);
        fisheyeFPS = 0;
        rgbFPS = 0;
        tofFPS = 0;
        eyeFPS = 0;
        thermalFPS = 0;
        currentTime = time;
    }
    printf("stream leftdata %d %hu %hu %s",data->leftDataSize,data->leftData[0].x,data->leftData[0].y,shortAryToString(data->leftData[0].data,16).data());
    printf("stream rightdata %d %hu %hu %s",data->rightDataSize,data->rightData[0].x,data->rightData[0].y,shortAryToString(data->rightData[0].data,16).data());
    printf("stream type %d size %d format %d width %d height %d timestamp %d type %d value %s\n",type,data->size,data->format,data->width,data->height,data->timestamp,data->type,charAryToString(data->data,10).data());
}

void onStreamData(StreamType type,NxrStreamData *data){
    if(type == STREAM_FISHEYE){
        fisheyeFPS++;
    }else if(type == STREAM_RGB){
        rgbFPS++;
    }else if(type == STREAM_TOF){
        tofFPS++;
    }else if(type == STREAM_EYETRACKING){
        eyeFPS++;
    }else if(type == STREAM_THERMAL){
        thermalFPS++;
    }
    uint64_t time = getTicksNanos();
    // printf("step %lu\n",(time-currentTime));
    if((time-currentTime) >= 1000000000){
        printf("fishEyeFps %d EyeFps %d RgbFps %d TofFps %d ThermamlFps %d\n",fisheyeFPS,eyeFPS,rgbFPS,tofFPS,thermalFPS);
        fisheyeFPS = 0;
        rgbFPS = 0;
        tofFPS = 0;
        eyeFPS = 0;
        thermalFPS = 0;
        currentTime = time;
    }
    // if(canPrintf()){
       
        printf("stream type %d size %d format %d width %d height %d timestamp %d type %d value %s\n",type,data->size,data->format,data->width,data->height,data->timestamp,data->type,charAryToString(data->data,10).data());
        // if(type == STREAM_FISHEYE){
        //     printf("left size %d right size %d left %s right %s\n",data->leftSize,data->rightSize,charAryToString(data->left,10).data(),charAryToString(data->right,10).data());
        // }
    // }
}

void onPlane(NxrPlaneData *s){
    // if(canPrintf()){
        printf("plane size %d id %d %lld %lld %f %f %f\n",s->size,s->id,s->deviceTimestamp,s->hostTimestamp,s->normal.x,s->normal.y,s->normal.z);
        printf("points %f %f %f %f %f %f\n",s->points[0].x,s->points[0].y,s->points[0].z,s->points[s->size-2].x,s->points[s->size-2].y,s->points[s->size-2].z);
        // printf("points %f %f %f %f %f %f\n",s->points[0].x,s->points[0].y,s->points[0].z,s->points[s->size-2].x,s->points[s->size-2].y,s->points[s->size-2].z);
    // }
}

void onStatusQuality(int status,int quality){
    printf("status %d quality %d\n",status,quality);
}

void onQuality(int quality){
    printf("quality %d\n",quality);
}

void onPercent(float percent){
    if(canPrintf()){
        printf("percent %f\n",percent);
    }
}

void ledControl(void){
    int r1,r2,r3;

	uint32_t selector;
	uint32_t channel;
	uint32_t brightness;

	const char menubuf[] = "enter et led selector id [0, 1]: ";
	printf("%s\n", menubuf);
    r1 = scanf("%d", &selector);

	const char menubuf1[] = "enter et led channel[0-7]: ";
	printf("%s\n", menubuf1);
    r2 =scanf("%d", &channel);

	const char menubuf2[] = "enter et led brightness [0-255]: ";
	printf("%s\n", menubuf2);
    r3 = scanf("%d", &brightness);

    if (r1<=0 || r2<=0|| r3<=0)
        return;

	printf("selector,channel,brightness: %d,%d,%d\n", selector,channel,brightness);
    int32_t values[3];
    values[0] = selector;
    values[1] = channel; 
    values[2] = brightness;
    if(vsc != NULL && status){
        for(int i=0;i<8;i++){
            values[1] = i;
            vsc->setParameterIA(90002,values,3);
        }
    }
}

void aecControl(void){
    int r1,r2,r3;

	uint32_t aecmode = 0;
	uint32_t aecgain;
	uint32_t aec_time;

	const char menubuf[] = "enter et aec mode [0:auto, 1:manul]: ";
	printf("%s\n", menubuf);
    r1 = scanf("%d", &aecmode);

	const char menubuf1[] = "enter et gain[0-255]: ";
	printf("%s\n", menubuf1);
	r2 = scanf("%d", &aecgain);

	const char menubuf2[] = "enter exposure [0-330] ";
	printf("%s\n", menubuf2);
	r3 = scanf("%d", &aec_time);

    if (r1<=0 || r2<=0|| r3<=0)
        return;
    int32_t values[3];
    values[0] = aecmode;
    values[1] = aecgain; 
    values[2] = aec_time;
    if(vsc != NULL && status){
        vsc->setParameterIA(90001,values,3);
    }
}

void isSupportFeature(void){
    int r;
    const char menubuf[] = "enter feature 1~12: ";
	printf("%s\n", menubuf);
    uint32_t feature = 0;
    r = scanf("%d", &feature);
    if(vsc != NULL && status){
        bool res = vsc->isSupportFeature((Feature)feature);
        printf("Feature %d support status %d",feature,res);
    }
}

static inline string doubleAryToString(double *ary, int len) {
    string s = "[ ";
    if (!ary) return "[]";
    for (int i = 0; i < len; i++) {
        s += to_string(ary[i]);
        if (i < len - 1) s += ",";
    }
    s += "]";
    return s;
}

static inline string floatAryToString(float *ary, int len) {
    string s = "[ ";
    if (!ary) return "[]";
    for (int i = 0; i < len; i++) {
        s += to_string(ary[i]);
        if (i < len - 1) s += ",";
    }
    s += "]";
    return s;
}

void readCalibration(void){
    int r;
    const char menubuf[]={
        "------------------------------\n"
        "1 : Fisheye\n"
        "2 : Fisheye2\n"
        "3 : Fisheye PDM\n"
        "4 : Tof\n"
        "5 : Rgb\n"
        "6 : Display\n"
        "------------------------------\n"
        "enter select:"
    };
	printf("%s\n", menubuf);
    uint32_t type = 0;
    r = scanf("%d", &type);
    if(vsc != NULL && status){
        bool res = false;
        if(type == 1){
            FishEyeCalibration calibration;
            res = vsc->readStereoFisheyesCalibration(calibration);
            printf("res %d\n",res);
            if(res){
                printf("FishEyeCalibration K[%s , %s] rotation[%s , %s] translation[%s , %s]\n",doubleAryToString(calibration.K[0],7).data(),doubleAryToString(calibration.K[1],7).data(),doubleAryToString(calibration.rotation[0],9).data(),doubleAryToString(calibration.rotation[1],9).data(),doubleAryToString(calibration.translation[0],3).data(),doubleAryToString(calibration.translation[1],3).data());
            }
        }else if(type == 2){
            FishEyeCalibration calibration;
            int imu;
            res = vsc->readStereoFisheyesCalibration(calibration,imu);
            printf("res %d\n",res);
            if(res){
                printf("FishEyeCalibration %d K[%s , %s] rotation[%s , %s] translation[%s , %s]\n",imu,doubleAryToString(calibration.K[0],7).data(),doubleAryToString(calibration.K[1],7).data(),doubleAryToString(calibration.rotation[0],9).data(),doubleAryToString(calibration.rotation[1],9).data(),doubleAryToString(calibration.translation[0],3).data(),doubleAryToString(calibration.translation[1],3).data());
            }
        }else if(type == 3){
            FishEyePDMCalibration calibration;
            res = vsc->readStereoFisheyesPDMCalibration(calibration);
            printf("res %d\n",res);
            if(res){
                printf("FishEyePDMCalibration K[%s , %s] rotation[%s , %s] translation[%s , %s]\n",doubleAryToString(calibration.K[0],11).data(),doubleAryToString(calibration.K[1],11).data(),doubleAryToString(calibration.rotation[0],9).data(),doubleAryToString(calibration.rotation[1],9).data(),doubleAryToString(calibration.translation[0],3).data(),doubleAryToString(calibration.translation[1],3).data());
            }
        }else if(type == 4){
            TOFCalibration calibration;
            res = vsc->readTofCalibration(calibration);
            printf("res %d\n",res);
            if(res){
                printf("TOFCalibration K[%s] rotation[%s] translation[%s]\n",doubleAryToString(calibration.K,11).data(),doubleAryToString(calibration.rotation,9).data(),doubleAryToString(calibration.translation,3).data());
            }
        }else if(type == 5){
            RGBCalibration calibration;
            res = vsc->readRgbCalibration(calibration);
            printf("res %d\n",res);
            if(res){
                printf("RGBCalibration K[%s , %s , %s] rotation[%s] translation[%s]\n",doubleAryToString(calibration.K[0],11).data(),doubleAryToString(calibration.K[1],11).data(),doubleAryToString(calibration.K[2],11).data(),doubleAryToString(calibration.rotation,9).data(),doubleAryToString(calibration.translation,3).data());
            }
        }else if (type == 6){
            NxrDisplayCalibration calibration;
            res = vsc->readDisplayCalibration(&calibration);
            printf("res %d\n",res);
            if(res){
                printf("NxrDisplayCalibration K[%s , %s] rotation[%s , %s] translation[%s , %s]\n",floatAryToString(calibration.K[0],11).data(),floatAryToString(calibration.K[1],11).data(),floatAryToString(calibration.rotation[0],9).data(),floatAryToString(calibration.rotation[1],9).data(),floatAryToString(calibration.translation[0],3).data(),floatAryToString(calibration.translation[1],3).data());
            }
        }
       
        // bool res = vsc->read((Feature)feature);
        // printf("Feature %d support status %d",feature,res);
    }
}

void startStreaming(void){
    int r;
    // const char menubuf[] = "enter type 1~4: ";
    const char menubuf[]={
        "------------------------------\n"
        "1 : Eyetracking\n"
        "2 : Rgb\n"
        "3 : Tof\n"
        "4 : Fisheye\n"
        "5 : Thermal\n"
        "------------------------------\n"
        "enter select:"
    };
	printf("%s\n", menubuf);
    uint32_t type = 0;
    r = scanf("%d", &type);
    if(vsc != NULL && status){
        if(type > STREAM_NONE && type <= STREAM_THERMAL){
            if(type == STREAM_TOF){
                int values[1];
                values[0] = 1;
                vsc->setParameterIA(90004, values, 1);

                values[0] = 5;
                vsc->setParameterIA(90006, values, 1);
            }
            bool res = vsc->startStreaming((StreamType)type,onStreamData);
            printf("startStreaming res %d\n",res);
        }
    }
}

void stopStreaming(void){
    printf("stopSreaming\n");
    int r;
    const char menubuf[]={
        "------------------------------\n"
        "1 : Eyetracking\n"
        "2 : Rgb\n"
        "3 : Tof\n"
        "4 : Fisheye\n"
        "5 : Thermal\n"
        "------------------------------\n"
        "enter select:"
    };
	printf("%s\n", menubuf);
    uint32_t type = 0;
    r = scanf("%d", &type);
    if(vsc != NULL && status){
        if(type > STREAM_NONE && type <= STREAM_THERMAL){
            bool res = vsc->stopStreaming((StreamType)type);
            printf("stopStreaming res %d\n",res);
        }
    }
}

void onSkeleton(NxrSkeleton *skeleton){
    printf("onSkeleton size %d\n",skeleton->size);
}

void onGesture(NxrGesture *gesture){
    printf("onGesture gesture %d %d\n",gesture->gesture[0],gesture->gesture[1]);
}

void useExternalData(void){
    int i = 0;
    vsc->enableExternalData(true);
    while(i++ <= 1000*30){
        float rotation[4];
        float translation[3];
        float gyro[3];
        vsc->updateExternalData(rotation,translation,gyro);
        // printf("Raw Data acc %s gyro %s mag %s\n",floatAryToString(sensor.acc,3).data(),floatAryToString(sensor.gyro,3).data(),floatAryToString(sensor.mag,3).data());
    }
    // vsc->enableExternalData(false);            
}

int selectMain(int args){
    int ret = 1;
    if(vsc != NULL && status){
        switch(args){
            case 1:
                startStreaming(); 
                break;
            case 2:
                stopStreaming();
                break;
            case 3:
                if(vsc->startPlaneDetection(onPlane)){
                    printf("startPlaneDetection success\n");
                }else{
                    printf("startPlaneDetection failed\n");
                }
                break;
            case 4:
                if(vsc->stopPlaneDetection()){
                    printf("stopPlaneDetection success\n");
                }else{
                    printf("stopPlaneDetection failed\n");
                }
                break;
            case 5:
                vsc->loadMapAndSwitchToCslam("/sdcard/xvisio_map.bin",onQuality,onPercent);
                break;
            case 6:
                vsc->saveMapAndSwitchToCslam("/sdcard/xvisio_map.bin",onStatusQuality,onPercent);
                break;
            case 7:
                vsc->switchToCslam(onQuality,onPercent);
                break;
            case 8:
                ledControl();
                break;
            case 9:
                aecControl();
                break;
            case 10:
                unsigned char value[3];
                memset(value,0,sizeof(unsigned char)*3);
                // buff[0] = 0x01;
                // buff[1] = 0x02;
                // buff[2] = 0x03;
                // buff[3] = 0x04;
                value[0] = 0x02;
                value[1] = 0xde;
                value[2] = 0x56;
                // if(vsc->hidWrite(value,5)){
                //     printf("hid write succ!!!");
                // }else{
                //     printf("hid write failed!!!");
                // }
                unsigned char buff[63];
                // if(vsc->hidRead(buff,63)){
                //     printf("hid read succ!!!");
                // }else{
                //     printf("hid read failed!!!");
                // }
                if(vsc->hidWriteAndRead(value,3,buff,63)){
                    printf("hid write and read succ\n");
                    printf("buff %s\n", (char *) &buff[3]);
                }else {
                    printf("hid write and read failed\n");
                }
                break;
            case 11:
                unsigned char temp[63];
                memset(temp,0,sizeof(unsigned char)*63);
                vsc->hidRead(temp,63);
                break;
            case 12:
                vsc->switchVRMode(TRACKING_6DOF);
                break;
            case 13:
                vsc->switchVRMode(TRACKING_3DOF);
                break;
            case 14:
                isSupportFeature();
                break;
            case 15:
                readCalibration();
                break; 
            case 16:
                useExternalData();
                break;
            case 17:
                vsc->extract3dPointsFromMap("/sdcard/xvisio_map.bin","/sdcard/output.csv");
                break;          
            case 18:
                vsc->startSkeleton(1,&onSkeleton);
                break;   
            case 19:
                vsc->startGesture(1,&onGesture);
                break;          
            case 20:
                ret = 0;
                break;
            default:
                break;
        }
    }
    return ret;
}

int main(void){
    NxrMoudle moudle;
    bool ret =  moudle.open("libnxrsensor_client.so");
    if(ret){
        vsc = (INxrSensorClient *)moudle.getNxrClient();
    }
    // vsc = new NativeVSC();
    if(!ret || vsc == NULL){
        return EXIT_SUCCESS;
    }
    status = vsc->initVSC();
    static const char meun_main[]={
        "\n\nnibiru interface demo\n"
        "------------------------------\n"
        "1 : Start streaming\n"
        "2 : Stop streaming\n"
        "3 : Start planedetection\n"
        "4 : Stop planedetection\n"
        "5 : Load switch to cslam(first to build map)\n"
        "6 : Save switch to cslam(first to build map)\n"
        "7 : Switch to cslam(first to build map)\n"
        "8 : Led control\n"
        "9 : Aec control\n"
        "10: Hid wirte\n"
        "11: Hid read\n"
        "12: Start mix mode to build map\n"
        "13: Stop mix mode\n"
        "14: Search feature status\n"
        "15: Read calibration\n"
        "16: Use External Data\n"
        "17: extract3dPointsFromMap(/sdcard/xvisio_map.bin,/sdcard/output.csv)\n"
        "18: Start Skeleton\n"
        "19: Start Gesture\n"
        "20: Exit program\n"
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
    vsc->uninitVSC();
    vsc = NULL;
	return EXIT_SUCCESS;
}





//
// Created by Mark on 2021/4/8.
//

#ifndef INXRSENSOR_CLIENT_H
#define INXRSENSOR_CLIENT_H

#include <stdlib.h>
#include "NxrSensorTypes.h"

using namespace nxr_client;

namespace android {
    class INxrSensorClient {
    public:
        virtual ~INxrSensorClient(){};
        
        virtual bool init() = 0;
        virtual int32_t getClientVersion() = 0;
        virtual int32_t getServiceVersion() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual void setTrackingMode(int32_t mode) = 0;
        virtual bool getHeadInfo(long prediction,float *dest,int length) = 0;
        virtual bool getHeadEuler(long prediction,double *dest,int length) = 0;
        virtual void destroy() = 0;
        virtual void reset() = 0;
        virtual bool is6Dof() = 0;
        virtual bool isSupportMatrix() = 0;
        virtual void setParameterI(int key,int value) = 0;
        virtual int getParameterI(int key) = 0;
        virtual void updateFeedData(void *feed) = 0;
        virtual bool getFeedData(void *dest) = 0;
        virtual bool getTrackingData(void *dest) = 0;
		virtual bool getNxrRawData(void *dest) = 0;
        virtual bool isSupportSlam() = 0;
        virtual void setParameterIA(int key,int *values,int length) = 0;
        virtual int getParameterIA(int key,int *result,int length) = 0;
		virtual bool switchVRMode(int mode) = 0;
		//for nxrslam 
        virtual bool initNxrSlam() = 0;
        virtual void pauseNxrSlam() = 0;
        virtual void resumeNxrSlam() = 0;
        virtual void destroyNxrSlam() = 0;
        virtual void setNxrSlamParameterI(int key,int value) = 0;
        virtual int getNxrSlamParameterI(int key) = 0;
        virtual int getNxrSlamPoseData(float *result,int length) = 0;
		//for vsc
        virtual bool initVSC() = 0;
        virtual void uninitVSC() = 0;
        virtual bool startStreaming(StreamType type,void(*f)(StreamType , NxrStreamData *)) = 0;
		virtual bool startStreamingV2(StreamType type,void(*f)(StreamType , NxrStreamV2 *)) = 0;
        virtual bool stopStreaming(StreamType type) = 0;
        virtual bool hidWrite(unsigned char *buff,int length) = 0;
        virtual bool hidRead(unsigned char *buff,int length) = 0;
        virtual bool loadMapAndSwitchToCslam(const char* filename,void(*f1)(int quality),void(*f2)(float percent)) = 0;
        virtual bool saveMapAndSwitchToCslam(const char* filename,void(*f1)(int status,int quality),void(*f2)(float percent)) = 0;
        virtual bool switchToCslam(void(*f1)(int quality),void(*f2)(float percent)) = 0;
        virtual bool isSupportFeature(Feature feature) = 0;
        virtual bool startPlaneDetection(void(*f)(NxrPlaneData *)) = 0;
        virtual bool stopPlaneDetection() = 0;
        virtual bool readStereoFisheyesCalibration(FishEyeCalibration &calibration) = 0;
        virtual bool readStereoFisheyesCalibration(FishEyeCalibration &calibration,int &imu_fisheye_shift_us) = 0;
        virtual bool readStereoFisheyesPDMCalibration(FishEyePDMCalibration &calibration) = 0;
        virtual bool readTofCalibration(TOFCalibration &calibration) = 0;
        virtual bool readRgbCalibration(RGBCalibration &calibration) = 0;
        virtual bool readDisplayCalibration(NxrDisplayCalibration *calibration) = 0;

        virtual void enableExternalData(bool enable) = 0;
        virtual void updateExternalData(float *roatition,float *translation,float *gyro) = 0;
        virtual bool extract3dPointsFromMap(const char *map,const char *out) = 0;
        virtual int redefineMapCoordinateFromTag(const char *inputMapFile,const char *outMapFile) = 0;
        virtual int redefineMapCoordinateFromTag(const char *inputMapFile,const char *outMapFile,double tag_square_size) = 0;
        virtual int detectTag(float *rotation,float *translation,uint8_t *confidence) = 0;
        virtual int detectTag(float *rotation,float *translation,uint8_t *confidence,double tag_square_size) = 0;
        virtual bool hidWriteAndRead(unsigned char *write,int wLen,unsigned char *read,int rLen) = 0;
        virtual int registerDetectTagCallback(fn_detect_tag_callback cb,double tag_square_size,int subsample_rate) = 0;
        virtual void unregisterDetectTagCallback(int id) = 0;
        virtual bool setDescriptor(const char *filepath) = 0;
        virtual bool setModel(const char *filepath) = 0;
        virtual bool setSource(int source) = 0;
        virtual int getSource() = 0;
        virtual Descriptor getDescriptor() = 0;
    
        virtual bool startSkeleton(int type,fn_skeleton_callback cb) = 0;
        virtual bool stopSkeleton(int type) = 0;
        virtual bool startGesture(int type,fn_gesture_callback cb) = 0;
        virtual bool stopGesture(int type) = 0;
    };
};//nxr_client

#endif //INXRSENSOR_CLIENT_H

#ifndef NXR_SENSOR_TYPES_H
#define NXR_SENSOR_TYPES_H

#include <vector>
#include <string>

namespace nxr_client{

    enum HelperTrackingMode {
        TRACKING_NONE = -1,
        TRACKING_3DOF,
        TRACKING_6DOF,
        TRACKING_SLAM,
    };

    struct NxrSharedData{
        float rotation[4];
        float translation[3];
        float prediction_coff_s[3];
        float prediction_coff_b[3];
        float prediction_coff_bdt[3];
        float prediction_coff_bdt2[3];
        long long int dataFetchTimeMs;
        long long int dataTimeStampMs;
        int source;//0->raw, 1->qvr
        int black;
        int rotateY;
    };

    struct NxrStreamData{
        unsigned char data[4096000];
        unsigned char left[2048000];
        unsigned char right[2048000];
        uint16_t type;
        uint16_t format;
        uint16_t width;
        uint16_t height;
        uint32_t size;
        uint32_t leftSize;
        uint32_t rightSize;
        uint64_t timestamp;
    };

    struct NxrHidData{
        unsigned char data[8888];
        int length;
        int type;//1=write,2=read
    };

    struct NxrRawData {
        float acc[3];
        float gyro[3];
        float mag[3];
        uint64_t dataTimeStamp;
        int quality;//acc quality
    };

    enum Feature {
        FEATURE_NONE = 0,
        FEATURE_VIO,
        FEATURE_CSLAM,
        FEATURE_MAPSHARE,
        FEATURE_RGB,
        FEATURE_TOF,
        FEATURE_FISHEYE,
        FEATURE_EYETRACKING,
        FEATURE_AEC,
        FEATURE_LED,
        FEATURE_LIGHT,
        FEATURE_AUDIO,
        FEATURE_PLANE,
    };

    enum StreamType {
        STREAM_NONE = 0,
        STREAM_EYETRACKING,
        STREAM_RGB,
        STREAM_TOF,
        STREAM_FISHEYE,
        STREAM_THERMAL,
    };

    struct NxrSlamData {
        float data[32];
        int length;
        long long int dataTimeStampMs;
        int source;
    };

    struct pointV2 {
        unsigned short x;
        unsigned short y;
        short data[16];
    };

    struct NxrStreamV2{
        unsigned char *data;
        unsigned char *left;
        unsigned char *right;
        pointV2 *leftData;
        pointV2 *rightData;
        uint16_t type;
        uint16_t format;
        uint16_t width;
        uint16_t height;
        uint32_t size;
        uint32_t leftSize;
        uint32_t rightSize;
        uint64_t timestamp;
        uint32_t leftDataSize;
        uint32_t rightDataSize;
        long long int frameNumber;
    };

    struct point {
        double x;
        double y;
        double z;
    };

    struct NxrPlaneData{
        long long hostTimestamp = 0; //!<Timestamp in µs read on host
        long long deviceTimestamp = 0; //!<Timestamp in µs read on the device
        point points[51200];
        point normal;
        int size;
        int id = -1;
    };

    struct NxrDisplayCalibration {
        float K[2][11];
        float rotation[2][9];
        float translation[2][3];
    };

    struct FishEyeCalibration {
        double K[2][7];
        double rotation[2][9];
        double translation[2][3];
    };

    struct FishEyePDMCalibration {
        double K[2][11];
        double rotation[2][9];
        double translation[2][3];
    };

    struct TOFCalibration{
        double K[11];
        double rotation[9];
        double translation[3];
    };

    struct RGBCalibration{
        double K[3][11];
        double rotation[9];
        double translation[3];
    };

    struct Descriptor {
        std::string type;
        std::vector<std::string> classes;
        double threshold = 0.5;
        bool flipStereo = false;
        bool flipRgb = false;
        bool flipTof = false;
    };

    struct Point{
        float x;
        float y;
        float z;
    };

    struct NxrSkeleton{
        int size;
        Point joints[2][21];
        long long int dataFetchTimeMs;
        long long int dataTimeStampMs;
    };

    struct NxrGesture{
        int size;
        int gesture[2];
        float distance;
        float confidence;
        Point position[2];
        Point slamPosition[2];
        long long int dataFetchTimeMs;
        long long int dataTimeStampMs;
    };

    typedef void (*fn_stream_callback)(StreamType type,NxrStreamData *);
    typedef void (*fn_stereo_callback)(StreamType type,NxrStreamV2 *stream);
    typedef void (*fn_quality_callback)(int);
    typedef void (*fn_status_quality_callback)(int,int);
    typedef void (*fn_percent_callback)(float);
    typedef void (*fn_plane_callback)(NxrPlaneData *);
    typedef void (*fn_detect_tag_callback)(float *rotation,float *translation,uint8_t confidence);
    typedef void (*fn_skeleton_callback)(NxrSkeleton *skeleton);
    typedef void (*fn_gesture_callback)(NxrGesture *gesture);
    
};//namespace nxr_client

#endif //NXR_SENSOR_TYPES_H
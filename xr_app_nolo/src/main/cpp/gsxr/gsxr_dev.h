//
// Created by Admin on 2021/6/30.
//

#ifndef LARKXRNATIVEDEMO_GSXR_DEV_H
#define LARKXRNATIVEDEMO_GSXR_DEV_H

#include <string>
#include <iostream>
#include <dev_api/gsxr_dev_hmd.h>
#include <dev_api/gsxr_dev_controller.h>
#include <dev_api/gsxr_dev_types.h>
#include <lark_xr/types.h>
#include "dev_api/gsxr_dev_types.h"
#include "../log.h"



class GsxrDev {

public:
    GsxrDev();
    ~GsxrDev();
    void Init();

    void GetHmdTracking(gsxr_pose_data_t &hmd_pose_data);
    void GetControllerTracking(gsxr_pose_data_t &hmd_pose_data,bool isLeft);

    void GetRightControllerState(const gsxr_pose_data_t& gsxr_pose_hmd, larkxrControllerDeviceState &deviceState);

    void GetLeftControllerState(const gsxr_pose_data_t& gsxr_pose_hmd, larkxrControllerDeviceState &deviceState);

    static gsxr_dev_id_t hmd_dev_id ,l_hand_dev_id ,r_hand_dev_id; //设备id

    void trigger_vibration(bool isLeft, const gsxr_haptic_vibration_t* haptics);

private:

   gsxr_dev_handle_t dev_hmd,dev_hand; //设备句柄

    gsxr_dev_event_callback_fn gsxr_callback_fn_hmd;
    gsxr_dev_event_callback_fn gsxr_callback_fn_hand;

    int h=1,l=2,r=3;
    void* hmd_cookie = (void*)&h;
    void* hand_cookie = (void*)&l;
    gsxr_dev_hmd_t* hmd;
    gsxr_dev_controller_t* hand;

};


#endif //LARKXRNATIVEDEMO_GSXR_DEV_H

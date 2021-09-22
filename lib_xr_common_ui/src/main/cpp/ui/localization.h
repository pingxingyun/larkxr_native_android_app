//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#ifndef CLOUDLARK_OCULUS_DEMO_LOCALIZATION_H
#define CLOUDLARK_OCULUS_DEMO_LOCALIZATION_H

#include <string>

namespace localization {
#if defined(LOCALIZATION_EN)
    const std::wstring ui_home_title                             = L"Application List";
    const std::wstring ui_home_device_name                       = L"Device Name";
    const std::wstring ui_home_mac_address                       = L"Mac Address";
    const std::wstring ui_home_setup_serveraddr                  = L"Setup Server Address";
    const std::wstring ui_home_advance_setting                   = L"Advanced Setting";
    const std::wstring ui_home_setup                             = L"Setting";
    const std::wstring ui_home_back                              = L"Back";
    const std::wstring ui_home_status_normal                     = L"Normal Setup";
    const std::wstring ui_home_status_device                     = L"VR Device";
    const std::wstring ui_home_reset                             = L"Restore Default Setting";
    const std::wstring ui_home_run_mode_teacher                  = L"Centralized management mode. Waiting for unified launch of the app.";
    const std::wstring ui_home_trigger_hit                       = L"Please use the handle Trackpad to confirm..";
    const std::wstring ui_advence_predit_title                   = L"Prediction";
    const std::wstring ui_advence_nolosetup_switch               = L"Nolo Setup";
    const std::wstring ui_advence_nolosetup_switch_normal        = L"Normal Setup";
    const std::wstring ui_advence_predit_open                    = L"Enable";
    const std::wstring ui_advence_predit_close                   = L"Disable";
    const std::wstring ui_advance_scale_clipping_plane_boundary  = L"ScaleClippingPlaneBoundary";
    const std::wstring ui_advence_set_nolo_hmd_trackcenter_z     = L"Nolo Head Pose Center Z";
    const std::wstring ui_advence_set_nolo_hmd_trackcenter_y     = L"Nolo Head Pose Center Y";
    const std::wstring ui_advence_set_nolo_predit                = L"Nolo PredictionTime";
    const std::wstring ui_advence_set_nolo_use_hmd_position      = L"Nolo HMD Position Offset";
    const std::wstring ui_setup_coderate                         = L"Coderate Setting";
    const std::wstring ui_setup_maincontroller                   = L"Main Controller";
    const std::wstring ui_setup_pitch                            = L"Pitch Setting";
    const std::wstring ui_setup_inited_height                    = L"Initial Height Setting";
    const std::wstring ui_setup_safe_zoon                        = L"Security Zone Setting";
    const std::wstring ui_setup_safe_vibrator                    = L"Virbrator";
    const std::wstring ui_setup_safe_zoon_auto                   = L"Auto";
    const std::wstring ui_setup_safe_zoon_close                  = L"Close";
    const std::wstring ui_setup_high_frame_rate                  = L"Frame Rate Mode";
    const std::wstring ui_setup_high_frame_rate_high             = L"High";
    const std::wstring ui_setup_high_frame_rate_normal           = L"Normal";

    const std::wstring ui_setup_serveraddr_current               = L"Current IP";
    const std::wstring ui_setup_serveraddr_hand                  = L"Manually set the ip address";
    const std::wstring ui_setup_serveraddr_auto                  = L"Automatic Search Server IP";
    const std::wstring ui_setup_serveraddr_auto_mode             = L"Automatic Search Mode";
    const std::wstring ui_setup_serveraddr_waring_empey          = L"Server address cannot be empty";
    const std::wstring ui_setup_serveraddr_auto_mode_start       = L"Automatically Find Server Address Mode Started";
#else
//    const std::wstring ui_home_title                             = L"应用列表";
//    const std::wstring ui_home_device_name                       = L"设备名称";
//    const std::wstring ui_home_mac_address                       = L"Mac 地址";
//    const std::wstring ui_home_setup_serveraddr                  = L"设置服务器地址";
//    const std::wstring ui_home_advance_setting                   = L"高级设置";
//    const std::wstring ui_home_setup                             = L"设置";
//    const std::wstring ui_home_back                              = L"返回";
//    const std::wstring ui_home_status_normal                     = L"通用设置";
//    const std::wstring ui_home_status_device                     = L"一体机设置";
//    const std::wstring ui_home_reset                             = L"恢复默认设置";
//    const std::wstring ui_home_run_mode_teacher                  = L"集中管控模式。等待统一启动应用。";
//    const std::wstring ui_home_trigger_hit                       = L"请使用手柄 Trackpad 键确认。";
//    const std::wstring ui_advence_predit_title                   = L"姿态预测设置";
//    const std::wstring ui_advence_nolosetup_switch               = L"Nolo设置";
//    const std::wstring ui_advence_nolosetup_switch_normal        = L"通用设置";
//    const std::wstring ui_advence_predit_open                    = L"开启";
//    const std::wstring ui_advence_predit_close                   = L"关闭";
//    const std::wstring ui_advance_scale_clipping_plane_boundary  = L"剪裁平面边界缩放";
//    const std::wstring ui_advence_set_nolo_hmd_trackcenter_z     = L"Nolo 头部定位器设置 Z";
//    const std::wstring ui_advence_set_nolo_hmd_trackcenter_y     = L"Nolo 头部定位器设置 Y";
//    const std::wstring ui_advence_set_nolo_predit                = L"Nolo 惯导预测时间";
//    const std::wstring ui_advence_set_nolo_use_hmd_position      = L"Nolo 头部定位偏移";
//    const std::wstring ui_setup_coderate                         = L"码率设置";
//    const std::wstring ui_setup_maincontroller                   = L"主控手设置";
//    const std::wstring ui_setup_pitch                            = L"瞳距设置";
//    const std::wstring ui_setup_inited_height                    = L"初始高度设置";
//    const std::wstring ui_setup_safe_zoon                        = L"安全区域设置";
//    const std::wstring ui_setup_safe_zoon_auto                   = L"自动";
//    const std::wstring ui_setup_safe_zoon_close                  = L"关闭";
//    const std::wstring ui_setup_safe_vibrator                    = L"是否震动手柄";
//    const std::wstring ui_setup_high_frame_rate                  = L"帧率模式";
//    const std::wstring ui_setup_high_frame_rate_high             = L"最高";
//    const std::wstring ui_setup_high_frame_rate_normal           = L"普通";
//
//    const std::wstring ui_setup_serveraddr_current               = L"当前连接地址";
//    const std::wstring ui_setup_serveraddr_hand                  = L"设置服务器地址";
//    const std::wstring ui_setup_serveraddr_auto                  = L"自动搜索服务器";
//    const std::wstring ui_setup_serveraddr_auto_mode             = L"自动搜索模式";
//    const std::wstring ui_setup_serveraddr_waring_empey          = L"服务器地址不能为空";
//    const std::wstring ui_setup_serveraddr_auto_mode_start       = L"自动寻找地址模式开始";
#endif
    struct LocalResource {
        std::wstring ui_home_title;
        std::wstring ui_home_device_name;
        std::wstring ui_home_mac_address;
        std::wstring ui_home_client_id;
        std::wstring ui_home_app_version;
        std::wstring ui_home_support_server_version;
        std::wstring ui_home_setup_serveraddr;
        std::wstring ui_home_advance_setting;
        std::wstring ui_home_quick_3d_ui;
        std::wstring ui_home_setup;
        std::wstring ui_home_back;
        std::wstring ui_home_status_normal;
        std::wstring ui_home_status_device;
        std::wstring ui_home_reset;
        std::wstring ui_home_run_mode_teacher;
        std::wstring ui_home_trigger_hit;
        std::wstring ui_advence_predit_title;
        std::wstring ui_advence_nolosetup_switch;
        std::wstring ui_advence_nolosetup_switch_normal;
        std::wstring ui_advence_predit_open;
        std::wstring ui_advence_predit_close;
        std::wstring ui_advance_scale_clipping_plane_boundary;
        std::wstring ui_advence_set_nolo_hmd_trackcenter_z;
        std::wstring ui_advence_set_nolo_hmd_trackcenter_y;
        std::wstring ui_advence_set_nolo_predit;
        std::wstring ui_advence_set_nolo_use_hmd_position;
        std::wstring ui_setup_coderate;
        std::wstring ui_setup_maincontroller;
        std::wstring ui_setup_pitch;
        std::wstring ui_setup_inited_height;
        std::wstring ui_setup_safe_zoon;
        std::wstring ui_setup_safe_zoon_auto;
        std::wstring ui_setup_safe_zoon_close;
        std::wstring ui_setup_safe_vibrator;
        std::wstring ui_setup_high_frame_rate;
        std::wstring ui_setup_high_frame_rate_high;
        std::wstring ui_setup_high_frame_rate_normal;

        std::wstring ui_setup_serveraddr_current;
        std::wstring ui_setup_serveraddr_hand;
        std::wstring ui_setup_serveraddr_auto;
        std::wstring ui_setup_serveraddr_auto_mode;
        std::wstring ui_setup_serveraddr_hand_mode;
        std::wstring ui_setup_serveraddr_waring_empey;
        std::wstring ui_setup_serveraddr_auto_mode_start;
        std::wstring ui_setup_serveraddr_ip;
        std::wstring ui_setup_serveraddr_port;
        std::wstring ui_setup_serveraddr_input_ip;
        std::wstring ui_setup_serveraddr_input_port;
        std::wstring ui_setup_serveraddr_currentmode;
        std::wstring ui_setup_serveraddr_submit;
        std::wstring ui_setup_serveraddr_autofind;

        std::wstring ui_setup_resolution_title;

        std::wstring ui_keyboard_number;
    };
    class Loader {
    public:
        static void load(bool isEn);
        static const LocalResource& getResource();
    private:
        static LocalResource localResource;
    };
};


#endif //CLOUDLARK_OCULUS_DEMO_LOCALIZATION_H

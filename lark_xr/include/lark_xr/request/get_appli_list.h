// 
// Created by fcx@pingxingyun.com
// 2020-01-09 18:07
//
#pragma once
#ifndef GET_APPLI_LIST_INCLUDE
#define GET_APPLI_LIST_INCLUDE

#include "../common.h"
#include <iostream>
#include <vector>

namespace lark
{
struct AppliInfo {
	std::string appliId;
	std::string appliName;
	std::string appliPath;
	int         appliType;
	std::string exeFileName;
	int         hasExtraParam;
	int         initWinSize;
	int         instanceMax;
	std::string param;
	int         runCnt;
	int         status;
	std::string updateDate;
	std::string zipFile;
	std::string picUrl;
	int         frameLowerLimit;
	int         initiativeSyncFlag;
	int         initCursorMode;
	int         offScreen;
	int         fullScreenMode;
	int         startProcType;
	int         useGamepad;
	int         mobileForceLandscape;
	int         mobileVirtualJoystick;
	int         mouseZoomDirection;
	int         playerListToggle;
	int         limitMaxFps;
	std::string touchOperateMode; // touchScreen mouse
	std::string playerModeEnable; //
};

struct AppliPageInfo {
	int pageNum;
	int pageSize;
	int size;
	int total;
	int pages;
	std::vector<AppliInfo> list;
	int prePage;
	int nextPage;
	bool hasPreviousPage;
	bool hasNextPage;
	int firstPage;
	int lastPage;
};

class GetAppliListListener {
public:
	virtual void OnSuccess(const AppliPageInfo& appliPageInfo) = 0;
	virtual void OnFailed(const std::string& msg) = 0;
};

class GetAppliListImp;
class LARK_COMMON_API GetAppliList {
public:
	GetAppliList();
	~GetAppliList();

	// send sync
	void Send();
	// send async
	void SendAsync();

	std::vector<AppliInfo> appli_info() const;
	AppliPageInfo appli_page_info() const;
	int http_code() const;
	int api_result_code() const;
	std::string message() const;
	bool is_success() const;

	void set_page(int page);
	void set_page_size(int pageSize);
	void set_callback(GetAppliListListener* callback);
private:
	GetAppliListImp* get_appli_list_imp_ = nullptr;
};
}
#endif // GET_APPLI_LIST_INCLUDE9
//
// Created by fcx on 2020/6/3.
//

#ifndef CLOUDLARKXR_CLIENT_LOG_ERROR_H
#define CLOUDLARKXR_CLIENT_LOG_ERROR_H

#include "../common.h"

namespace lark {
class ClientLogErrorImp;
class LARK_COMMON_API ClientLogError {
public:
    ClientLogError();
    ~ClientLogError();
    // send sync
    void Send(const std::string& errorCode, const std::string& errorMsg, const std::string& params, const std::string& clientMac);
    // send async
    void SendAsync(const std::string& errorCode, const std::string& errorMsg, const std::string& params, const std::string& clientMac);

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    ClientLogErrorImp* client_log_error_imp_;
};
}
#endif //CLOUDLARKXR_CLIENT_LOG_ERROR_H

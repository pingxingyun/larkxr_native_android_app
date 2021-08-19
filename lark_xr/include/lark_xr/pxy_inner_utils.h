//
// Created by Administrator on 2021/8/16.
//

#ifndef CLOUDLARKXR_PXY_INNER_UTILS_H
#define CLOUDLARKXR_PXY_INNER_UTILS_H

#include <string>


/**
 * WARNING 内部代码库使用，注意不要传到 github 上
 */
namespace lark {
std::string GetSignature(const std::string& appkey, const std::string& secret,
                         const std::string& utc_millseconds);
}


#endif //CLOUDLARKXR_PXY_INNER_UTILS_H

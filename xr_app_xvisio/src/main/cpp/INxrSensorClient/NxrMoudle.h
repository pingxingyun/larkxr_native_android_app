//
// Created by Mark on 2018/9/13.
//

#ifndef NXR_MOUDLE_H
#define NXR_MOUDLE_H

#include <stdint.h>
#include <dlfcn.h>

typedef void* (*__nxrclient_getNxrClient)();

class NxrMoudle {
public:
    NxrMoudle();

    ~NxrMoudle();

    bool open(const char* soname);

    bool isOpen();

    void close();

    void* getNxrClient();

private:
    void *handle;
    __nxrclient_getNxrClient nxrclient_getNxrClient;

private:
    void clear();
};

#endif //NXR_MOUDLE_H

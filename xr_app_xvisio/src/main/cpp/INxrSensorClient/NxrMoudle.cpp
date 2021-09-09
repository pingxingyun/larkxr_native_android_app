//
// Created by Mark on 2018/9/13.
//
// #include <cutils/log.h>
#include "NxrMoudle.h"

NxrMoudle::NxrMoudle()  {
    clear();
}

bool NxrMoudle::open(const char* soname){
    handle = dlopen(soname, RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL) {
        // ALOGI("dlopen NxrMoudle %s failed!!!",soname);
        return false;
    }
    nxrclient_getNxrClient = (__nxrclient_getNxrClient)dlsym(handle,"getNxrClient");
    return nxrclient_getNxrClient != NULL;
}

void NxrMoudle::clear(){
    handle = NULL;
    nxrclient_getNxrClient = NULL;
}

void NxrMoudle::close(){
    if (handle != NULL) {
        dlclose(handle);
    }
    clear();
}

NxrMoudle::~NxrMoudle() {
    close();
}

bool NxrMoudle::isOpen(){
    return handle != NULL;
}

void* NxrMoudle::getNxrClient(){
    if(nxrclient_getNxrClient != NULL){
        return nxrclient_getNxrClient();
    }
    return NULL;
}
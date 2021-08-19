/*
 * NVRenderer.h
 *
 *  Created on: 2016��3��18��
 *      Author: Steven
 */

#ifndef NVRSCENE_H_
#define NVRSCENE_H_
#include "nibiru/NibiruVRApi.h"

using namespace nvr;

class NVRScene {

public:
	virtual void onPreDraw(float * position) = 0;
	virtual void onDrawEye(nvr_Eye *eye) = 0;
	virtual void onDrawMultiView(nvr_Eye *eyes) = 0;
	virtual void onPostDraw() = 0;
	virtual void onSceneCreated() =0;
	virtual void onSceneDestroyed() =0;
	virtual void setAssets(JNIEnv *env, jobject &assets) =0;
	virtual void setNVRApi(NibiruVRApi *api) =0;
	virtual void setSupport6Dof(bool support) {
		this->support6Dof = support;
	}

	bool support6Dof;
};

#endif /* NVRSCENE_H_ */

/*
 * TestCardboard.cpp
 *
 *  Created on: 2016��3��22��
 *      Author: Steven
 */

#include "NVRScene.h"
#include <EGL/egl.h>
#include "time.h"
#include <unistd.h>			// for usleep
#include <GLES3/gl3.h>
#include <nibiru/NVRUtils.h>
#include "nibiru/NibiruVRApi.h"

using namespace nvr;

#define CAMERA_Z 0.1
#define TIME_DELTA 0.9
#define Z_NEAR 0.1
#define Z_FAR 100

class World: public NVRScene {
public:
	World() {

		LIGHT_POS_IN_WORLD_SPACE.x = 0.0;
		LIGHT_POS_IN_WORLD_SPACE.y = 2.0;
		LIGHT_POS_IN_WORLD_SPACE.z = 0.0;
		LIGHT_POS_IN_WORLD_SPACE.w = 1.0;

	}

	void setAssets(JNIEnv *env, jobject &assets) {
		mUtils.setAssetsManger(env, assets);
	}

	void setNVRApi(NibiruVRApi *api) {
		mNVRApi = api;
	}

	void onPreDraw(float * headPosition) {
		// 获取手柄连接状态:0=左手连接，1=右手连接，2=左右手连接
		int connnectStatus = mNVRApi->nvr_getContollerConnectStatus();

		if (connnectStatus == 1) {
			// 右手坐标系
			// 四元数的顺序: x-y-z-w
			// 位移: x y z
			float controllerRotPos[7];
			mNVRApi->nvr_getControllerRotPos(HAND_RIGHT, controllerRotPos, 7);
//			NLOGE("Controller Pose Info: %f,%f,%f,%f", controllerRotPos[0],
//					controllerRotPos[1], controllerRotPos[2],
//					controllerRotPos[3]);
			const int size = 11;
			KEY_CODE codeArray[size] = { KEYCODE_DPAD_UP, KEYCODE_DPAD_DOWN,
					KEYCODE_DPAD_LEFT, KEYCODE_DPAD_RIGHT, KEYCODE_DPAD_CENTER,
					KEYCODE_BUTTON_NIBIRU, KEYCODE_BUTTON_APP,
					KEYCODE_VOLUME_UP, KEYCODE_VOLUME_DOWN, KEYCODE_BUTTON_R1,
					KEYCODE_BUTTON_SELECT };
			for (int i = 0; i < size; i++) {
				// 手柄按键
				KEY_CODE keyCode = codeArray[i];
				if (mNVRApi->nvr_getControllerButtonDown(HAND_RIGHT, keyCode)) {
					NLOGE("Button Down : %d", keyCode);
				}

				if (mNVRApi->nvr_getControllerButtonUp(HAND_RIGHT, keyCode)) {
					NLOGE("Button Up : %d", keyCode);
				}

				if (mNVRApi->nvr_getControllerButtonPressed(HAND_RIGHT,
						keyCode)) {
					NLOGE("Button Pressed : %d", keyCode);
				}
			}

			if (mNVRApi->nvr_getControllerTouchDown(HAND_RIGHT)) {
				NLOGE("Touch Down");
				vec2 touchposition;
				mNVRApi->nvr_getControllerTouchPosition(HAND_RIGHT,
						touchposition);
				NLOGE("Touch Position : %f,%f", touchposition.x,
						touchposition.y);
			}

			if (mNVRApi->nvr_getControllerTouchUp(HAND_RIGHT)) {
				NLOGE("Touch Up");
			}

		}

		modelCube.setRotate(0, TIME_DELTA * rcount, 0.5, 0.5, 1.0);

		//NLOGE("%d_pos_%f,%f,%f", support6Dof, headPosition[0], headPosition[1],
		//headPosition[2]);
		if (support6Dof) {
			camera.setLookAtM(0, headPosition[0], headPosition[1],
					headPosition[2] + CAMERA_Z, headPosition[0],
					headPosition[1], headPosition[2], 0.0f, 1.0f, 0.0f);
		} else {
			camera.setLookAtM(0, 0.0f, 0.0f, CAMERA_Z, 0.0f, 0.0f, 0.0f, 0.0f,
					1.0f, 0.0f);
		}
		glCheck("ready to draw");

		rcount += 1;
//		 camera.lookAt()
	}

	void onDrawEye(nvr_Eye *eye) {

		glEnable (GL_DEPTH_TEST);

		if (!mNVRApi->nvr_IsEnableDirectRender()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		view = eye->eyeRotationView * camera;

		lightPosInEyeSpace = view * LIGHT_POS_IN_WORLD_SPACE;

		modelView = view * modelCube;

		modelViewProjection = eye->projection * modelView;

		drawCube();

		modelView = view * modelFloor;
		modelViewProjection = eye->projection * modelView;

		drawFloor();

		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void onDrawMultiView(nvr_Eye *eyes) {
	}

	void onPostDraw() {

	}

	void drawCube() {
		glUseProgram(cubeProgram);

		float posProg[4];
		lightPosInEyeSpace.value_ptr(posProg, 4);
		glUniform3fv(mCubeLightPosProg, 1, posProg);

		glUniformMatrix4fv(mCubeModelProg, 1, GL_FALSE, modelCube.value_ptr());
////
		glUniformMatrix4fv(mCubeModelViewProg, 1, GL_FALSE,
				modelView.value_ptr());
////
		glUniformMatrix4fv(mCubeModelViewProjProg, 1, GL_FALSE,
				modelViewProjection.value_ptr());

		glBindBuffer(GL_ARRAY_BUFFER, cubeVertices);

		glVertexAttribPointer(mCubePosProg, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mCubePosProg);

		glBindBuffer(GL_ARRAY_BUFFER, cubeNormal);

		glVertexAttribPointer(mCubeNormalProg, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mCubeNormalProg);

		glBindBuffer(GL_ARRAY_BUFFER, cubeColor);

		glVertexAttribPointer(mCubeColorProg, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mCubeColorProg);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glCheck("draw cube");
	}

	void drawFloor() {
		glUseProgram(floorProgram);

		float posProg[4];
		lightPosInEyeSpace.value_ptr(posProg, 4);
		glUniform3fv(mFloorLightPosProg, 1, posProg);

		glUniformMatrix4fv(mFloorModelProg, 1, GL_FALSE,
				modelFloor.value_ptr());

		glUniformMatrix4fv(mFloorModelViewProg, 1, GL_FALSE,
				modelView.value_ptr());

		glUniformMatrix4fv(mFloorModelViewProjProg, 1, GL_FALSE,
				modelViewProjection.value_ptr());

		glBindBuffer(GL_ARRAY_BUFFER, floorVertices);

		glVertexAttribPointer(mFloorPosProg, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mFloorPosProg);

		glBindBuffer(GL_ARRAY_BUFFER, floorNormals);

		glVertexAttribPointer(mFloorNormalProg, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mFloorNormalProg);

		glBindBuffer(GL_ARRAY_BUFFER, floorColor);
		glVertexAttribPointer(mFloorColorProg, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mFloorColorProg);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glCheck("draw floor");
	}

	void initWorld() {
		glClearColor(1.0f, 0.0f, 0.0f, 0.5f);

		rcount = 0;
		initData();

		ShaderUnit cube;
		mUtils.loadShaderUnit(&cube, "light_vertex.shader",
				"passthrough_fragment.shader");

		cubeProgram = cube.mProgram;
		glUseProgram(cubeProgram);

		glCheck("load cube program");

		mCubePosProg = glGetAttribLocation(cubeProgram, "a_Position");
		mCubeNormalProg = glGetAttribLocation(cubeProgram, "a_Normal");
		mCubeColorProg = glGetAttribLocation(cubeProgram, "a_Color");

		mCubeModelProg = glGetUniformLocation(cubeProgram, "u_Model");
		mCubeModelViewProg = glGetUniformLocation(cubeProgram, "u_MVMatrix");
		mCubeModelViewProjProg = glGetUniformLocation(cubeProgram, "u_MVP");
		mCubeLightPosProg = glGetUniformLocation(cubeProgram, "u_LightPos");

		glEnableVertexAttribArray(mCubePosProg);
		glEnableVertexAttribArray(mCubeNormalProg);
		glEnableVertexAttribArray(mCubeColorProg);

		ShaderUnit floor;
		mUtils.loadShaderUnit(&floor, "light_vertex.shader",
				"grid_fragment.shader");

		floorProgram = floor.mProgram;
		glUseProgram(floorProgram);

		glCheck("load floor program");

		mFloorPosProg = glGetAttribLocation(floorProgram, "a_Position");
		mFloorNormalProg = glGetAttribLocation(floorProgram, "a_Normal");
		mFloorColorProg = glGetAttribLocation(floorProgram, "a_Color");

		mFloorModelProg = glGetUniformLocation(floorProgram, "u_Model");
		mFloorModelViewProg = glGetUniformLocation(floorProgram, "u_MVMatrix");
		mFloorModelViewProjProg = glGetUniformLocation(floorProgram, "u_MVP");
		mFloorLightPosProg = glGetUniformLocation(floorProgram, "u_LightPos");

		glEnableVertexAttribArray(mFloorPosProg);
		glEnableVertexAttribArray(mFloorNormalProg);
		glEnableVertexAttribArray(mFloorColorProg);

		modelCube.setIdentity(0);
		modelCube.translate(0.0f, 0.0f, 0.0f, -8.0f);

		modelFloor.setIdentity(0);
		modelFloor.translate(0.0f, 0.0f, -25.0f, 0.0f);

		if (mNVRApi != NULL) {
			mNVRApi->nvr_SetGazeColor(1.0f, 1.0f, 1.0f);
			mNVRApi->nvr_ShowGaze();
		}

		glCheck("init completed");
	}

	void onSceneCreated() {
		initWorld();
	}

	void onSceneDestroyed()
	{
		glDeleteBuffers(1, &cubeVertices);
		glDeleteBuffers(1, &cubeColor);
		glDeleteBuffers(1, &cubeNormal);

		glDeleteBuffers(1, &floorVertices);
		glDeleteBuffers(1, &floorColor);
		glDeleteBuffers(1, &floorNormals);

		glDeleteProgram(cubeProgram);
		glDeleteProgram(floorProgram);

		cubeProgram = 0;
		floorProgram = 0;
		cubeVertices = 0;
		cubeColor = 0;
		cubeNormal = 0;
		floorVertices = 0;
		floorColor = 0;
		floorNormals = 0;
		NLOGD("onSceneDestroyed");

	}

	GLuint makeData(float *data, int len) {
		GLuint result = 0;
		glGenBuffers(1, &result);
		glBindBuffer(GL_ARRAY_BUFFER, result);
		glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return result;
	}

	void initData() {
		float cube_coords[] = {
				// Front face
				-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

				// Right face
				1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f,

				// Back face
				1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,

				// Left face
				-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

				// Top face
				-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,

				// Bottom face
				1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, };

		cubeVertices = makeData(cube_coords, sizeof(cube_coords));

		float cube_color[] = {
				// front, green
				0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273,
				0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0,
				0.5273, 0.2656, 1.0,

				// right, blue
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,

				// back, also green
				0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273,
				0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0,
				0.5273, 0.2656, 1.0,

				// left, also blue
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,

				// top, red
				0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f,
				0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f,
				0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f,
				1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f,

				// bottom, also red
				0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f,
				0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f,
				0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f,
				1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f, };

//		   int i = 0;
//
//		   while( i < (sizeof(cube_color) / sizeof(float)) ){
//			   LOGI("%f %f %f %f\n", cube_color[i], cube_color[i+1], cube_color[i+2], cube_color[i+3]);
//			   i += 4;
//		   }

		cubeColor = makeData(cube_color, sizeof(cube_color));

		float cube_normal[] = {
				// Front face
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

				// Right face
				1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

				// Back face
				0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

				// Left face
				-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

				// Top face
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

				// Bottom face
				0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f };

		cubeNormal = makeData(cube_normal, sizeof(cube_normal));

		float floor_coords[] = { 200, 0, -200, -200, 0, -200, -200, 0, 200, 200,
				0, -200, -200, 0, 200, 200, 0, 200, };

		floorVertices = makeData(floor_coords, sizeof(floor_coords));

		float floor_normal[] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, };

		floorNormals = makeData(floor_normal, sizeof(floor_normal));

		float floor_color[] = { 0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f,
				0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f,
				0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f,
				0.9023f, 1.0f, };

		floorColor = makeData(floor_color, sizeof(floor_color));

	}
private:
	NVRUtils mUtils;

	GLuint cubeVertices;
	GLuint cubeColor;
	GLuint cubeNormal;

	GLuint floorVertices;
	GLuint floorColor;
	GLuint floorNormals;

	GLuint cubeProgram;
	GLuint floorProgram;

	GLuint mCubePosProg;
	GLuint mCubeNormalProg;
	GLuint mCubeColorProg;

	GLuint mCubeModelProg;
	GLuint mCubeModelViewProg;
	GLuint mCubeModelViewProjProg;
	GLuint mCubeLightPosProg;

	GLuint mFloorPosProg;
	GLuint mFloorNormalProg;
	GLuint mFloorColorProg;

	GLuint mFloorModelProg;
	GLuint mFloorModelViewProg;
	GLuint mFloorModelViewProjProg;
	GLuint mFloorLightPosProg;

	int rcount;
	mat4 modelCube;
	mat4 camera;
	mat4 view;
	mat4 headView;
	mat4 modelViewProjection;
	mat4 modelView;
	mat4 modelFloor;

	vec4 lightPosInEyeSpace;
	vec4 LIGHT_POS_IN_WORLD_SPACE;

	NibiruVRApi *mNVRApi = NULL;

};


// 摄像机类
#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include <Angel.h>
#include <vector>

extern double program_speed;

namespace Cam {
	// 相机视角
	enum Camera_View {
		FPS_VIEW, TPS_VIEW, GOD_VIEW
	};

	// 相机移动方向
	enum Camera_Movement {
		FORWARD, BACKWARD, LEFT, RIGHT
	};

	// Default camera values
	const GLfloat YAW = -90.0f;
	const GLfloat PITCH = 0.0f;


	class Camera {
	public:
		// 相机的位置、前向向量、上向向量与右向向量
		vec3 position, front, up, right;
		// 欧拉角yaw和pitch
		GLfloat yaw, pitch;
		// 当前视角
		Camera_View current_view;

		// 默认构造
		Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);

		// 获得正交投影矩阵
		mat4 ortho(const GLfloat left, const GLfloat right,
			const GLfloat bottom, const GLfloat top,
			const GLfloat zNear, const GLfloat zFar);

		// 获得透视投影矩阵
		mat4 perspective(const GLfloat fovy, const GLfloat aspect,
			const GLfloat zNear, const GLfloat zFar);
		
		// 获得视图矩阵
		mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up);

		// 根据内部向量获得视图矩阵
		mat4 getViewMatrix();

		// 处理键盘
		void ProcessKeyboard(Camera_Movement direction);

		// 处理鼠标移动
		void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);

		// 处理第一人称(FPS)，第三人称(TPS)，上帝(GOD) 视角
		void ProcessView();
		void ProcessTPSView();
		void ProcessFPSView();
		void ProcessGODView();

		// 处理机器人死亡视角
		void ProcessRobotOverView();

		// 初始化相机的着色器
		void initShaderCamera();

		// 更新相机的着色器信息
		void displayShaderCamera();

		void changeView();

	private:
		// 更新相关向量
		void updateCameraVectors();

		// 角度值转弧度制
		GLfloat radians(GLfloat angle);

		// 视图矩阵，投影矩阵在着色器中的位置
		GLint ViewMatrixID;
		GLint ProjectionMatrixID;
	};

}

#endif  // __CAMERA_CLASS_H__
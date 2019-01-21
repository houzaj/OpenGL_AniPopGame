#include "Core_Camera.h"
#include "Model.h"

namespace Cam {
	// 默认为第一人称视角(FPS)
	Camera::Camera(vec3 position, vec3 up,
		GLfloat yaw, GLfloat pitch) : front(vec3(0.0f, 0.0f, -1.0f)),
		position(position), yaw(yaw), pitch(pitch) {
		updateCameraVectors();
		current_view = Camera_View::FPS_VIEW;
		ProcessFPSView();
	}

	// Cam::Camera::ProcessView 处理相机相应视角
	void Camera::ProcessView() {
		if (current_view == FPS_VIEW) {
			ProcessFPSView();
		} else if (current_view == TPS_VIEW) {
			ProcessTPSView();
		} else if (current_view == GOD_VIEW) {
			ProcessGODView();
		}
	}

	// Cam::Camera::ProcessTPSView 处理TPS相机视角
	void Camera::ProcessTPSView() {
		// 更新相机位置
		position = ModelRobot::move_position + vec3(0, ModelRobot::LOWER_LEG_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + 1.5 * ModelRobot::TORSO_HEIGHT, 0) + ModelRobot::move_direction * -3;
		// 更新相关向量
		updateCameraVectors();
	}

	// Cam::Camera::ProcessFPSView 处理FPS相机视角
	void Camera::ProcessFPSView() {
		position = ModelRobot::move_position + vec3(0, ModelRobot::LOWER_LEG_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + 1.1 * ModelRobot::TORSO_HEIGHT, 0) + ModelRobot::move_direction * 0.3;
		updateCameraVectors();
	}

	// Cam::Camera::ProcessGODView 处理FPS相机视角
	void Camera::ProcessGODView() {
		updateCameraVectors();
	}


	// Cam::Camera::ProcessRobotOverView 处理机器人死亡后的视角
	void Camera::ProcessRobotOverView() {
		position = vec3(0, 2 * ModelGround::block_scale, 0);	// 选取合适位置
		yaw = -90, pitch = 0;									// 选取适当角度
		updateCameraVectors();
	}

	// 初始化着色器
	void Camera::initShaderCamera() {
		GLuint program = InitShader("glsl/vshader.glsl", "glsl/fshader.glsl");
		glUseProgram(program);

		ViewMatrixID = glGetUniformLocation(program, "ViewMatrix");
		ProjectionMatrixID = glGetUniformLocation(program, "ProjectionMatrix");
	}

	// 设置视图矩阵与透视投影矩阵
	void Camera::displayShaderCamera() {
		glUniformMatrix4fv(ViewMatrixID, 1, GL_TRUE, getViewMatrix());
		glUniformMatrix4fv(ProjectionMatrixID, 1, GL_TRUE, perspective(30.0, 1.0, 0.1, 400.0));
	}

	// Cam::Camera::changeView 切换视角
	void Camera::changeView() {
		if (current_view == FPS_VIEW) {	// 若当前视角为FPS视角，则切换到TPS视角，下同理
			current_view = TPS_VIEW;
		} else if (current_view == TPS_VIEW) {
			current_view = GOD_VIEW;
			position = vec3(0, ModelRobot::UPPER_LEG_HEIGHT + ModelRobot::LOWER_LEG_HEIGHT + 2.5 * ModelRobot::TORSO_HEIGHT, 0) + ModelRobot::move_position;
		} else if (current_view == GOD_VIEW) {
			current_view = FPS_VIEW;
		}
		// 更新相关信息
		ProcessView();
	}

	// Camera::perspective 正交投影矩阵
	mat4 Camera::ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar) {
		return mat4(2.0 / (right - left), 0, 0, 0,
			0, 2.0 / (top - bottom), 0, 0,
			0, 0, -2.0 / (zFar - zNear), 0,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1);
	}

	// Camera::perspective 透视投影矩阵
	mat4 Camera::perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar) {
		const GLfloat top = zNear * tan(fovy * M_PI / 180);
		const GLfloat right = top * aspect;
		return mat4(zNear / right, 0, 0, 0,
			0, zNear / top, 0, 0,
			0, 0, -(zFar + zNear) / (zFar - zNear), -1,
			0, 0, -2.0 * zFar * zNear / (zFar - zNear), 0);
	}

	// Camera::lookAt 获取视图矩阵
	mat4 Camera::lookAt(const vec4& eye, const vec4& at, const vec4& up) {
		vec4 n = normalize(eye - at);
		vec4 u = normalize(cross(up, n));
		vec4 v = normalize(cross(n, u));
		mat4 t1 = mat4(1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			-eye[0], -eye[1], -eye[2], 1.0);
		mat4 t2 = mat4(u[0], v[0], n[0], 0,
			u[1], v[1], n[1], 0,
			u[2], v[2], n[2], 0,
			0, 0, 0, 1);
		return t2 * t1;
	}

	// Camera::getViewMatrix 构造视图矩阵
	mat4 Camera::getViewMatrix() {
		return lookAt(position, position + front, up);
	}

	// Camera::ProcessKeyboard 处理键盘按键带来的相机位置变化
	void Camera::ProcessKeyboard(Camera_Movement direction) {
		// 计算移动速度
		GLfloat velocity = program_speed * 0.7;
		// 向各个方向移动
		if (direction == FORWARD) {
			position += front * velocity;
		}
		else if (direction == BACKWARD) {
			position -= front * velocity;
		}
		else if (direction == LEFT) {
			position -= right * velocity;
		}
		else if (direction == RIGHT) {
			position += right * velocity;
		}
		ModelSun::reloadModelSun();
	}

	// Camera::ProcessMouseMovement 处理鼠标移动时相机视角的变化
	void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) {
		// 将鼠标移动的xoffset和yoffset乘上缩放比
		xoffset *= program_speed * 7, yoffset *= program_speed * 7;
		// 更新yaw, pitch角
		yaw += xoffset, pitch += yoffset;

		// 避免万向锁
		if (pitch > 89.0f)              pitch = 89.0f;
		else if (pitch < -89.0f)        pitch = -89.0f;

		// 更新相关向量
		updateCameraVectors();
	}

	// Camera::updateCameraVectors 更新相机相关向量
	void Camera::updateCameraVectors() {
		// 计算相机朝向向量 front
		GLfloat rad_yaw = radians(yaw), rad_pitch = radians(pitch);
		front = normalize(vec3(cos(rad_yaw) * cos(rad_pitch), sin(rad_pitch), sin(rad_yaw) * cos(rad_pitch)));

		// 计算相机右向向量和上向向量
		right = normalize(cross(front, vec3(0, 1.0, 0)));
		up = normalize(cross(right, front));
	}

	// Camera::radians 将角度值转弧度值
	GLfloat Camera::radians(GLfloat angle) {
		return angle / 180.0 * M_PI;
	}
}

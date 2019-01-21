#include "Core_Camera.h"
#include "Model.h"

namespace Cam {
	// Ĭ��Ϊ��һ�˳��ӽ�(FPS)
	Camera::Camera(vec3 position, vec3 up,
		GLfloat yaw, GLfloat pitch) : front(vec3(0.0f, 0.0f, -1.0f)),
		position(position), yaw(yaw), pitch(pitch) {
		updateCameraVectors();
		current_view = Camera_View::FPS_VIEW;
		ProcessFPSView();
	}

	// Cam::Camera::ProcessView ���������Ӧ�ӽ�
	void Camera::ProcessView() {
		if (current_view == FPS_VIEW) {
			ProcessFPSView();
		} else if (current_view == TPS_VIEW) {
			ProcessTPSView();
		} else if (current_view == GOD_VIEW) {
			ProcessGODView();
		}
	}

	// Cam::Camera::ProcessTPSView ����TPS����ӽ�
	void Camera::ProcessTPSView() {
		// �������λ��
		position = ModelRobot::move_position + vec3(0, ModelRobot::LOWER_LEG_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + 1.5 * ModelRobot::TORSO_HEIGHT, 0) + ModelRobot::move_direction * -3;
		// �����������
		updateCameraVectors();
	}

	// Cam::Camera::ProcessFPSView ����FPS����ӽ�
	void Camera::ProcessFPSView() {
		position = ModelRobot::move_position + vec3(0, ModelRobot::LOWER_LEG_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + 1.1 * ModelRobot::TORSO_HEIGHT, 0) + ModelRobot::move_direction * 0.3;
		updateCameraVectors();
	}

	// Cam::Camera::ProcessGODView ����FPS����ӽ�
	void Camera::ProcessGODView() {
		updateCameraVectors();
	}


	// Cam::Camera::ProcessRobotOverView �����������������ӽ�
	void Camera::ProcessRobotOverView() {
		position = vec3(0, 2 * ModelGround::block_scale, 0);	// ѡȡ����λ��
		yaw = -90, pitch = 0;									// ѡȡ�ʵ��Ƕ�
		updateCameraVectors();
	}

	// ��ʼ����ɫ��
	void Camera::initShaderCamera() {
		GLuint program = InitShader("glsl/vshader.glsl", "glsl/fshader.glsl");
		glUseProgram(program);

		ViewMatrixID = glGetUniformLocation(program, "ViewMatrix");
		ProjectionMatrixID = glGetUniformLocation(program, "ProjectionMatrix");
	}

	// ������ͼ������͸��ͶӰ����
	void Camera::displayShaderCamera() {
		glUniformMatrix4fv(ViewMatrixID, 1, GL_TRUE, getViewMatrix());
		glUniformMatrix4fv(ProjectionMatrixID, 1, GL_TRUE, perspective(30.0, 1.0, 0.1, 400.0));
	}

	// Cam::Camera::changeView �л��ӽ�
	void Camera::changeView() {
		if (current_view == FPS_VIEW) {	// ����ǰ�ӽ�ΪFPS�ӽǣ����л���TPS�ӽǣ���ͬ��
			current_view = TPS_VIEW;
		} else if (current_view == TPS_VIEW) {
			current_view = GOD_VIEW;
			position = vec3(0, ModelRobot::UPPER_LEG_HEIGHT + ModelRobot::LOWER_LEG_HEIGHT + 2.5 * ModelRobot::TORSO_HEIGHT, 0) + ModelRobot::move_position;
		} else if (current_view == GOD_VIEW) {
			current_view = FPS_VIEW;
		}
		// ���������Ϣ
		ProcessView();
	}

	// Camera::perspective ����ͶӰ����
	mat4 Camera::ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar) {
		return mat4(2.0 / (right - left), 0, 0, 0,
			0, 2.0 / (top - bottom), 0, 0,
			0, 0, -2.0 / (zFar - zNear), 0,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1);
	}

	// Camera::perspective ͸��ͶӰ����
	mat4 Camera::perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar) {
		const GLfloat top = zNear * tan(fovy * M_PI / 180);
		const GLfloat right = top * aspect;
		return mat4(zNear / right, 0, 0, 0,
			0, zNear / top, 0, 0,
			0, 0, -(zFar + zNear) / (zFar - zNear), -1,
			0, 0, -2.0 * zFar * zNear / (zFar - zNear), 0);
	}

	// Camera::lookAt ��ȡ��ͼ����
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

	// Camera::getViewMatrix ������ͼ����
	mat4 Camera::getViewMatrix() {
		return lookAt(position, position + front, up);
	}

	// Camera::ProcessKeyboard ������̰������������λ�ñ仯
	void Camera::ProcessKeyboard(Camera_Movement direction) {
		// �����ƶ��ٶ�
		GLfloat velocity = program_speed * 0.7;
		// ����������ƶ�
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

	// Camera::ProcessMouseMovement ��������ƶ�ʱ����ӽǵı仯
	void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) {
		// ������ƶ���xoffset��yoffset�������ű�
		xoffset *= program_speed * 7, yoffset *= program_speed * 7;
		// ����yaw, pitch��
		yaw += xoffset, pitch += yoffset;

		// ����������
		if (pitch > 89.0f)              pitch = 89.0f;
		else if (pitch < -89.0f)        pitch = -89.0f;

		// �����������
		updateCameraVectors();
	}

	// Camera::updateCameraVectors ��������������
	void Camera::updateCameraVectors() {
		// ��������������� front
		GLfloat rad_yaw = radians(yaw), rad_pitch = radians(pitch);
		front = normalize(vec3(cos(rad_yaw) * cos(rad_pitch), sin(rad_pitch), sin(rad_yaw) * cos(rad_pitch)));

		// �������������������������
		right = normalize(cross(front, vec3(0, 1.0, 0)));
		up = normalize(cross(right, front));
	}

	// Camera::radians ���Ƕ�ֵת����ֵ
	GLfloat Camera::radians(GLfloat angle) {
		return angle / 180.0 * M_PI;
	}
}

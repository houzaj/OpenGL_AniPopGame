// �������
#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include <Angel.h>
#include <vector>

extern double program_speed;

namespace Cam {
	// ����ӽ�
	enum Camera_View {
		FPS_VIEW, TPS_VIEW, GOD_VIEW
	};

	// ����ƶ�����
	enum Camera_Movement {
		FORWARD, BACKWARD, LEFT, RIGHT
	};

	// Default camera values
	const GLfloat YAW = -90.0f;
	const GLfloat PITCH = 0.0f;


	class Camera {
	public:
		// �����λ�á�ǰ��������������������������
		vec3 position, front, up, right;
		// ŷ����yaw��pitch
		GLfloat yaw, pitch;
		// ��ǰ�ӽ�
		Camera_View current_view;

		// Ĭ�Ϲ���
		Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);

		// �������ͶӰ����
		mat4 ortho(const GLfloat left, const GLfloat right,
			const GLfloat bottom, const GLfloat top,
			const GLfloat zNear, const GLfloat zFar);

		// ���͸��ͶӰ����
		mat4 perspective(const GLfloat fovy, const GLfloat aspect,
			const GLfloat zNear, const GLfloat zFar);
		
		// �����ͼ����
		mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up);

		// �����ڲ����������ͼ����
		mat4 getViewMatrix();

		// �������
		void ProcessKeyboard(Camera_Movement direction);

		// ��������ƶ�
		void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);

		// �����һ�˳�(FPS)�������˳�(TPS)���ϵ�(GOD) �ӽ�
		void ProcessView();
		void ProcessTPSView();
		void ProcessFPSView();
		void ProcessGODView();

		// ��������������ӽ�
		void ProcessRobotOverView();

		// ��ʼ���������ɫ��
		void initShaderCamera();

		// �����������ɫ����Ϣ
		void displayShaderCamera();

		void changeView();

	private:
		// �����������
		void updateCameraVectors();

		// �Ƕ�ֵת������
		GLfloat radians(GLfloat angle);

		// ��ͼ����ͶӰ��������ɫ���е�λ��
		GLint ViewMatrixID;
		GLint ProjectionMatrixID;
	};

}

#endif  // __CAMERA_CLASS_H__
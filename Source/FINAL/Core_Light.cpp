// ��Դ��
#include "Auxiliary_H.h"

namespace CoreLight {
	// ��������ϵ
	float angle_sita, angle_phi;
	// �뾶
	float radius;
	// ��Դλ��
	vec3 light_pos;
	// ��ɫ�����
	GLuint program;
	GLuint scaleID, lightPosID, modelMatrixID;

	// ��ʼ����ɫ����Դ
	void initShaderLight() {
		// ���ýǶȡ��뾶���ɴ����ù�Դλ��
		angle_sita = angle_phi = M_PI * 0.45;
		radius = 80;
		light_pos = radius * vec3(sin(angle_phi) * cos(angle_sita), sin(angle_phi) * sin(angle_sita), cos(angle_phi));

		// ��ʼ����ɫ��
		program = InitShader("glsl/vshader.glsl", "glsl/fshader.glsl");
		scaleID = glGetUniformLocation(program, "scale");
		lightPosID = glGetUniformLocation(program, "lightPos");
	}

	// �ı��Դλ�����ǿ����
	void displayShaderLight() {
		// ���ݹ�Դλ��
		glUniform3fv(lightPosID, 1, &light_pos[0]);
		// ���ݹ�ǿ��������
		glUniform1f(scaleID, 1.2);
	}

	// CoreLight::getShadowProjectionMatrix Ӳ��ӰͶӰ����
	mat4 getShadowProjectionMatrix() {
		return mat4(-light_pos[1], 0, 0, 0,
			light_pos[0], 0, light_pos[2], 1,
			0, 0, -light_pos[1], 0,
			0, 0, 0, -light_pos[1]);
	}
}

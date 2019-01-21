// 光源库
#include "Auxiliary_H.h"

namespace CoreLight {
	// 球面坐标系
	float angle_sita, angle_phi;
	// 半径
	float radius;
	// 光源位置
	vec3 light_pos;
	// 着色器相关
	GLuint program;
	GLuint scaleID, lightPosID, modelMatrixID;

	// 初始化着色器光源
	void initShaderLight() {
		// 设置角度、半径，由此设置光源位置
		angle_sita = angle_phi = M_PI * 0.45;
		radius = 80;
		light_pos = radius * vec3(sin(angle_phi) * cos(angle_sita), sin(angle_phi) * sin(angle_sita), cos(angle_phi));

		// 初始化着色器
		program = InitShader("glsl/vshader.glsl", "glsl/fshader.glsl");
		scaleID = glGetUniformLocation(program, "scale");
		lightPosID = glGetUniformLocation(program, "lightPos");
	}

	// 改变光源位置与光强因子
	void displayShaderLight() {
		// 传递光源位置
		glUniform3fv(lightPosID, 1, &light_pos[0]);
		// 传递光强缩放因子
		glUniform1f(scaleID, 1.2);
	}

	// CoreLight::getShadowProjectionMatrix 硬阴影投影矩阵
	mat4 getShadowProjectionMatrix() {
		return mat4(-light_pos[1], 0, 0, 0,
			light_pos[0], 0, light_pos[2], 1,
			0, 0, -light_pos[1], 0,
			0, 0, 0, -light_pos[1]);
	}
}

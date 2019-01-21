#ifndef __CORE_LIGHT_H__
#define __CORE_LIGHT_H__

#include "include/Angel.h"
#include "include/TriMesh.h"

#pragma comment(lib, "glew32.lib")

namespace CoreLight {
	extern vec3 light_pos;

	mat4 getShadowProjectionMatrix();

	void initShaderLight();
	void displayShaderLight();
}

#endif // !__CORE_LIGHT_H__
#ifndef __AUXILIARY_H__
#define __AUXILIARY_H__

#include "Angel.h"
#include "FreeImage.h"
#include "TriMesh.h"

#include "Core_MouseKeyboard.h"
#include "Core_Camera.h"
#include "Core_Light.h"
#include "Core_Mesh.h"
#include "Core_ImageTexture.h"

#include "Model.h"

#include "Auxiliary_GenShape.h"
#include "Auxiliary_Func.h"

#include <vector>
#include <stack>
#include <algorithm>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")


extern const int WIN_X, WIN_Y;
extern Cam::Camera cam;
extern vec3 CoreLight::light_pos;

extern double program_speed;

#endif // !__AUXILIARY_H__

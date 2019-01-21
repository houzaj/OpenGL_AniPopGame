// 太阳模型
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelSun {
    std::vector<vec3> ls_v;
    std::vector<vec3> ls_n;
    std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	const float MODEL_SUN_SCALE = 6.0;		// 缩放系数

    int pos_begin;

	// 加载UV坐标
	void loadModelSunTexCoord(int k) {
		ls_vt.clear();
		list_vertex v[2];
		for (int i = 0, dir = 1; i < 2; i++, dir *= -1) {
			double delta = 2 * M_PI / k;
			double angle = 0;
			for (int j = 0; j < k; j++) {
				ls_vt.push_back(vec2(0.5 * cos(angle) + 0.5, 0.5 * sin(angle) + 0.5));
				angle += delta;
			}
		}

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < k; j++) {
				ls_vt.push_back(vec2(0, 0));
			}
		}
	}

	// 加载太阳模型
    void loadModelSun() {
        AuxiliaryGenShape::genCylinder(ls_v, ls_n, ls_fs, 100);
		loadModelSunTexCoord(100);

        pos_begin = CoreMesh::painter.getSize();

		reloadModelSun();
    }
	
	// 重加载太阳模型
	void reloadModelSun() {
		// 正则化光线位置向量
		vec3 v = normalize(CoreLight::light_pos);
		// 根据光线位置向量计算旋转角度，确定旋转模型矩阵
		mat4 model_matrix_rotation = RotateZ(acos(v.x) / M_PI * 180 - 90) * RotateX(-acos(v.z) / M_PI * 180 + 90);

		mat4 model_matrix = Translate(cam.position) * 
			Translate(CoreLight::light_pos * 1.1) * 
			model_matrix_rotation * 
			Scale(MODEL_SUN_SCALE, 0.01, MODEL_SUN_SCALE);
		if (pos_begin == CoreMesh::painter.getSize()) {
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh();
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix, vec4(1.0, 1.0, 1.0, 1.0));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_SUN, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_begin, false, false);
		} else {
			CoreMesh::painter.getMeshAt(pos_begin)->model_matrix = model_matrix;
		}
		
	}
}

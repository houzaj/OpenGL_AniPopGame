// 地面
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelGround {
	// 白色
	const vec4 WHITE = vec4(1.0, 1.0, 1.0, 1.0);

	// 图形相关信息
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec2> ls_vt;
	std::vector<vec3i> ls_fs;

	float block_scale = 6.0;		// 方块缩放值

	// 加载UV坐标
	void loadModelGroundTexCoord() {
		ls_vt.clear();
		int n = ls_v.size();
		for (int i = 0; i < n; i += 4) {
			ls_vt.push_back(vec2(0, 1));
			ls_vt.push_back(vec2(1, 1));
			ls_vt.push_back(vec2(1, 0));
			ls_vt.push_back(vec2(0, 0));
		}
	}

	// 加载模型
	void loadModelGound() {
		// 生成单位立方体
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelGroundTexCoord();
		// 缩放系数
		block_scale = 6.0;
		
		CoreMesh::Mesh* mesh;
		mat4 model_matrix;
		// 生成5x5的地面
		for (int x = -5; x <= 5; x++) {
			for (int z = -5; z <= 5; z++) {
				mesh = new CoreMesh::Mesh();
				// 先缩放block_scale倍，再平移
				model_matrix = Translate(block_scale * x, -0.501, block_scale * z) * Scale(block_scale, 1, block_scale);
				vec4 color = WHITE;

				mesh->setData(ls_v, ls_n, ls_fs, model_matrix, color);
				mesh->setTexture(CoreImageTexture::TextureType::MODEL_GROUND, ls_vt);
				CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false);
			}
		}

		// 生成10x10的外围地面
		for (int x = -10; x <= 10; x++) {
			for (int z = -10; z <= 10; z++) {
				if (-5 <= x && x <= 5 && -5 <= z && z <= 5) {
					continue;
				}

				mesh = new CoreMesh::Mesh();
				model_matrix = Translate(block_scale * x, -0.501, block_scale * z) * Scale(block_scale, 1, block_scale);
				vec4 color = WHITE;

				mesh->setData(ls_v, ls_n, ls_fs, model_matrix, color);
				mesh->setTexture(CoreImageTexture::TextureType::MODEL_GROUND_EDGE, ls_vt);
				CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false);
			}
		}
	}
}
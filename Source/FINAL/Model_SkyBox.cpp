// 天空盒子
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelSkyBox {
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	// 缩放比
	const int SKYBOX_SCALE = 300;

	// 加载VT坐标
	void loadModelSkyBoxTexCoord() {
		ls_vt.clear();

		ls_vt.push_back(vec2(0, 1));
		ls_vt.push_back(vec2(1, 1));
		ls_vt.push_back(vec2(1, 0));
		ls_vt.push_back(vec2(0, 0));
	}

	// 加载天空盒子
	void loadModelSkyBox() {
		AuxiliaryGenShape::genRectangle(ls_v, ls_n, ls_fs);
		loadModelSkyBoxTexCoord();

		// 对其进行缩放
		mat4 model_matrix = Scale(SKYBOX_SCALE, 0, SKYBOX_SCALE);
		
		CoreMesh::Mesh* mesh;

		// 开始绘制
		// UP 上面
		mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(0, 0.5 * SKYBOX_SCALE, 0) * RotateY(180) * RotateX(180) * model_matrix, vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::SKY_UP, ls_vt);
		CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false, false, true);

		// DOWN 下面
		mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(0, -0.5 * SKYBOX_SCALE, 0) * model_matrix, vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::SKY_DOWN, ls_vt);
		CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false, false, true);

		// LEFT 左面
		mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(-0.5 * SKYBOX_SCALE, 0, 0) * RotateY(90) * RotateX(90) * model_matrix, vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::SKY_LEFT, ls_vt);
		CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false, false, true);

		// RIGHT 右面
		mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(0.5 * SKYBOX_SCALE, 0, 0) * RotateY(-90) * RotateX(90) * model_matrix, vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::SKY_RIGHT, ls_vt);
		CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false, false, true);

		// FRONT 前面
		mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(0, 0, 0.5 * SKYBOX_SCALE) * RotateY(180) * RotateX(90) * model_matrix, vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::SKY_FORWARD, ls_vt);
		CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false, false, true);

		// BACK 后面
		mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(0, 0, -0.5 * SKYBOX_SCALE) * RotateX(90) * model_matrix, vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::SKY_BACK, ls_vt);
		CoreMesh::painter.addMesh(mesh, CoreMesh::painter.getSize(), false, false, true);
	}
}
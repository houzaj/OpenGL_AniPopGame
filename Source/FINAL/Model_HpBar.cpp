// 血量条
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelHpBar {
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;

	const float HP_BAR_LENGTH_TOTAL = 0.8;		// 总长度
	const float HP_BAR_WIDTH = 0.05;			// 宽度

	const vec3 RED = vec3(194 / 128.0, 54 / 128.0, 22 / 128.0);		// 红色

	float hp_bar_length = HP_BAR_LENGTH_TOTAL;	// 实际长度

	int pos_begin;

	// 加载血量条
	void loadModelHpBar() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);

		pos_begin = CoreMesh::painter.getSize();

		reloadModelHpBar();
	}

	// 重加载血量条
	void reloadModelHpBar() {
		CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
		mat4 model_matrix = Translate(0, ModelRobot::LOWER_LEG_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + ModelRobot::TORSO_HEIGHT * 1.8, 0) * 
			Translate(ModelRobot::move_position) *	// 随机器人移动而移动
			RotateY(ModelRobot::view_yaw) *			// 随机器人方向而转动
			Scale(std::max(hp_bar_length * ModelRobot::robot_hp, 0.0f), HP_BAR_WIDTH, HP_BAR_WIDTH);
		mesh->setData(ls_v, ls_n, ls_fs, model_matrix, RED);
		CoreMesh::painter.addMesh(mesh, pos_begin + 0, false, true, true);
	}

}
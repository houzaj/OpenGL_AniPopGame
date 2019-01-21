// Ѫ����
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelHpBar {
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;

	const float HP_BAR_LENGTH_TOTAL = 0.8;		// �ܳ���
	const float HP_BAR_WIDTH = 0.05;			// ���

	const vec3 RED = vec3(194 / 128.0, 54 / 128.0, 22 / 128.0);		// ��ɫ

	float hp_bar_length = HP_BAR_LENGTH_TOTAL;	// ʵ�ʳ���

	int pos_begin;

	// ����Ѫ����
	void loadModelHpBar() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);

		pos_begin = CoreMesh::painter.getSize();

		reloadModelHpBar();
	}

	// �ؼ���Ѫ����
	void reloadModelHpBar() {
		CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
		mat4 model_matrix = Translate(0, ModelRobot::LOWER_LEG_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + ModelRobot::TORSO_HEIGHT * 1.8, 0) * 
			Translate(ModelRobot::move_position) *	// ��������ƶ����ƶ�
			RotateY(ModelRobot::view_yaw) *			// ������˷����ת��
			Scale(std::max(hp_bar_length * ModelRobot::robot_hp, 0.0f), HP_BAR_WIDTH, HP_BAR_WIDTH);
		mesh->setData(ls_v, ls_n, ls_fs, model_matrix, RED);
		CoreMesh::painter.addMesh(mesh, pos_begin + 0, false, true, true);
	}

}
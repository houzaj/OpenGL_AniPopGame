// ����
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelBomb {
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	bool is_painted = false;
	int pos_begin;
	mat4 bomb_model_matrix = mat4(1.0);

	vec3  bomb_direction;								// �����ӳ�����
	float bomb_scale = 0.2;								// �������Ŵ�С
	float bomb_speed_y = 1;								// ����y���ʼ�ٶ�
	float bomb_speed_xz = 5 * bomb_speed_y;				// ����xz���ʼ�ٶ�
	float bomb_away_time = 0;							// ����ʱ��
	int   bomb_nums = 2;								// ���׳�ʼ����
	const int BOMB_NUMS_SCORE_DELTA = 100;				// ������������Ļ�������
	int   pre_bombs_nums_score = BOMB_NUMS_SCORE_DELTA;	// ������������Ļ���
	bool  is_bomb_away = false;							// �����Ƿ����ڷ��еı�Ǳ���

	// ����UV����
	void loadModelBombTexCoord() {
		ls_vt.clear();
		int n = ls_v.size();
		for (int i = 0; i < n; i += 4) {
			ls_vt.push_back(vec2(0, 1));
			ls_vt.push_back(vec2(1, 1));
			ls_vt.push_back(vec2(1, 0));
			ls_vt.push_back(vec2(0, 0));
		}
	}

	// ����ģ��
	void loadModelBomb() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelBombTexCoord();
		pos_begin = CoreMesh::painter.getSize();
		reloadModelBomb(bomb_model_matrix);
	}

	// ����ģ�;����ؼ���ģ��
	void reloadModelBomb(const mat4& model_matrix) {
		// �������ģ�;����������ַ��ͬ���򲻸���
		if (&bomb_model_matrix != &model_matrix) {
			bomb_model_matrix = model_matrix;
		}
		if (pos_begin == CoreMesh::painter.getSize()) {
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, bomb_model_matrix * Translate(0, 0.5 * bomb_scale, 0) * Scale(bomb_scale, bomb_scale, bomb_scale), vec3(0, 0, 0));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_BOMB, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_begin, false, false, is_painted);
		} else {
			CoreMesh::painter.getMeshAt(pos_begin)->model_matrix = bomb_model_matrix * Translate(0, 0.5 * bomb_scale, 0) * Scale(bomb_scale, bomb_scale, bomb_scale);
			// �ı��Ƿ���Ʊ�Ǳ���
			CoreMesh::painter.getMeshAt(pos_begin)->is_painted = is_painted;
		}
		
	}

	// ��ȡ��ǰ������������ײ��
	void getPointIsCracked(vec3& out_point, bool& ok) {
		// ԭʼλ��
		vec4 bomb_position[8] = {
			vec4(-0.5, -0.5, -0.5, 1),
			vec4(-0.5, -0.5, 0.5, 1),
			vec4(-0.5, 0.5, -0.5, 1),
			vec4(-0.5, 0.5, 0.5, 1),
			vec4(0.5, -0.5, -0.5, 1),
			vec4(0.5, -0.5, 0.5, 1),
			vec4(0.5, 0.5, -0.5, 1),
			vec4(0.5, 0.5, 0.5, 1)
		};

		for (int i = 0; i < 8; i++) {
			// ��ǰλ��
			vec4 point = bomb_model_matrix * bomb_position[i];
			vec3 point_vec3 = vec3(point.x, point.y, point.z);
			// �ж��Ƿ��Ѿ��������
			if (point.y <= 0.0) {
				out_point = point_vec3;
				ok = true;
				return;
			}
		}
		ok = false;
	}

	// �������׿�ʼ����
	void setBombAway() {
		// �����ӳ�����
		bomb_direction = normalize(cam.front + vec3(0, 0.1 * ModelGameBlock::game_block_scale, 0));
		// ��Ǳ�����Ϊ��
		is_bomb_away = true;
	}

	// ModelBomb::ProcessBombAway �������׷��ж�������غ����Ϸ��������
	void ProcessBombAway() {
		// �ж���Ǳ����Ƿ���ȻΪ�棬��Ϊ���򲻼�������
		if (!is_bomb_away) {
			return;
		}

		// ��ȡ��������ײ��
		vec3 point;
		bool ok;
		getPointIsCracked(point, ok);
		// ���õ㲻����
		if (!ok) {
			// �����˶�����
			float delta_x = program_speed * bomb_speed_xz * bomb_direction.x;
			float delta_y = program_speed * bomb_speed_y * bomb_direction.y - 0.5 * 9.8 * 60 * bomb_away_time * bomb_away_time;
			float delta_z = program_speed * bomb_speed_xz * bomb_direction.z;
			// �ı�ģ�;���ʹ���˶�
			bomb_model_matrix = Translate(delta_x, delta_y, delta_z) * bomb_model_matrix;
			bomb_away_time += 0.008 * program_speed;
		} else {
			// ��غ�ѡȡһ����Χ�ڵķ���ʹ������
			for (int x = -4; x <= 4; x++) {
				for (int z = -4; z <= 4; z++) {
					if (AuxiliaryFunc::sqr(point.x - x * ModelGround::block_scale) +
						AuxiliaryFunc::sqr(point.z - z * ModelGround::block_scale) <= 
						AuxiliaryFunc::sqr(2 * ModelGround::block_scale)) {
						ModelGameBlock::game_block_removed_ls.push_back(ModelGameBlock::getIndex(x, z));
					}
				}
			}
			ModelGameBlock::is_game_block_removed = true;

			// ��������
			ModelRobot::score += ModelGameBlock::game_block_removed_ls.size();
			// �Ƿְ��������
			ModelBoard::ProcessScoreUpdate();
			ModelBomb::updateBombNums();
			// ����
			bomb_away_time = 0;
			is_bomb_away = false;
			is_painted = false;
			// ����������1
			bomb_nums--;
		}
		// �ؼ���ģ��
		reloadModelBomb(bomb_model_matrix);
	}

	// ������������
	void updateBombNums() {
		// ������������������������֣��������ף�����ߴ˻���
		if (ModelRobot::score >= pre_bombs_nums_score) {
			bomb_nums++;
			pre_bombs_nums_score += BOMB_NUMS_SCORE_DELTA;
		}
	}

	// ��ʼ������ģ����ر���
	void initModelBomb() {
		is_painted = false;
		bomb_nums = 2;
		pre_bombs_nums_score = BOMB_NUMS_SCORE_DELTA;
		is_bomb_away = false;
	}
}

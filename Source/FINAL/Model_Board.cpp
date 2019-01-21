// �Ƿְ�
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelBoard {
	// �����ʾ���ֵ�������
	// ����ֵ
	const float BLOCK_BOARD_SCALE = 0.5;
	// ֮��ľ�������ֵ
	const float BLOCK_BOARD_GAP_SCALE = BLOCK_BOARD_SCALE * 0.7;
	// ���

	// �����ʾ���ֵ���λ
	// ���
	const float BLOCK_BOARD_WIDTH = 0.01;
	// ÿ���x������
	const int BLOCK_BOARD_NUMS_GROUP_X = 3;
	// ÿ���y������
	const int BLOCK_BOARD_NUMS_GROUP_Y = 5;
	// ÿ������
	const int BLOCK_BOARD_NUMS_GROUP = 4;
	// ÿ���x�᳤��
	const float BLOCK_BOARD_GROUP_X = BLOCK_BOARD_NUMS_GROUP_X * (BLOCK_BOARD_SCALE + BLOCK_BOARD_GAP_SCALE);
	
	// �Ƿְ�
	// �Ƿְ�x�᳤��
	const float BOARD_X = ModelGround::block_scale * (5 + 5 + 1);
	// �Ƿְ�y�᳤��
	const float BOARD_Y = ModelGround::block_scale * (1 + 1 + 1);
	// �Ƿְ���
	const float BOARD_WIDTH = ModelGround::block_scale;

	// ����λ��
	const int SCORE_DIGIT_NUMS = 4;

	// ��λ״̬
	const bool DIGIT_DISPLAY[10][3 * 5] = {
		{ 1, 1, 1,   1, 0, 1,   1, 0, 1,   1, 0, 1,   1, 1, 1 },	// 0
		{ 0, 1, 0,   1, 1, 0,   0, 1, 0,   0, 1, 0,   1, 1, 1 },	// 1
		{ 1, 1, 1,   0, 0, 1,   1, 1, 1,   1, 0, 0,   1, 1, 1 },	// 2
		{ 1, 1, 1,   0, 0, 1,   1, 1, 1,   0, 0, 1,   1, 1, 1 },	// 3
		{ 1, 0, 1,   1, 0, 1,   1, 1, 1,   0, 0, 1,   0, 0, 1 },	// 4
		{ 1, 1, 1,   1, 0, 0,   1, 1, 1,   0, 0, 1,   1, 1, 1 },	// 5
		{ 1, 1, 1,   1, 0, 0,   1, 1, 1,   1, 0, 1,   1, 1, 1 },	// 6
		{ 1, 1, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1 },	// 7
		{ 1, 1, 1,   1, 0, 1,   1, 1, 1,   1, 0, 1,   1, 1, 1 },	// 8
		{ 1, 1, 1,   1, 0, 1,   1, 1, 1,   0, 0, 1,   1, 1, 1 }		// 9
	};

	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	int pos_begin, pos_iterate;

	// ������λ
	int score_digit[SCORE_DIGIT_NUMS] = { 0 };
	// ��ʾ״̬
	bool block_board_state[BLOCK_BOARD_NUMS_GROUP_X * BLOCK_BOARD_NUMS_GROUP_Y * BLOCK_BOARD_NUMS_GROUP];
	// ��ǰ��ģ�;���
	mat4 model_matrix;

	// ����UV����
	void loadModelBoardTexCoord() {
		float x = 1.0;
		float y = x / 3;
		float z = x / 11;
		ls_vt.clear();

		ls_vt.push_back(vec2(0, y)); ls_vt.push_back(vec2(z, y));
		ls_vt.push_back(vec2(z, 0)); ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, z)); ls_vt.push_back(vec2(x, z));
		ls_vt.push_back(vec2(x, 0)); ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, y)); ls_vt.push_back(vec2(z, y));
		ls_vt.push_back(vec2(z, 0)); ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, z)); ls_vt.push_back(vec2(x, z));
		ls_vt.push_back(vec2(x, 0)); ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, y)); ls_vt.push_back(vec2(x, y));
		ls_vt.push_back(vec2(x, 0)); ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, y)); ls_vt.push_back(vec2(x, y));
		ls_vt.push_back(vec2(x, 0)); ls_vt.push_back(vec2(0, 0));
	}

	// ���ؼǷְ�
	void loadModelBoard() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelBoardTexCoord();

		pos_begin = CoreMesh::painter.getSize();

		CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
		model_matrix = Translate(0, BOARD_Y * 0.5, -6 * ModelGround::block_scale);
		mesh->setData(ls_v, ls_n, ls_fs, model_matrix * Scale(BOARD_X, BOARD_Y, BOARD_WIDTH), vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::MODEL_SCORE_BOARD, ls_vt);
		CoreMesh::painter.addMesh(mesh, pos_begin);

		// ���ڼǷְ岻��Ҫ�ؼ��أ���˽�pos_begin�����ڼ���������ʾ�����������εĵ�һ��λ��
		pos_begin = pos_iterate = CoreMesh::painter.getSize();
		// ��������
		ProcessScoreUpdate();
	}

	// ModelBoard::reloadModelBoard ���¼��ؼǷְ�ģ��
	void reloadModelBoard() {
		pos_iterate = pos_begin;

		int block_bloard_pos = 0;

		// ��������ÿһ��������
		for (int i = 0; i < BLOCK_BOARD_NUMS_GROUP; i++) {
			for (int y = BLOCK_BOARD_NUMS_GROUP_Y / 2; y >= -BLOCK_BOARD_NUMS_GROUP_Y / 2; y--) {
				for (int x = -BLOCK_BOARD_NUMS_GROUP_X / 2; x <= BLOCK_BOARD_NUMS_GROUP_X / 2; x++) {
					mat4 tmp_matrix = 
						Translate(x * (BLOCK_BOARD_SCALE + BLOCK_BOARD_GAP_SCALE), y * (BLOCK_BOARD_SCALE + BLOCK_BOARD_GAP_SCALE), BOARD_WIDTH * 0.5) *
						Translate((i - BLOCK_BOARD_NUMS_GROUP / 2) * (BLOCK_BOARD_GROUP_X + BLOCK_BOARD_GAP_SCALE) + BLOCK_BOARD_GAP_SCALE * 0.5 + BLOCK_BOARD_GROUP_X / 2, 0, 0) * 
						model_matrix *
						Scale(BLOCK_BOARD_SCALE, BLOCK_BOARD_SCALE, BLOCK_BOARD_WIDTH);
					CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
					mesh->setData(ls_v, ls_n, ls_fs, tmp_matrix, vec3(1, 1, 1));
					// ������ʾ״̬ѡ���Ƿ����
					CoreMesh::painter.addMesh(mesh, pos_iterate++, false, false, block_board_state[block_bloard_pos++]);
					
				}
			}
		}
	}

	// �����������
	void ProcessScoreUpdate() {
		// ��λ���
		for (int i = SCORE_DIGIT_NUMS - 1, score_num = ModelRobot::score; i >= 0; i--) {
			score_digit[i] = score_num % 10;
			score_num /= 10;
		}

		// ������λ�޸���ʾ״̬
		int block_bloard_pos = 0;
		for (int i = 0; i < SCORE_DIGIT_NUMS; i++) {
			const bool* digit_display_ptr = DIGIT_DISPLAY[score_digit[i]];
			for (int j = 0; j < BLOCK_BOARD_NUMS_GROUP_X * BLOCK_BOARD_NUMS_GROUP_Y; j++) {
				block_board_state[block_bloard_pos++] = digit_display_ptr[j];
			}
		}

		// ���ؼ��ؼǷְ�ģ��
		reloadModelBoard();
	}
}

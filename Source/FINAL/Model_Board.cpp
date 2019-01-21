// 记分板
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelBoard {
	// 组成显示数字的正方形
	// 缩放值
	const float BLOCK_BOARD_SCALE = 0.5;
	// 之间的距离缩放值
	const float BLOCK_BOARD_GAP_SCALE = BLOCK_BOARD_SCALE * 0.7;
	// 厚度

	// 组成显示数字的数位
	// 厚度
	const float BLOCK_BOARD_WIDTH = 0.01;
	// 每组的x轴数量
	const int BLOCK_BOARD_NUMS_GROUP_X = 3;
	// 每组的y轴数量
	const int BLOCK_BOARD_NUMS_GROUP_Y = 5;
	// 每组组数
	const int BLOCK_BOARD_NUMS_GROUP = 4;
	// 每组的x轴长度
	const float BLOCK_BOARD_GROUP_X = BLOCK_BOARD_NUMS_GROUP_X * (BLOCK_BOARD_SCALE + BLOCK_BOARD_GAP_SCALE);
	
	// 记分板
	// 记分板x轴长度
	const float BOARD_X = ModelGround::block_scale * (5 + 5 + 1);
	// 记分板y轴长度
	const float BOARD_Y = ModelGround::block_scale * (1 + 1 + 1);
	// 记分板宽度
	const float BOARD_WIDTH = ModelGround::block_scale;

	// 数的位数
	const int SCORE_DIGIT_NUMS = 4;

	// 数位状态
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

	// 分数数位
	int score_digit[SCORE_DIGIT_NUMS] = { 0 };
	// 显示状态
	bool block_board_state[BLOCK_BOARD_NUMS_GROUP_X * BLOCK_BOARD_NUMS_GROUP_Y * BLOCK_BOARD_NUMS_GROUP];
	// 当前的模型矩阵
	mat4 model_matrix;

	// 加载UV坐标
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

	// 加载记分板
	void loadModelBoard() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelBoardTexCoord();

		pos_begin = CoreMesh::painter.getSize();

		CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
		model_matrix = Translate(0, BOARD_Y * 0.5, -6 * ModelGround::block_scale);
		mesh->setData(ls_v, ls_n, ls_fs, model_matrix * Scale(BOARD_X, BOARD_Y, BOARD_WIDTH), vec3(1, 1, 1));
		mesh->setTexture(CoreImageTexture::TextureType::MODEL_SCORE_BOARD, ls_vt);
		CoreMesh::painter.addMesh(mesh, pos_begin);

		// 由于记分板不需要重加载，因此将pos_begin设置在即将加载显示分数的正方形的第一个位置
		pos_begin = pos_iterate = CoreMesh::painter.getSize();
		// 分数更新
		ProcessScoreUpdate();
	}

	// ModelBoard::reloadModelBoard 重新加载记分板模型
	void reloadModelBoard() {
		pos_iterate = pos_begin;

		int block_bloard_pos = 0;

		// 迭代处理每一个正方形
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
					// 根据显示状态选择是否绘制
					CoreMesh::painter.addMesh(mesh, pos_iterate++, false, false, block_board_state[block_bloard_pos++]);
					
				}
			}
		}
	}

	// 处理分数更新
	void ProcessScoreUpdate() {
		// 数位拆分
		for (int i = SCORE_DIGIT_NUMS - 1, score_num = ModelRobot::score; i >= 0; i--) {
			score_digit[i] = score_num % 10;
			score_num /= 10;
		}

		// 根据数位修改显示状态
		int block_bloard_pos = 0;
		for (int i = 0; i < SCORE_DIGIT_NUMS; i++) {
			const bool* digit_display_ptr = DIGIT_DISPLAY[score_digit[i]];
			for (int j = 0; j < BLOCK_BOARD_NUMS_GROUP_X * BLOCK_BOARD_NUMS_GROUP_Y; j++) {
				block_board_state[block_bloard_pos++] = digit_display_ptr[j];
			}
		}

		// 重载加载记分板模型
		reloadModelBoard();
	}
}

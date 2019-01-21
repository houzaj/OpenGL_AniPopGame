// 手雷
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

	vec3  bomb_direction;								// 手雷扔出方向
	float bomb_scale = 0.2;								// 手雷缩放大小
	float bomb_speed_y = 1;								// 手雷y轴初始速度
	float bomb_speed_xz = 5 * bomb_speed_y;				// 手雷xz轴初始速度
	float bomb_away_time = 0;							// 动画时间
	int   bomb_nums = 2;								// 手雷初始数量
	const int BOMB_NUMS_SCORE_DELTA = 100;				// 奖励手雷所需的积分增量
	int   pre_bombs_nums_score = BOMB_NUMS_SCORE_DELTA;	// 奖励手雷所需的积分
	bool  is_bomb_away = false;							// 手雷是否正在飞行的标记变量

	// 加载UV坐标
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

	// 加载模型
	void loadModelBomb() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelBombTexCoord();
		pos_begin = CoreMesh::painter.getSize();
		reloadModelBomb(bomb_model_matrix);
	}

	// 根据模型矩阵重加载模型
	void reloadModelBomb(const mat4& model_matrix) {
		// 若传入的模型矩阵与自身地址相同，则不复制
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
			// 改变是否绘制标记变量
			CoreMesh::painter.getMeshAt(pos_begin)->is_painted = is_painted;
		}
		
	}

	// 获取当前手雷与地面的碰撞点
	void getPointIsCracked(vec3& out_point, bool& ok) {
		// 原始位置
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
			// 当前位置
			vec4 point = bomb_model_matrix * bomb_position[i];
			vec3 point_vec3 = vec3(point.x, point.y, point.z);
			// 判定是否已经到达地面
			if (point.y <= 0.0) {
				out_point = point_vec3;
				ok = true;
				return;
			}
		}
		ok = false;
	}

	// 设置手雷开始飞行
	void setBombAway() {
		// 计算扔出方向
		bomb_direction = normalize(cam.front + vec3(0, 0.1 * ModelGameBlock::game_block_scale, 0));
		// 标记变量置为真
		is_bomb_away = true;
	}

	// ModelBomb::ProcessBombAway 处理手雷飞行动画与落地后的游戏方块消除
	void ProcessBombAway() {
		// 判定标记变量是否仍然为真，不为真则不继续处理
		if (!is_bomb_away) {
			return;
		}

		// 获取与地面的碰撞点
		vec3 point;
		bool ok;
		getPointIsCracked(point, ok);
		// 若该点不存在
		if (!ok) {
			// 计算运动分量
			float delta_x = program_speed * bomb_speed_xz * bomb_direction.x;
			float delta_y = program_speed * bomb_speed_y * bomb_direction.y - 0.5 * 9.8 * 60 * bomb_away_time * bomb_away_time;
			float delta_z = program_speed * bomb_speed_xz * bomb_direction.z;
			// 改变模型矩阵使其运动
			bomb_model_matrix = Translate(delta_x, delta_y, delta_z) * bomb_model_matrix;
			bomb_away_time += 0.008 * program_speed;
		} else {
			// 落地后选取一定范围内的方块使其消除
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

			// 分数增加
			ModelRobot::score += ModelGameBlock::game_block_removed_ls.size();
			// 记分板分数增加
			ModelBoard::ProcessScoreUpdate();
			ModelBomb::updateBombNums();
			// 重置
			bomb_away_time = 0;
			is_bomb_away = false;
			is_painted = false;
			// 手雷数量减1
			bomb_nums--;
		}
		// 重加载模型
		reloadModelBomb(bomb_model_matrix);
	}

	// 更新手雷数量
	void updateBombNums() {
		// 若分数超过奖励手雷所需积分，则奖励手雷，并提高此积分
		if (ModelRobot::score >= pre_bombs_nums_score) {
			bomb_nums++;
			pre_bombs_nums_score += BOMB_NUMS_SCORE_DELTA;
		}
	}

	// 初始化手雷模型相关变量
	void initModelBomb() {
		is_painted = false;
		bomb_nums = 2;
		pre_bombs_nums_score = BOMB_NUMS_SCORE_DELTA;
		is_bomb_away = false;
	}
}

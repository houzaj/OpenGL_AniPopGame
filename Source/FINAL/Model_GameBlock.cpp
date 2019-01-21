// 游戏方块
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelGameBlock {

	float game_block_scale = 2;						// 游戏方块缩放值
	float game_block_offset_y = 0.1;				// 游戏方块在y轴的偏移分量

	/* 游戏方块指向相关 */
	int	game_block_pointed_index = -1;

	/* 游戏方块移除相关 */
	int game_block_removed_step = 1;				// 移除动画步骤
	float game_block_removed_time = 0;				// 移除动画时间
	bool is_game_block_removed = false;				// 标记是否有游戏方块被移除
	std::vector<int> game_block_removed_ls;			// 记录需要被移除的游戏方块的下标
	const float GAME_BLOCK_REMOVED_MAX_TIME = 50;	// 移除动画的最大限时

	/* 游戏方块交换相关 */
	int game_block_swaped_index[2] = { -1, -1 };	// 交换的两个方块的下标
	int game_block_swaped_direction_x = 0;			// 交换的两个方块是否为x轴
	int game_block_swaped_direction_z = 0;			// 交换的两个方块是否为z轴
	float game_block_swaped_time = 0;				// 交换的两个方块动画时间
	bool is_game_block_swaped = false;				// 标记是否正在进行交换
	const float GAME_BLOCK_SWAPED_MAX_TIME = 40;	// 交换动画的最大限时

	/* 游戏方块选择相关 */
	int game_block_selected_index = -1;				// 被选择的游戏方块下标
	float game_block_selected_time = 0;				// 当前颜色变化动画时间
	bool  is_game_block_selected = false;			// 标记是否有方块被选择

	
	const float PUSH_ROBOT_SPEED = 30.0;			// 推开机器人的速度

	const int NUMS_GAME_BLOCKS_TYPE = 5;			// 游戏方块类型数量
	const int NUMS_GAME_BLOCKS = 81;				// 游戏方块总数

	int  game_block_type[NUMS_GAME_BLOCKS];			// 游戏方块类型
	mat4 game_block_model_matrix[NUMS_GAME_BLOCKS];	// 游戏方块模型矩阵

	// 方块类型对应的纹理类型
	CoreImageTexture::TextureType game_block_tex_type[NUMS_GAME_BLOCKS_TYPE] = {
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_YELLOW,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_RED,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_GREEN,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_PURPLE,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_PINK
	};

	int pos_begin;						// 记录STL向量中游戏方块中第一个图形的位置

	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	// 由(x,z)坐标映射到数组下标index
	int getIndex(int x, int z) {
		x += 4, z += 4;
		return z * 9 + x;
	}

	// 由index获取(x.z)坐标
	void getXZ(int index, int& x, int& z) {
		x = index % 9 - 4;
		z = index / 9 - 4;
	}

	// 加载UV坐标
	void loadModelGameBlockTexCoord() {
		ls_vt.clear();
		int n = ls_v.size();
		for (int i = 0; i < n; i += 3) {
			ls_vt.push_back(vec2(0.5, 1));
			ls_vt.push_back(vec2(0, 0));
			ls_vt.push_back(vec2(1, 0));
		}
	}

	// 加载游戏方块
	void loadModelGameBlock() {
		AuxiliaryGenShape::genRectangularPyramid(ls_v, ls_n, ls_fs);
		loadModelGameBlockTexCoord();

		pos_begin = CoreMesh::painter.getSize();		// 记录第一次所在位置

		// 随机生成游戏方块类型
		for (int i = 0; i < NUMS_GAME_BLOCKS; i++) {
			game_block_type[i] = AuxiliaryFunc::rand() % NUMS_GAME_BLOCKS_TYPE;
		}

		// 重加载所有游戏方块
		for (int z = -4; z <= 4; z++) {
			for (int x = -4; x <= 4; x++) {
				reloadModelGameBlockAt(x, z);
			}
		}
	}

	// ModelGameBlock::reloadModelGameBlockAt 重新加载位于(x, z)的游戏方块
	void reloadModelGameBlockAt(int x, int z, vec3 color, float rotation_y, float scale, vec3 translate_vec) {
		// 获取对应下标
		int index = getIndex(x, z);
		game_block_model_matrix[index] =
			Translate(x * ModelGround::block_scale, game_block_scale * 0.5 + game_block_offset_y, z * ModelGround::block_scale)	// 移动到地面的相应位置
			* Translate(translate_vec) //平移translate_vec的量
			* RotateY(45) // 初始旋转45度
			* RotateY(rotation_y)	// 绕y轴旋转rotation_y度
			* Scale(game_block_scale, game_block_scale, game_block_scale) //缩放game_block_scale值
			* Scale(scale);	//根据传入参数缩放
		
		if (pos_begin + getIndex(x, z) == CoreMesh::painter.getSize()) {
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, game_block_model_matrix[index], color);
			mesh->setTexture(game_block_tex_type[game_block_type[index]], ls_vt);
			CoreMesh::painter.addMesh(mesh, getIndex(x, z) + pos_begin, true);
		} else {
			CoreMesh::Mesh* mesh = CoreMesh::painter.getMeshAt(pos_begin + getIndex(x, z));
			mesh->model_matrix = game_block_model_matrix[index];
			mesh->tex_type = game_block_tex_type[game_block_type[index]];
			mesh->color = color;
		}
		
	}

	// 获取被选择的游戏方块（模拟子弹）
	int getIndexGameBlockPointed() {
		// 最大迭代次数
		int max_time = 40;
		// 子弹最远运动距离
		float max_distance = 2.5 * ModelGround::block_scale + 0.5 * game_block_scale;
		// 子弹运动速度
		float v = max_distance / max_time;

		// 模拟运动
		for (int iterate_time = 1; iterate_time <= max_time; iterate_time++) {
			// 当前位置
			vec3 position = cam.position + v * iterate_time * cam.front;

			// 剪枝优化，若不落入y的范围直接继续迭代
			if (position.y < game_block_offset_y || position.y > game_block_offset_y + game_block_scale) {
				continue;
			}

			// 获取被打中的游戏方块
			int x = -5, z = -5;
			for (int i = -4; i <= 4; i++) {
				if (i * ModelGround::block_scale - game_block_scale * 0.5 <= position.x &&
					position.x <= i * ModelGround::block_scale + game_block_scale * 0.5) {
					x = i;
				}
				if (i * ModelGround::block_scale - game_block_scale * 0.5 <= position.z &&
					position.z <= i * ModelGround::block_scale + game_block_scale * 0.5) {
					z = i;
				}
			}

			// 若打中，则返回被打中的游戏方块下标
			if (x != -5 && z != -5) {
				return getIndex(x, z);
			}
		}
		// 若没打中，返回-1
		return -1;
	}


	// 检查行中是否有可消除的游戏方块
	bool checkGameBlockRowRemoved() {
		// 清空记录的STL向量
		game_block_removed_ls.clear();

		for (int z = -4; z <= 4; z++) {
			int cnt = 1;	// 记录值默认为1
			for (int x = -3; x <= 4; x++) {
				int t1 = game_block_type[getIndex(x - 1, z)];
				int t2 = game_block_type[getIndex(x, z)];
				if (t1 == t2) {		// 若相邻的相同
					cnt++;			// 则记录值+1
				}
				else {			// 否则记录值重置为1
					cnt = 1;
				}

				// 若有3个连续相同，则加入STL向量中
				if (cnt == 3) {
					game_block_removed_ls.push_back(getIndex(x - 2, z));
					game_block_removed_ls.push_back(getIndex(x - 1, z));
					game_block_removed_ls.push_back(getIndex(x, z));
				}
				// 若超过3个，则当前的也应该被加入STL向量中
				if (cnt > 3) {
					game_block_removed_ls.push_back(getIndex(x, z));
				}
			}
		}
		// 若STL向量非空，则说明有可被消除的游戏方块
		return !game_block_removed_ls.empty();
	}

	// 检查列中是否有可消除的游戏方块,同上
	bool checkGameBlockColRemoved() {
		game_block_removed_ls.clear();
		for (int x = -4; x <= 4; x++) {
			int cnt = 1;
			for (int z = -3; z <= 4; z++) {
				int t1 = game_block_type[getIndex(x, z - 1)];
				int t2 = game_block_type[getIndex(x, z)];
				if (t1 == t2) {
					cnt++;
				} else {
					cnt = 1;
				}

				if (cnt == 3) {
					game_block_removed_ls.push_back(getIndex(x, z - 2));
					game_block_removed_ls.push_back(getIndex(x, z - 1));
					game_block_removed_ls.push_back(getIndex(x, z));
				}
				if (cnt > 3) {
					game_block_removed_ls.push_back(getIndex(x, z));
				}
			}
		}
		return !game_block_removed_ls.empty();
	}

	// 更新选择的方块
	void updateGameBlockPointed() {
		if (is_game_block_removed == true || is_game_block_swaped == true) {
			return;
		}

		// 若上一次有指向方块，或者没有被选中的方块，则恢复上一次指向的方块本身的颜色
		if (game_block_pointed_index != -1 && game_block_pointed_index != game_block_selected_index) {
			int x, z;
			getXZ(game_block_pointed_index, x, z);
			reloadModelGameBlockAt(x, z);
		}

		// 获取本次被指向的方块
		game_block_pointed_index = getIndexGameBlockPointed();

		// 若不存在或者与被选中的游戏方块是同一个，则不做修改
		if (game_block_pointed_index == -1 || game_block_pointed_index == game_block_selected_index) {
			return;
		}

		// 否则做对应修改
		int x, z;
		getXZ(game_block_pointed_index, x, z);
		reloadModelGameBlockAt(x, z, vec3(1.5, 1.5, 1.5));
	}

	// 更新被选择时的游戏方块
	void updateGameBlockSelected() {
		// 若有游戏方块正在被移除，那么不更新
		if (is_game_block_removed == true || is_game_block_swaped == true) {
			return;
		}
		
		int x, z;

		// 若指向的游戏方块与上一次被选择的游戏方块不同
		if (game_block_pointed_index != game_block_selected_index) {
			// 若上一次被选择的方块存在，则取消给方块的效果
			if (game_block_selected_index != -1) {
				getXZ(game_block_selected_index, x, z);
				reloadModelGameBlockAt(x, z);
				is_game_block_selected = false;
			}
			// 若本次的选择不能发生交换
			if (!updateGameBlockIsSwaped()) {
				getXZ(game_block_pointed_index, x, z);
				is_game_block_selected = true;
				game_block_selected_index = game_block_pointed_index;
			}
		} else if(game_block_pointed_index != -1){
			// 若选择的方块与上一次相同，那么取消这一方块的效果
			getXZ(game_block_pointed_index, x, z);
			reloadModelGameBlockAt(x, z);
			game_block_selected_index = -1;
			is_game_block_selected = false;
		}
	}

	// 游戏方块推开机器人
	void pushRobot(int game_block_removed_series_x, int game_block_removed_series_z) {
		// 计算实际(x,z)坐标
		float game_block_position_x = game_block_removed_series_x * ModelGround::block_scale;
		float game_block_position_z = game_block_removed_series_z * ModelGround::block_scale;
		// 计算推开的方向
		vec3 game_block_push_direction = ModelRobot::move_position - vec3(game_block_position_x, 0, game_block_position_z);
		game_block_push_direction.y = 0;
		game_block_push_direction = normalize(game_block_push_direction);
		// 更新机器人的位置
		ModelRobot::move_position += program_speed * PUSH_ROBOT_SPEED * game_block_push_direction;
		// 更新机器人的血量
		ModelRobot::robot_hp -= 0.05;

		if (ModelRobot::robot_hp <= 0.0) {
			ModelRobot::setRobotOver();
		}
	}

	// 处理游戏方块消除动画
	void ProcessGameBlockRemove() {
		// 计算动画时间，缩放速度与旋转速度
		float animation_time = GAME_BLOCK_REMOVED_MAX_TIME;
		float animation_scale_v = 1 / animation_time;
		float animation_rotation_v = 540 / animation_time;

		if (game_block_removed_step == 1) {		// 若在动画步骤一
			if (game_block_removed_time <= animation_time) {	// 若此动画时间还未结束
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					// 计算当前缩放值与旋转值
					float game_block_removed_scale = std::max(1 - animation_scale_v * game_block_removed_time, 0.001f);
					float game_block_removed_rotation = animation_rotation_v * game_block_removed_time;
					// 更新方块
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					reloadModelGameBlockAt(game_block_removed_series_x, game_block_removed_series_z, vec3(1, 1, 1), game_block_removed_rotation, game_block_removed_scale);
				}
				// 时间推移
				game_block_removed_time += program_speed * 1.5;
			} else {	// 若动画步骤一结束
				// 转移到动画步骤二
				game_block_removed_step++;
				// 对消除的方块重新随机生成类型
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					game_block_type[game_block_removed_series_index] = rand() % NUMS_GAME_BLOCKS_TYPE;
				}
			}
		}

		if (game_block_removed_step == 2) {	// 若在动画步骤二
			// 计算相对时间与y轴正向的偏移，使游戏方块做自由落体运动
			float time = game_block_removed_time - animation_time;
			float game_block_removed_translate_y = 150 * program_speed - 0.5 * 9.8 * time * time;
			// 若游戏方块还未着地
			if (game_block_removed_translate_y >= game_block_offset_y) {
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					// 重加载游戏方块
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					reloadModelGameBlockAt(game_block_removed_series_x, game_block_removed_series_z, vec3(1.0, 1.0, 1.0), 0.0, 1.0, vec3(0, game_block_removed_translate_y, 0));
					// 若发生了碰撞则推开机器人
					if (ModelGameBlock::checkPointIsInGameBlock(ModelRobot::move_position +
						vec3(0, ModelRobot::TORSO_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + ModelRobot::LOWER_LEG_HEIGHT, 0), 0, game_block_removed_series_index)) {
						pushRobot(game_block_removed_series_x, game_block_removed_series_z);
					}
				}
				// 时间推移
				game_block_removed_time += program_speed * 1.2;
			} else {	// 若动画步骤二结束
				// 重加载游戏方块
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					reloadModelGameBlockAt(game_block_removed_series_x, game_block_removed_series_z, vec3(1.0, 1.0, 1.0), 0.0, 1.0, 0.0);
				}

				// 时间清0，步骤置一，标记变量置为假
				game_block_removed_time = 0;
				game_block_removed_step = 1;
				is_game_block_removed = false;
			}
		}
	}

	// 随机消除方块
	void ProcessGameBlockRandomBoom() {
		// 随机选择数量为[1, 5]
		int nums = AuxiliaryFunc::rand() % 5 + 1;
		// 记录随机选择的方块下标
		std::vector<int> nums_series;
		// 随机生成[0, NUMS_GAME_BLOCKS)，即范围为所有游戏方块
		AuxiliaryFunc::genRandomSeries(nums_series, 0, NUMS_GAME_BLOCKS - 1);

		// 置是否有游戏方块消除标记变量为真
		is_game_block_removed = true;
		// 清除记录的消除方块下标的STL向量，并加入随机生成的游戏方块下标
		game_block_removed_ls.clear();
		for (int i = 0; i < nums; i++) {
			game_block_removed_ls.push_back(nums_series[i]);
		}
	}

	// 被选中的游戏方块的动画效果
	void ProcessGameBlockSelected() {
		if (game_block_selected_index == -1) {
			return;
		}

		// 变化值
		float delta_sita = 0.3 * sin(game_block_selected_time) + 1;
		vec3  delta_vec = vec3(delta_sita, delta_sita, delta_sita);
		// 更新颜色
		int x, z;
		getXZ(game_block_selected_index, x, z);
		reloadModelGameBlockAt(x, z, delta_vec);
		// 时间推移
		game_block_selected_time += program_speed * 0.2;
	}

	// ModelGameBlock::ProcessCoreGame 游戏基础逻辑空闲回调函数
	void ProcessCoreGame() {
		if (ModelRobot::is_robot_alive == false) {
			return;
		}

		if (is_game_block_swaped) {	// 若有方块交换则继续处理动画
			ProcessGameBlockSwap();
		} else if (is_game_block_removed) {	// 否则，若有方块正在消除则进行消除动画
			ProcessGameBlockRemove();
		} else if (AuxiliaryFunc::rand() % (int)(1000 / program_speed) == 0){	// 否则以一定概率进行随机消除
			ProcessGameBlockRandomBoom();
		} else {	// 否则检查是否有方块可进行消除，并处理游戏方块被选择的动画
			updateGameBlockIsRemoved();
			if (is_game_block_selected) {
				ProcessGameBlockSelected();
			}
		}
	}

	// 处理游戏方块交换动画
	void ProcessGameBlockSwap() {
		// 计算动画时间与速度
		float animation_time = GAME_BLOCK_SWAPED_MAX_TIME;
		float animation_v = ModelGround::block_scale / animation_time;
		if (game_block_swaped_time <= animation_time) {		// 若还处于动画时间中
			// 计算x轴与z轴的偏移量
			float delta_x = game_block_swaped_direction_x * std::min(animation_v * game_block_swaped_time, ModelGround::block_scale);
			float delta_z = game_block_swaped_direction_z * std::min(animation_v * game_block_swaped_time, ModelGround::block_scale);
			float delta_rotation = animation_v * game_block_swaped_time;
			// 重加载游戏方块模型
			int x0, z0, x1, z1;
			getXZ(game_block_swaped_index[0], x0, z0);
			getXZ(game_block_swaped_index[1], x1, z1);
			reloadModelGameBlockAt(x0, z0, vec3(1.0, 1.0, 1.0), delta_rotation, 1, vec3(delta_x, 0, delta_z));
			reloadModelGameBlockAt(x1, z1, vec3(1.0, 1.0, 1.0), -delta_rotation, 1, vec3(-delta_x, 0, -delta_z));
			game_block_swaped_time += program_speed * 2;
		} else {	// 若动画结束
			// 交换两方块类型，并重置移动分量，重加载模型
			int x0, z0, x1, z1;
			getXZ(game_block_swaped_index[0], x0, z0);
			getXZ(game_block_swaped_index[1], x1, z1);
			std::swap(game_block_type[game_block_swaped_index[0]], game_block_type[game_block_swaped_index[1]]);
			reloadModelGameBlockAt(x0, z0, vec3(1.0, 1.0, 1.0), 0.0, 1, vec3(0, 0, 0));
			reloadModelGameBlockAt(x1, z1, vec3(1.0, 1.0, 1.0), 0.0, 1, vec3(0, 0, 0));
			// 重置动画时间，将是否进行交换的标记变量置为假，将是否进行移除的标记变量置为真
			game_block_swaped_time = 0;
			is_game_block_swaped = false;
			is_game_block_removed = true;
		}
	}

	// 更新被移除的方块
	void updateGameBlockIsRemoved() {
		// 检查是否有可被移除的方块
		if (checkGameBlockRowRemoved() || checkGameBlockColRemoved()) {
			// 标记变量设置为真
			is_game_block_removed = true;
			// 分数增加
			ModelRobot::score += game_block_removed_ls.size();
			// 更新记分板分数
			ModelBoard::ProcessScoreUpdate();
			// 更新手雷数量
			ModelBomb::updateBombNums();
		}
	}

	// 判断方块是否可交换与更新
	bool updateGameBlockIsSwaped() {
		// 当前指向方块和上一次指向方块其中一个不存在则不继续操作
		if (game_block_pointed_index == -1 || game_block_selected_index == -1) {
			return false;
		}

		// 获取两个方块的(x,z)坐标
		int x1, z1, x2, z2;
		getXZ(game_block_pointed_index, x1, z1);
		getXZ(game_block_selected_index, x2, z2);

		// 计算曼哈顿距离
		int delta_x = (x1 - x2 > 0 ? x1 - x2 : x2 - x1);
		int delta_z = (z1 - z2 > 0 ? z1 - z2 : z2 - z1);

		// 若不相邻则不继续操作
		if (!((delta_x == 1 && delta_z == 0) || (delta_x == 0 && delta_z == 1))) {
			return false;
		}

		// 首先交换两个方块的类型，并尝试能否进行消除，若不能则不继续操作，最后需要交换回两个方块的类型
		std::swap(game_block_type[game_block_pointed_index], game_block_type[game_block_selected_index]);
		if (!checkGameBlockColRemoved() && !checkGameBlockRowRemoved()) {
			std::swap(game_block_type[game_block_pointed_index], game_block_type[game_block_selected_index]);
			return false;
		}
		std::swap(game_block_type[game_block_pointed_index], game_block_type[game_block_selected_index]);

		// 可交换则增加分数，尝试增加手雷数量等
		ModelRobot::score += ModelGameBlock::game_block_removed_ls.size();
		ModelBoard::ProcessScoreUpdate();
		ModelBomb::updateBombNums();
		
		// 保存两个方块的下标值，并给定初始值
		game_block_swaped_index[0] = game_block_pointed_index;
		game_block_swaped_index[1] = game_block_selected_index;
		game_block_swaped_direction_x = 1;
		game_block_swaped_direction_z = 0;
		// 将x值或z值小的方块放在这一数组的0位置
		if ((delta_z && z1 > z2) || (delta_x && x1 > x2)) {
			std::swap(game_block_swaped_index[0], game_block_swaped_index[1]);
		}
		// 若是存在z轴的分量，那么将x轴分量设为0，z轴分量设为1
		// 即交换初始值
		if (delta_z) {
			std::swap(game_block_swaped_direction_x, game_block_swaped_direction_z);
		}
		// 将是否正在交换的标记变量设为真
		is_game_block_swaped = true;
		game_block_selected_index = -1;
		return true;
	}

	// ModelGameBlock::checkPointIsInGameBlock 对点position对于大小偏移为delta的任意方块的碰撞检测
	int checkPointIsInGameBlock(vec3 position, float delta) {
		// 获取其所在地面方块的坐标(x,z)
		int x = -5, z = -5;
		for (int i = -4; i <= 4; i++) {
			if (i * ModelGround::block_scale - 0.5 * ModelGround::block_scale <= position.x
				&& position.x <= i * ModelGround::block_scale + 0.5 * ModelGround::block_scale) {
				x = i;
				break;
			}
		}
		for (int i = -4; i <= 4; i++) {
			if (i * ModelGround::block_scale - 0.5 * ModelGround::block_scale <= position.z
				&& position.z <= i * ModelGround::block_scale + 0.5 * ModelGround::block_scale) {
				z = i;
				break;
			}
		}

		// 若不位于9x9地面上，则返回-1，表示不存在
		if (x == -5 || z == -5) {
			return -1;
		}

		// 否则，扫描周围3x3区域内的游戏方块，做碰撞检测
		for (int i = std::max(x - 1, -4); i <= 4 && i <= x + 1; i++) {
			for (int j = std::max(z - 1, -4); j <= 4 && j <= z + 1; j++) {
				if (checkPointIsInGameBlock(position, delta, getIndex(i, j))) {
					return getIndex(i, j);
				}
			}
		}

		// 无碰撞则返回-1
		return -1;
	}

	// 对点position对于下标index的游戏方块大小偏移为delta的碰撞检测
	bool checkPointIsInGameBlock(vec3 position, float delta, int index) {
		// 初始游戏方块坐标
		vec4 game_block_position[8] = {
			vec4(-0.5, -0.5, -0.5, 1),
			vec4(-0.5, -0.5, 0.5, 1),
			vec4(-0.5, 0.5, -0.5, 1),
			vec4(-0.5, 0.5, 0.5, 1),
			vec4(0.5, -0.5, -0.5, 1),
			vec4(0.5, -0.5, 0.5, 1),
			vec4(0.5, 0.5, -0.5, 1),
			vec4(0.5, 0.5, 0.5, 1)
		};

		// x, z坐标的最小值最大值, y坐标的最小值
		float minx = 1e9, maxx = -1e9;
		float minz = 1e9, maxz = -1e9;
		float miny = 1e9;
		// 维护上述相关值
		for (int k = 0; k < 8; k++) {
			vec4 point = game_block_model_matrix[index] * game_block_position[k];
			minx = std::min(minx, point.x);
			maxx = std::max(maxx, point.x);
			minz = std::min(minz, point.z);
			maxz = std::max(maxz, point.z);
			miny = std::min(miny, point.y);
		}
		// 若在其中，说明发生碰撞，返回真
		if (minx + delta <= position.x && position.x <= maxx - delta && minz + delta <= position.z &&
			position.z <= maxz - delta && position.y >= miny + delta) {
			return true;
		}
		return false;
	}

	// 初始化游戏方块各变量
	void initGameBlock() {
		game_block_pointed_index = -1;
		game_block_removed_step = 1;				
		game_block_removed_time = 0;				
		is_game_block_removed = false;				
		game_block_removed_ls.clear();			
		game_block_swaped_index[0] = game_block_swaped_index[0] = -1;	
		game_block_swaped_direction_x = 0;			
		game_block_swaped_direction_z = 0;			
		game_block_swaped_time = 0;				
		is_game_block_swaped = false;
												
		game_block_selected_index = -1;				
		game_block_selected_time = 0;				
		is_game_block_selected = false;			
	}
}

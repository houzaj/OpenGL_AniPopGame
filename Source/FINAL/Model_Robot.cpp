// 机器人
#include "Auxiliary_H.h"
#include "Model.h"

#include <stack>
#include <vector>

namespace ModelRobot {
	typedef Angel::vec4 point4;
	typedef Angel::vec4 color4;

	const float TORSO_HEIGHT = 0.8;
	const float TORSO_WIDTH_X = 0.4;
	const float TORSO_WIDTH_Y = 0.25;
	const float UPPER_ARM_HEIGHT = 0.4;
	const float LOWER_ARM_HEIGHT = 0.3;
	const float UPPER_LEG_WIDTH = 0.15;
	const float LOWER_LEG_WIDTH = 0.15;
	const float LOWER_LEG_HEIGHT = 0.5;
	const float UPPER_LEG_HEIGHT = 0.4;
	const float UPPER_ARM_WIDTH = 0.12;
	const float LOWER_ARM_WIDTH = 0.1199;
	const float HEAD_HEIGHT = 0.4;
	const float HEAD_WIDTH = 0.4;

	// Joint angles with initial values
	GLfloat theta[NumJointAngles] = {
		0.0,    // Torso
		0.0,    // Head1
		0.0,    // Head2
		180.0,  // RightUpperArm
		0.0,    // RightLowerArm
		180.0,  // LeftUpperArm
		0.0,    // LeftLowerArm
		180.0,  // RightUpperLeg
		0.0,    // RightLowerLeg
		180.0,  // LeftUpperLeg
		0.0     // LeftLowerLeg
	};

	point4 color_head = point4(0, 1, 0, 1);
	point4 color_torso = point4(0, 0, 1, 1);
	point4 color_upper_arm = point4(1, 1, 0, 1);
	point4 color_lower_arm = point4(1, 0, 0, 1);
	point4 color_upper_leg = point4(0, 1, 1, 1);
	point4 color_lower_leg = point4(0.5, 0.5, 0.5, 1);

	std::stack<mat4> mvstack;
	mat4 model_matrix;

	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;
	int pos_begin;								// 记录STL向量中机器人模型中第一个图形的位置
	int pos_iterate;							// 遍历时使用的变量

	/* 移动相关 */
	vec3 move_position = vec3(0, 0, 5 * ModelGround::block_scale);			// 机器人位置
	vec3 move_direction = vec3(0, 0, -1);									// 机器人朝向
	vec3 move_direction_n = vec3(1, 0, 0);									// 机器人右向
	float move_speed = 0.2;													// 移动速度
	float move_time;														// 移动时间
	float per_move_speed;													// 动作速度
	float view_yaw;															// 视角在y=0平面的角度
	const float MOVE_SPEED_EPS = 1e-4;										// 无穷小量
	const float PER_MOVE_TIME = 150;										// 动作时间

	/* 跳跃相关 */
	bool is_jump;										// 标记是否处于跳跃状态
	float pre_move_position_y;							// 起跳前的y坐标
	float move_up_time;									// 目前跳跃时间

	/* 道具相关 */
	bool is_robot_get_item = false;						// 机器人是否获得道具

	const float ROBOT_SPEED = 0.2;
	float robot_speed_up_time = 0;						// 加速道具作用时间
	float robot_speed_down_time = 0;					// 减速道具作用时间
	float robot_speed_recover_time = 0;					// 恢复原速度时间
	const float ROBOT_SPEED_RECOVER_MAX_TIME = 200;		// 恢复原速度最大时间

	/* 血量相关 */
	float robot_hp = 1;									// 机器人血量
	bool is_robot_alive = true;							// 机器人是否存活的标记变量
	const float ROBOT_HP_COMSUME_FACTOR = 5e-5;			// 扣血因子
	float robot_hp_down_time = 0;						// 扣血动画时间
	float robot_hp_down_moveup_speed = 30;				// 扣血动画中的上抛速度
	bool is_robot_hp_down = false;						// 是否正在扣血标记变量
	
	/* 分数相关 */
	int score = 0;										// 分数
	const int SCORE_MAX = 9999;							// 分数最大值

	/* 手雷相关 */
	float robot_throw_bomb_time = 0;					// 动画时间
	float ROBOT_THROW_BOMB_MAX_TIME = 20;				// 扔出手雷最大动画时间
	bool is_robot_throw_bomb = false;					// 是否扔出手雷的标记变量

	/* UV坐标相关 */
	// 脸部的UV坐标加载
	void loadModelRobotFaceTexCoord() {
		int n = ls_v.size();
		ls_vt.clear();
		ls_vt.resize(n);
		
		int index = 4 * 5;
		ls_vt[index + 0] = vec2(0, 1);
		ls_vt[index + 1] = vec2(1, 1);
		ls_vt[index + 2] = vec2(1, 0);
		ls_vt[index + 3] = vec2(0, 0);
	}

	// 躯体的UV坐标加载
	void loadModelRobotTorsoTexCoord() {
		ls_vt.clear();
		float unit = 0.5 / (TORSO_WIDTH_X + TORSO_WIDTH_Y);
		float b = unit * TORSO_WIDTH_Y;
		float a = unit * TORSO_WIDTH_X;
		float h = unit * TORSO_HEIGHT;

		ls_vt.push_back(vec2(0, h));
		ls_vt.push_back(vec2(b, h));
		ls_vt.push_back(vec2(b, 0));
		ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, h + b));
		ls_vt.push_back(vec2(a, h + b));
		ls_vt.push_back(vec2(a, h));
		ls_vt.push_back(vec2(0, h));

		ls_vt.push_back(vec2(0, h));
		ls_vt.push_back(vec2(b, h));
		ls_vt.push_back(vec2(b, 0));
		ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(0, h + b));
		ls_vt.push_back(vec2(a, h + b));
		ls_vt.push_back(vec2(a, h));
		ls_vt.push_back(vec2(0, h));

		ls_vt.push_back(vec2(0, h));
		ls_vt.push_back(vec2(b, h));
		ls_vt.push_back(vec2(b, 0));
		ls_vt.push_back(vec2(0, 0));

		ls_vt.push_back(vec2(2 * b, h));
		ls_vt.push_back(vec2(2 * b + a, h));
		ls_vt.push_back(vec2(2 * b + a, 0));
		ls_vt.push_back(vec2(2 * b, 0));
	}

	// 大臂的UV坐标加载
	void loadModelRobotUpperArmTexCoord() {
		ls_vt.clear();
		int n = ls_v.size();

		float unit = 1.0 / 3;
		float h = unit * UPPER_ARM_HEIGHT;
		float w = unit;
		
		for (int i = 0; i < n; i += 4) {
			ls_vt.push_back(vec2(w, h));
			ls_vt.push_back(vec2(w + w, h));
			ls_vt.push_back(vec2(w + w, 0));
			ls_vt.push_back(vec2(w, 0));
		}
	}

	// 小臂的UV坐标加载
	void loadModelRobotLowerArmTexCoord() {
		ls_vt.clear();
		
		float unit = 1.0 / (3 * LOWER_LEG_WIDTH);
		float h = unit * LOWER_ARM_HEIGHT;
		float w = unit * LOWER_LEG_WIDTH;

		for (int i = 0; i < 2; i++) {
			ls_vt.push_back(vec2(w, 0));
			ls_vt.push_back(vec2(0, 0));
			ls_vt.push_back(vec2(0, h));
			ls_vt.push_back(vec2(w, h));
			

			ls_vt.push_back(vec2(2 * w, w));
			ls_vt.push_back(vec2(2 * w + w, w));
			ls_vt.push_back(vec2(2 * w + w, 0));
			ls_vt.push_back(vec2(2 * w, 0));
		}

		for (int i = 0; i < 2; i++) {
			ls_vt.push_back(vec2(w, 0));
			ls_vt.push_back(vec2(0, 0));
			ls_vt.push_back(vec2(0, h));
			ls_vt.push_back(vec2(w, h));
		}
	}

	// 大腿的UV坐标加载
	void loadModelRobotUpperLegTexCoord() {
		ls_vt.clear();

		float unit = 1.0 / (4 * UPPER_LEG_WIDTH);
		float h = unit * UPPER_LEG_HEIGHT;
		float w = unit * UPPER_LEG_WIDTH;

		for (int i = 0; i < 2; i++) {
			ls_vt.push_back(vec2(2 * w + w, 0));
			ls_vt.push_back(vec2(2 * w, 0));
			ls_vt.push_back(vec2(2 * w, h));
			ls_vt.push_back(vec2(2 * w + w, h));

			ls_vt.push_back(vec2(3 * w, w));
			ls_vt.push_back(vec2(3 * w, w));
			ls_vt.push_back(vec2(3 * w, w));
			ls_vt.push_back(vec2(3 * w, w));
		}

		ls_vt.push_back(vec2(w + w, 0));
		ls_vt.push_back(vec2(w, 0));
		ls_vt.push_back(vec2(w, h));
		ls_vt.push_back(vec2(w + w, h));

		ls_vt.push_back(vec2(2 * w + w, 0));
		ls_vt.push_back(vec2(2 * w, 0));
		ls_vt.push_back(vec2(2 * w, h));
		ls_vt.push_back(vec2(2 * w + w, h));
	}

	// 小腿的UV坐标加载
	void loadModelRobotLowerLegTexCoord() {
		ls_vt.clear();

		float unit = 1.0 / (4 * LOWER_LEG_WIDTH);
		float h = unit * LOWER_LEG_HEIGHT;
		float w = unit * LOWER_LEG_WIDTH;

		for (int i = 0; i < 2; i++) {
			ls_vt.push_back(vec2(w, 0));
			ls_vt.push_back(vec2(0, 0));
			ls_vt.push_back(vec2(0, h - 0.1));
			ls_vt.push_back(vec2(w, h - 0.1));
			
			ls_vt.push_back(vec2(2 * w + w, 0));
			ls_vt.push_back(vec2(2 * w, 0));
			ls_vt.push_back(vec2(2 * w, h));
			ls_vt.push_back(vec2(2 * w + w, h));
		}

		for (int i = 0; i < 2; i++) {
			ls_vt.push_back(vec2(w, 0));
			ls_vt.push_back(vec2(0, 0));
			ls_vt.push_back(vec2(0, h - 0.01));
			ls_vt.push_back(vec2(w, h - 0.01));
		}
	}

	/* 绘制相关 */
	// 绘制躯体
	void torso() {
		mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
			Scale(TORSO_WIDTH_X, TORSO_HEIGHT, TORSO_WIDTH_Y));
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotTorsoTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_TORSO, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
		
	}

	// 绘制头部
	void head() {
		mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
			Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));//本节点局部变换矩阵

		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotFaceTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_FACE, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
		
	}

	// 绘制左大臂
	void left_upper_arm() {
		mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
			Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//本节点局部变换矩阵

		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotUpperArmTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1.0, 1.0, 1.0));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_ARM, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else{
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制左小臂
	void left_lower_arm() {
		mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
			Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//本节点局部变换矩阵
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotLowerArmTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_ARM, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制右大臂
	void right_upper_arm() {
		mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
			Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//本节点局部变换矩阵
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotUpperArmTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1.0, 1.0, 1.0));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_ARM, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制右小臂
	void right_lower_arm() {
		mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
			Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//本节点局部变换矩阵
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotLowerArmTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_ARM, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制左大腿
	void left_upper_leg() {
		mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
			Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//本节点局部变换矩阵
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotUpperLegTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_LEG, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制左小腿
	void left_lower_leg() {
		mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
			Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//本节点局部变换矩阵

		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotLowerLegTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_LEG, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制右大腿
	void right_upper_leg() {
		mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
			Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//本节点局部变换矩阵
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotUpperLegTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_LEG, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 绘制右小腿
	void right_lower_leg() {
		mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
			Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//本节点局部变换矩阵
		if (pos_iterate == CoreMesh::painter.getSize()) {
			loadModelRobotLowerLegTexCoord();
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, model_matrix * instance, vec3(1, 1, 1));
			mesh->setTexture(CoreImageTexture::TextureType::MODEL_ROBOT_LEG, ls_vt);
			CoreMesh::painter.addMesh(mesh, pos_iterate++, true);
		} else {
			CoreMesh::painter.getMeshAt(pos_iterate++)->model_matrix = model_matrix * instance;
		}
	}

	// 重新加载机器人模型
	void reloadModelRobot() {
		// 令记录遍历位置的变量等于绘制机器人模型中第一个图形在STL向量中的位置
		pos_iterate = pos_begin;
		per_move_speed = move_speed * program_speed * 70;

		// 通过层级建模思想绘制
		model_matrix = Translate(move_position.x, UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT + move_position.y, move_position.z) * RotateY(view_yaw) * RotateY(theta[Torso]);//躯干变换矩阵
		torso();
		mvstack.push(model_matrix);

		model_matrix *= Translate(0, TORSO_HEIGHT, 0);
		head();

		model_matrix = mvstack.top();
		model_matrix *= Translate(-TORSO_WIDTH_X * 0.5 - UPPER_ARM_WIDTH * 0.5, TORSO_HEIGHT, 0) * RotateX(theta[LeftUpperArm]);
		left_upper_arm();

		model_matrix *= Translate(0, UPPER_ARM_HEIGHT, 0) * RotateX(theta[LeftLowerArm]);
		left_lower_arm();

		// 绘制右上臂
		model_matrix = mvstack.top();
		model_matrix *= Translate(TORSO_WIDTH_X * 0.5 + UPPER_ARM_WIDTH * 0.5, TORSO_HEIGHT, 0) * RotateX(theta[RightUpperArm]);
		right_upper_arm();

		// 绘制左上臂
		model_matrix *= Translate(0, UPPER_ARM_HEIGHT, 0) * RotateX(theta[RightLowerArm]);
		right_lower_arm();

		if (is_robot_throw_bomb == true) {
			model_matrix *= Translate(0, LOWER_ARM_HEIGHT, 0);
			ModelBomb::reloadModelBomb(model_matrix);
		}


		model_matrix = mvstack.top();
		model_matrix *= Translate(-TORSO_WIDTH_X * 0.5 + UPPER_LEG_WIDTH * 0.5, 0, 0) * RotateX(theta[LeftUpperLeg]);
		left_upper_leg();

		model_matrix *= Translate(0, UPPER_LEG_HEIGHT, 0) * RotateX(theta[LeftLowerLeg]);
		left_lower_leg();

		model_matrix = mvstack.top();
		model_matrix *= Translate(TORSO_WIDTH_X * 0.5 - UPPER_LEG_WIDTH * 0.5, 0, 0) * RotateX(theta[RightUpperLeg]);
		right_upper_leg();

		model_matrix *= Translate(0, UPPER_LEG_HEIGHT, 0) * RotateX(theta[RightLowerLeg]);
		right_lower_leg();
	}

	// 加载机器人
	void loadModelRobot() {
		// 生成单位立方体
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);

		move_time = 0;												// 初始化移动时间
		pos_iterate = pos_begin = CoreMesh::painter.getSize();		// 记录第一次所在位置

		// 设置为初始值
		theta[LeftUpperLeg] = theta[RightUpperLeg] = 180;
		theta[LeftLowerLeg] = theta[RightLowerLeg] = 0;
		theta[LeftUpperArm] = 180;
		theta[RightUpperArm] = 180;
		theta[Torso] = 0;
		move_time = 0;
		view_yaw = 0;
		is_jump = false;

		// 重新加载模型
		reloadModelRobot();
	}

	// 检查是否可以通过键盘等移动
	bool checkCanMove() {
		return is_jump == false &&
			is_robot_hp_down == false &&
			is_robot_throw_bomb == false;
	}

	// 检查机器人是否可以移动
	bool checkMove(vec3 delta) {
		//超过5x5方块视为越界，阻止移动
		float newx = move_position.x + delta.x;
		float newz = move_position.z + delta.z;
		if (newx < -5.4 * ModelGround::block_scale || newx > 5.4 * ModelGround::block_scale ||
			newz < -5.4 * ModelGround::block_scale || newz > 5.4 * ModelGround::block_scale) {
			return false;
		}

		// 获取发生碰撞检测的游戏方块下标
		int index = ModelGameBlock::checkPointIsInGameBlock(move_position + vec3(0, TORSO_HEIGHT, 0) + delta, ModelGameBlock::game_block_scale * 0.2);

		// 若不存在，则说明机器人没有与任何方块发生碰撞，应允许移动
		if (index == -1) {
			return true;
		}

		// 通过点乘判定机器人是否正在远离方块，是则允许移动，否则禁止移动
		int x, z;
		ModelGameBlock::getXZ(index, x, z);
		vec3 v = vec3(x * ModelGround::block_scale, 0, z * ModelGround::block_scale) - move_position;
		v.y = 0, delta.y = 0;
		if (dot(delta, v) <= 0) {
			return true;
		}
		return false;
	}

	// 更新机器人的移动位置
	void updateMovePosition() {
		if (GameMouseKeyboard::move_key_state == 8) {	// 若向前移动
			if (checkMove(move_speed * program_speed * move_direction)) {	// 若可以移动，下同
				// 增加正常移动量
				move_position += move_speed * program_speed * move_direction;
			} else {
				// 增加微小移动量
				move_position += MOVE_SPEED_EPS * move_speed * program_speed * move_direction;
			}
		} else if (GameMouseKeyboard::move_key_state == 4) {	// 若向后移动
			if (checkMove(-move_speed * program_speed * move_direction)) {
				move_position -= move_speed * program_speed * move_direction;
			} else {
				move_position -= MOVE_SPEED_EPS * move_speed * program_speed * move_direction;
			}
		} else if (GameMouseKeyboard::move_key_state == 2) {	// 若向左移动
			if (checkMove(-move_speed * program_speed * move_direction_n)) {
				move_position -= move_speed * program_speed * move_direction_n;
			} else {
				move_position -= MOVE_SPEED_EPS * move_speed * program_speed * move_direction_n;
			}
		} else if (GameMouseKeyboard::move_key_state == 1) {	// 若向右移动
			if (checkMove(move_speed * program_speed * move_direction_n)) {
				move_position += move_speed * program_speed * move_direction_n;
			} else {
				move_position += MOVE_SPEED_EPS * move_speed * program_speed * move_direction_n;
			}
		} else if (GameMouseKeyboard::move_key_state == 10) {	// 若向左前移动
			if (checkMove(move_speed * program_speed * move_direction / 1.141 - move_speed * program_speed * move_direction_n / 1.141)) {
				move_position += move_speed * program_speed * move_direction / 1.141;
				move_position -= move_speed * program_speed * move_direction_n / 1.141;
			} else {
				move_position += MOVE_SPEED_EPS * move_speed * program_speed * move_direction / 1.141;
				move_position -= MOVE_SPEED_EPS * move_speed * program_speed * move_direction_n / 1.141;
			}
		} else if (GameMouseKeyboard::move_key_state == 9) {	// 若向右前移动
			if (checkMove(move_speed * program_speed * move_direction / 1.141 + move_speed * program_speed * move_direction_n / 1.141)) {
				move_position += move_speed * program_speed * move_direction / 1.141;
				move_position += move_speed * program_speed * move_direction_n / 1.141;
			} else {
				move_position += MOVE_SPEED_EPS * move_speed * program_speed * move_direction / 1.141;
				move_position += MOVE_SPEED_EPS * move_speed * program_speed * move_direction_n / 1.141;
			}
		} else if (GameMouseKeyboard::move_key_state == 6) {	// 若向最后移动
			if (checkMove(-move_speed * program_speed * move_direction / 1.141 - move_speed * program_speed * move_direction_n / 1.141)) {
				move_position -= move_speed * program_speed * move_direction / 1.141;
				move_position -= move_speed * program_speed * move_direction_n / 1.141;
			} else {
				move_position -= MOVE_SPEED_EPS * move_speed * program_speed * move_direction / 1.141;
				move_position -= MOVE_SPEED_EPS * move_speed * program_speed * move_direction_n / 1.141;
			}
		} else if (GameMouseKeyboard::move_key_state == 5) {	// 若向右后移动
			if (checkMove(-move_speed * program_speed * move_direction / 1.141 + move_speed * program_speed * move_direction_n / 1.141)) {
				move_position -= move_speed * program_speed * move_direction / 1.141;
				move_position += move_speed * program_speed * move_direction_n / 1.141;
			} else {
				move_position -= MOVE_SPEED_EPS * move_speed * program_speed * move_direction / 1.141;
				move_position += MOVE_SPEED_EPS * move_speed * program_speed * move_direction_n / 1.141;
			}
		}
	}

	// 机器人向前移动
	void moveForward() {
		updateMovePosition();

		// 更新躯体的旋转角度为0
		theta[Torso] = 0;
		per_move_speed = move_speed * program_speed * 70;

		// 以4个动作为周期进行动作变化，下同
		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.2;
			theta[RightUpperLeg] -= per_move_speed * 0.2;
			theta[LeftUpperArm] -= per_move_speed * 0.1;
			theta[RightUpperArm] += per_move_speed * 0.1;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.2;
			theta[RightUpperLeg] += per_move_speed * 0.2;
			theta[LeftUpperArm] += per_move_speed * 0.1;
			theta[RightUpperArm] -= per_move_speed * 0.1;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.2;
			theta[RightUpperLeg] += per_move_speed * 0.2;
			theta[LeftUpperArm] += per_move_speed * 0.1;
			theta[RightUpperArm] -= per_move_speed * 0.1;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.2;
			theta[RightUpperLeg] -= per_move_speed * 0.2;
			theta[LeftUpperArm] -= per_move_speed * 0.1;
			theta[RightUpperArm] += per_move_speed * 0.1;
		}
		reloadModelRobot();

		// 到达周期时重新把移动时间设置为0
		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向后移动
	void moveBackward() {
		updateMovePosition();

		per_move_speed = move_speed * program_speed * 70;
		theta[Torso] = 0;
		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.06;
			theta[RightUpperLeg] -= per_move_speed * 0.03;
			theta[RightLowerLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.06;
			theta[RightUpperLeg] += per_move_speed * 0.03;
			theta[RightLowerLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[RightUpperLeg] += per_move_speed * 0.06;
			theta[LeftUpperLeg] -= per_move_speed * 0.03;
			theta[LeftLowerLeg] -= per_move_speed * 0.15;
			theta[RightUpperArm] -= per_move_speed * 0.03;
			theta[LeftUpperArm] += per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[RightUpperLeg] -= per_move_speed * 0.06;
			theta[LeftUpperLeg] += per_move_speed * 0.03;
			theta[LeftLowerLeg] += per_move_speed * 0.15;
			theta[RightUpperArm] += per_move_speed * 0.03;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向左运动
	void moveLeft() {
		updateMovePosition();

		per_move_speed = move_speed * program_speed * 70;
		theta[Torso] = 45;	// 躯体转动45度，下同理

		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.15;
			theta[RightUpperLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.15;
			theta[RightUpperLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.15;
			theta[RightUpperLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.15;
			theta[RightUpperLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向右运动
	void moveRight() {
		updateMovePosition();

		theta[Torso] = -45;

		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.15;
			theta[RightUpperLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.15;
			theta[RightUpperLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.15;
			theta[RightUpperLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.15;
			theta[RightUpperLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向左前运动
	void moveForwardLeft() {
		updateMovePosition();

		theta[Torso] = 45;

		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.2;
			theta[RightUpperLeg] -= per_move_speed * 0.2;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.2;
			theta[RightUpperLeg] += per_move_speed * 0.2;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.2;
			theta[RightUpperLeg] += per_move_speed * 0.2;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.2;
			theta[RightUpperLeg] -= per_move_speed * 0.2;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向右前运动
	void moveForwardRight() {
		updateMovePosition();

		theta[Torso] = -45;

		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.2;
			theta[RightUpperLeg] -= per_move_speed * 0.2;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.2;
			theta[RightUpperLeg] += per_move_speed * 0.2;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.2;
			theta[RightUpperLeg] += per_move_speed * 0.2;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.2;
			theta[RightUpperLeg] -= per_move_speed * 0.2;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向左后运动
	void moveBackwardLeft() {
		updateMovePosition();

		theta[Torso] = -45;

		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.06;
			theta[RightUpperLeg] -= per_move_speed * 0.03;
			theta[RightLowerLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.06;
			theta[RightUpperLeg] += per_move_speed * 0.03;
			theta[RightLowerLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[RightUpperLeg] += per_move_speed * 0.06;
			theta[LeftUpperLeg] -= per_move_speed * 0.03;
			theta[LeftLowerLeg] -= per_move_speed * 0.15;
			theta[RightUpperArm] -= per_move_speed * 0.03;
			theta[LeftUpperArm] += per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[RightUpperLeg] -= per_move_speed * 0.06;
			theta[LeftUpperLeg] += per_move_speed * 0.03;
			theta[LeftLowerLeg] += per_move_speed * 0.15;
			theta[RightUpperArm] += per_move_speed * 0.03;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人向右后运动
	void moveBackwardRight() {
		updateMovePosition();

		theta[Torso] = 45;

		if (move_time < PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] += per_move_speed * 0.06;
			theta[RightUpperLeg] -= per_move_speed * 0.03;
			theta[RightLowerLeg] -= per_move_speed * 0.15;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
			theta[RightUpperArm] += per_move_speed * 0.03;
		}
		else if (PER_MOVE_TIME / per_move_speed <= move_time && move_time < 2 * PER_MOVE_TIME / per_move_speed) {
			theta[LeftUpperLeg] -= per_move_speed * 0.06;
			theta[RightUpperLeg] += per_move_speed * 0.03;
			theta[RightLowerLeg] += per_move_speed * 0.15;
			theta[LeftUpperArm] += per_move_speed * 0.03;
			theta[RightUpperArm] -= per_move_speed * 0.03;
		}
		else if (2 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 3 * PER_MOVE_TIME / per_move_speed) {
			theta[RightUpperLeg] += per_move_speed * 0.06;
			theta[LeftUpperLeg] -= per_move_speed * 0.03;
			theta[LeftLowerLeg] -= per_move_speed * 0.15;
			theta[RightUpperArm] -= per_move_speed * 0.03;
			theta[LeftUpperArm] += per_move_speed * 0.03;
		}
		else if (3 * PER_MOVE_TIME / per_move_speed <= move_time && move_time < 4 * PER_MOVE_TIME / per_move_speed) {
			theta[RightUpperLeg] -= per_move_speed * 0.06;
			theta[LeftUpperLeg] += per_move_speed * 0.03;
			theta[LeftLowerLeg] += per_move_speed * 0.15;
			theta[RightUpperArm] += per_move_speed * 0.03;
			theta[LeftUpperArm] -= per_move_speed * 0.03;
		}
		reloadModelRobot();

		move_time += 1.0;
		if (move_time >= 4 * PER_MOVE_TIME / per_move_speed) {
			moveStop();
		}
	}

	// 机器人跳跃
	void moveJump() {
		float move_up_speed_y = 40 * move_speed;
		// 判断是否已经落地
		if (move_position.y + 0.0001 >= 0) {
			// 若还没落地则标记为真
			is_jump = true;
			// 响应起跳前的按键导致的位置变化
			updateMovePosition();
			// 用竖直上抛运动公式更新y坐标
			move_position.y = move_up_speed_y * move_up_time - 0.5 * 9.8 * move_up_time * move_up_time;
			// 时间往前推移
			move_up_time += 0.07 * program_speed;
		}
		else {
			// 落地后，为防止误差累加而更新y坐标为起跳前的坐标
			move_position.y = pre_move_position_y;
			// 目前跳跃时间更新为0
			move_up_time = 0;
			// 是否处于跳跃状态更新为假
			is_jump = false;
		}

		cam.ProcessView();

		// 重新加载模型
		reloadModelRobot();
	}

	// 机器人站立
	void moveStop() {
		// 设置为初始值
		theta[LeftUpperLeg] = theta[RightUpperLeg] = 180;
		theta[LeftLowerLeg] = theta[RightLowerLeg] = 0;
		theta[LeftUpperArm] = 180;
		theta[RightUpperArm] = 180;
		theta[Torso] = 0;
		move_time = 0;

		is_jump = false;
		// pre_move_position_y = move_position.y;
		move_up_time = 0;

		// 重新加载模型
		reloadModelRobot();
	}

	// 更新TPS相机视角下的移动方向，下同理
	void updateTPSMoveDirection() {
		// 通过相机的yaw值更新机器人的view_yaw值，并转为弧度制
		view_yaw = -cam.yaw - 90.0;
		float rad = view_yaw / 180 * M_PI;
		// 通过view_yaw的弧度值计算机器人的移动方向与移动法向
		move_direction = vec3(-sin(rad), 0, -cos(rad));
		move_direction_n = vec3(cos(rad), 0, -sin(rad));
		// 更新相机位置与相关向量
		cam.ProcessTPSView();
		// 重加载模型
		reloadModelRobot();
	}

	// 更新FPS相机视角下的移动方向，下同理
	void updateFPSMoveDirection() {
		view_yaw = -cam.yaw - 90.0;
		float rad = view_yaw / 180 * M_PI;
		move_direction = vec3(-sin(rad), 0, -cos(rad));
		move_direction_n = vec3(cos(rad), 0, -sin(rad));
		cam.ProcessFPSView();
		reloadModelRobot();
	}

	// 处理机器人速度恢复
	void ProcessRobotSpeedRecover() {
		// 时间推移
		robot_speed_recover_time += program_speed * 0.1;
		// 若时间到达，则恢复默认速度，并时间清0
		if (robot_speed_recover_time >= ROBOT_SPEED_RECOVER_MAX_TIME) {
			move_speed = ROBOT_SPEED;
			robot_speed_recover_time = 0;
		}
	}

	// 处理机器人扣血
	void ProcessRobotHpDown() {
		if (move_position.y + 0.001 >= 0) {		// 若机器人还未落地，则继续做竖直上抛运动
			move_position.y = robot_hp_down_moveup_speed * robot_hp_down_time - 0.5 * 9.8 * robot_hp_down_time * robot_hp_down_time;
			robot_hp_down_time += program_speed * 0.12;
		} else {	// 若已落地
			move_position.y = 0;		// 将y坐标置0，使机器人在地面上
			robot_hp_down_time = 0;		// 将动画时间置0
			robot_hp -= 0.1;			// 扣血10%
			is_robot_hp_down = false;	// 将扣血标记变量置假
			is_robot_get_item = false;	// 将获取道具标记变量置假
			
			if (robot_hp <= 0.0) {
				setRobotOver();
			}
		}
		// 重加载机器人模型，以呈现动画
		reloadModelRobot();
		// 重加载相机相应视角
		cam.ProcessView();
	}

	// 处理机器人获取道具
	void ProcessRobotGetItem() {
		// 获取发生了碰撞的道具下标
		int index = ModelGameItem::checkPointIsInItem(move_position);
		if (index == -1) {
			return;
		}

		// 将机器人获取道具标记变量置真
		is_robot_get_item = true;
		// 当前道具是 扣血 道具
		if (ModelGameItem::game_item_type[index] == ModelGameItem::GAME_ITEM_TYPE::ITEM_HP_DOWN) {
			is_robot_hp_down = true;			// 机器人扣血变量标记为真
		} 
		// 当前道具是 回血 道具
		else if (ModelGameItem::game_item_type[index] == ModelGameItem::GAME_ITEM_TYPE::ITEM_HP_UP) {
			robot_hp += 0.05;					// 回血
			is_robot_get_item = false;			// 标记变量置为假
		}
		// 当前道具是 加速 道具
		else if (ModelGameItem::game_item_type[index] == ModelGameItem::GAME_ITEM_TYPE::ITEM_SPEED_UP) {
			move_speed = ROBOT_SPEED * 1.5;		// 加速
			robot_speed_recover_time = 0;		// 恢复速度时间清0
			is_robot_get_item = false;			// 标记变量置为假
		}
		// 当前道具是 减速 道具
		else if (ModelGameItem::game_item_type[index] == ModelGameItem::GAME_ITEM_TYPE::ITEM_SPEED_DOWN) {
			move_speed = ROBOT_SPEED * 0.25;	// 减速
			robot_speed_recover_time = 0;		// 恢复速度时间清0
			is_robot_get_item = false;			// 标记变量置为假
		}
		// 重加载下标为index的道具
		ModelGameItem::reloadModelGameItemAt(index);
	}

	// 机器人血量随时间减少
	void ProcessRobotConsume() {
		robot_hp -= program_speed * ROBOT_HP_COMSUME_FACTOR;
		if (robot_hp <= 0.0) {
			setRobotOver();
		}
	}

	// 处理机器人抛手雷动画
	void ProcessRobotThrowBombProcess() {
		// 判定是否有方块进行消除，或手雷正在飞行，如果有则不继续处理
		if (!ModelGameBlock::game_block_removed_ls.empty() || ModelBomb::is_bomb_away) {
			is_robot_throw_bomb = false;
			return;
		}
		// 显示手雷模型
		ModelBomb::is_painted = true;
		/// 动画部分
		if (robot_throw_bomb_time <= ROBOT_THROW_BOMB_MAX_TIME) {
			theta[RightUpperArm] += 10 * program_speed;		// 增加右大臂角度值
			theta[RightLowerArm] += 5 * program_speed;		// 增加右小臂角度值
			robot_throw_bomb_time += program_speed * 1.5;	// 增加时间推移
			reloadModelRobot();								// 重加载模型
		}
	}

	// 处理丢出手雷
	void ProcessRobotThrowBombStop() {
		if (is_robot_throw_bomb == false) {
			return;
		}

		// 恢复原状态
		is_robot_throw_bomb = false;
		robot_throw_bomb_time = 0;
		theta[RightUpperArm] = 180;
		theta[RightLowerArm] = 0;
		reloadModelRobot();
		// 设置手雷处于飞行状态
		ModelBomb::setBombAway();
	}

	// 设置机器人死亡状态
	void setRobotOver() {
		is_robot_alive = false;
		cam.ProcessRobotOverView();
	}

	// 处理机器人的空闲回调事件
	void ProcessRobot() {
		if (ModelRobot::is_robot_alive == false) {
			return;
		}

		if (is_robot_throw_bomb) {
			ProcessRobotThrowBombProcess();
		} else if (!is_robot_get_item) {	// 若没有获取到物品，则尝试获取物品
			ProcessRobotGetItem();
		} else if (is_robot_hp_down) {		// 否则进行扣血动画
			ProcessRobotHpDown();
		}

		if (is_jump) {						// 若正在跳跃，则进行跳跃动画
			ModelRobot::moveJump();
		}

		ProcessRobotSpeedRecover();			// 速度恢复
		ModelHpBar::reloadModelHpBar();		// 重加载血量条
		ProcessRobotConsume();				// 血量随时间减少
	}

	// 初始化机器人各变量
	void initRobot() {
		robot_hp = 1.0;
		is_robot_alive = true;

		move_position = vec3(0, 0, 5 * ModelGround::block_scale);
		move_direction = vec3(0, 0, -1);
		move_direction_n = vec3(1, 0, 0);
		move_speed = 0.2;
		move_time = 0;
		robot_speed_up_time = 0;
		robot_speed_up_time = 0;
		robot_speed_down_time = 0;
		robot_speed_recover_time = 0;

		score = 0;

		robot_throw_bomb_time = 0;

		is_robot_hp_down = false;
		is_robot_throw_bomb = false;
		is_robot_get_item = false;
	}

	// ModelRobot::restartRobot 重启机器人
	void restartRobot() {
		// 初始化各模型
		initRobot();
		ModelGameBlock::initGameBlock();
		ModelBomb::initModelBomb();
		// 重加载机器人模型
		reloadModelRobot();
		// 切换相机视角
		SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		cam.current_view = Cam::Camera_View::FPS_VIEW;
		cam.ProcessView();
		// 重加载血量条
		ModelHpBar::reloadModelHpBar();
		// 重加载记分板分数
		ModelBoard::ProcessScoreUpdate();
	}
}

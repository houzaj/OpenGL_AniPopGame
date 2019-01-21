#ifndef __Model_H__
#define __Model_H__

extern double program_speed;

// 机器人
namespace ModelRobot {
	enum {
		Torso, Head1, Head2, RightUpperArm, RightLowerArm, LeftUpperArm, LeftLowerArm,
		RightUpperLeg, RightLowerLeg, LeftUpperLeg, LeftLowerLeg, NumJointAngles
	};
	extern GLfloat theta[NumJointAngles];
	extern bool is_robot_throw_bomb;

	extern bool is_jump;
	extern vec3 move_position;			// 机器人位置
	extern vec3 move_direction;			// 机器人朝向
	extern vec3 move_direction_n;		// 机器人右向

	extern const float LOWER_LEG_HEIGHT;
	extern const float UPPER_LEG_HEIGHT;
	extern const float TORSO_HEIGHT;

	extern float view_yaw;

	extern float robot_hp;
	extern bool is_robot_alive;

	extern int score;

	void loadModelRobot();
	void reloadModelRobot();
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void moveForwardLeft();
	void moveForwardRight();
	void moveBackwardLeft();
	void moveBackwardRight();
	void moveStop();
	void moveJump();
	void updateTPSMoveDirection();
	void updateFPSMoveDirection();

	void ProcessRobot();

	void ProcessRobotThrowBombProcess();
	void ProcessRobotThrowBombStop();

	bool checkCanMove();

	void setRobotOver();
	void restartRobot();
}

// 地面
namespace ModelGround {
	extern float block_scale;

	void loadModelGound();
}

// 太阳
namespace ModelSun {
	void loadModelSun();
	void reloadModelSun();
}

// 游戏方块
namespace ModelGameBlock {
	extern const int NUMS_GAME_BLOCKS_TYPE;
	extern const int NUMS_GAME_BLOCKS;
	extern int game_block_type[];
	extern float game_block_scale;
	extern float game_block_offset_y;
	extern bool is_game_block_removed;
	extern std::vector<int> game_block_removed_ls;

	void loadModelGameBlock();
	void reloadModelGameBlockAt(int x, int z, vec3 color = vec3(1.0, 1.0, 1.0), float rotation_y = 0, float scale = 1, vec3 translate_vec = vec3(0.0, 0.0, 0.0));
	void updateGameBlockPointed();
	void updateGameBlockSelected();
	void ProcessCoreGame();

	void updateGameBlockIsRemoved();
	bool updateGameBlockIsSwaped();
	void ProcessGameBlockRemove();
	void ProcessGameBlockSwap();

	int checkPointIsInGameBlock(vec3 position, float delta);
	bool checkPointIsInGameBlock(vec3 position, float delta, int index);

	int getIndexGameBlockPointed();

	int getIndex(int x, int z);
	void getXZ(int index, int& x, int& z);

	void initGameBlock();
}

// 游戏道具
namespace ModelGameItem {
	// 道具类型
	enum GAME_ITEM_TYPE{
		ITEM_SPEED_UP,		// 加速 
		ITEM_SPEED_DOWN,	// 减速
		ITEM_HP_UP,			// 回血
		ITEM_HP_DOWN,		// 扣血
		NUMS_MODEL_GAME_ITEM_TYPE
	};
	extern GAME_ITEM_TYPE game_item_type[];

	void loadModelGameItem();
	void reloadModelGameItemAt(int index);
	void updateGameItemAt(int index);

	int checkPointIsInItem(vec3 position);
}

// 血量条
namespace ModelHpBar {
	void loadModelHpBar();
	void reloadModelHpBar();
}

// 记分板
namespace ModelBoard {
	void loadModelBoard();
	void reloadModelBoard();
	void ProcessScoreUpdate();
}

// 手雷
namespace ModelBomb {
	extern bool is_painted;
	extern bool is_bomb_away;
	extern int bomb_nums;

	void loadModelBomb();
	void reloadModelBomb(const mat4& model_matrix);
	void getPointIsCracked(vec3& out_point, bool& ok);
	void setBombAway();
	void ProcessBombAway();
	void updateBombNums();

	void initModelBomb();
}

// 天空盒
namespace ModelSkyBox {
	extern void loadModelSkyBox();
}

#endif // !__Model_H__

#ifndef __Model_H__
#define __Model_H__

extern double program_speed;

// ������
namespace ModelRobot {
	enum {
		Torso, Head1, Head2, RightUpperArm, RightLowerArm, LeftUpperArm, LeftLowerArm,
		RightUpperLeg, RightLowerLeg, LeftUpperLeg, LeftLowerLeg, NumJointAngles
	};
	extern GLfloat theta[NumJointAngles];
	extern bool is_robot_throw_bomb;

	extern bool is_jump;
	extern vec3 move_position;			// ������λ��
	extern vec3 move_direction;			// �����˳���
	extern vec3 move_direction_n;		// ����������

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

// ����
namespace ModelGround {
	extern float block_scale;

	void loadModelGound();
}

// ̫��
namespace ModelSun {
	void loadModelSun();
	void reloadModelSun();
}

// ��Ϸ����
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

// ��Ϸ����
namespace ModelGameItem {
	// ��������
	enum GAME_ITEM_TYPE{
		ITEM_SPEED_UP,		// ���� 
		ITEM_SPEED_DOWN,	// ����
		ITEM_HP_UP,			// ��Ѫ
		ITEM_HP_DOWN,		// ��Ѫ
		NUMS_MODEL_GAME_ITEM_TYPE
	};
	extern GAME_ITEM_TYPE game_item_type[];

	void loadModelGameItem();
	void reloadModelGameItemAt(int index);
	void updateGameItemAt(int index);

	int checkPointIsInItem(vec3 position);
}

// Ѫ����
namespace ModelHpBar {
	void loadModelHpBar();
	void reloadModelHpBar();
}

// �Ƿְ�
namespace ModelBoard {
	void loadModelBoard();
	void reloadModelBoard();
	void ProcessScoreUpdate();
}

// ����
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

// ��պ�
namespace ModelSkyBox {
	extern void loadModelSkyBox();
}

#endif // !__Model_H__

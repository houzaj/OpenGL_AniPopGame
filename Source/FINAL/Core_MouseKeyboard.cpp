#include "Auxiliary_H.h"
#include "Core_MouseKeyboard.h"

// 按键标记数组
bool state_key[128];

namespace GameMouseKeyboard {
	// 记录之前的组合按键对应值
	int move_key_state;

	// GameMouseKeyboard::getState 返回目前的组合按键对应值
	int getState(const char* button) {
		int ret = 0;
		for (int i = 0; i < 4; i++) {
			ret <<= 1;
			if (state_key[button[i]]) {		// 若此键被按下，则置1
				ret ^= 1;
			}
		}
		return ret;
	}
}


namespace CoreMouseKeyboard {
	// 初始化，绑定各函数
	void init() {
		memset(state_key, false, sizeof(state_key));
		GameMouseKeyboard::move_key_state = 0;
		glutPassiveMotionFunc(mouseFunc);
		glutMouseFunc(mouseClickFunc);
		glutKeyboardFunc(keyboardDown);
		glutKeyboardUpFunc(keyboardUp);
		glutSpecialFunc(GameSpecialKeyFunc);
	}

	// 游戏鼠标功能
	void GameMouseFunc(int x, int y) {
		// 当机器人处于死亡状态下时，不响应，下同
		if (ModelRobot::is_robot_alive == false) {
			return;
		}

		// 根据当前视角调用不同的摄像机视角
		if (cam.current_view == Cam::Camera_View::FPS_VIEW) {
			GameMouseFunc_FPS(x, y);
		} else if (cam.current_view == Cam::Camera_View::TPS_VIEW) {
			GameMouseFunc_TPS(x, y);
		} else if (cam.current_view == Cam::Camera_View::GOD_VIEW) {
			GameMouseFunc_GOD(x, y);
		}

		// 更新目前指向的游戏方块
		ModelGameBlock::updateGameBlockPointed();
	}

	// 游戏键盘功能
	void GameKeyboardFunc() {
		if (ModelRobot::is_robot_alive == false) {
			return;
		}

		if (cam.current_view == Cam::Camera_View::FPS_VIEW) {
			GameKeyboardFunc_FPS();
		}
		else if (cam.current_view == Cam::Camera_View::TPS_VIEW) {
			GameKeyboardFunc_TPS();
		}
		else if (cam.current_view == Cam::Camera_View::GOD_VIEW) {
			GameKeyboardFunc_GOD();
		}
		ModelSun::reloadModelSun();
	}

	// 鼠标引起的相机视角变化
	void GameMouseFunc_FPS(int x, int y) {
		// 通过与窗口中心的位置之间的差值计算xoffset, yoffset
		GLfloat xoffset = x - (WIN_X / 2);
		GLfloat yoffset = (WIN_Y / 2) - y;

		// 当差值的绝对值超过1时，进行移动
		if (xoffset > 1 || xoffset < -1 || yoffset > 1 || yoffset < -1) {
			// 变换相机视角
			cam.ProcessMouseMovement(xoffset, yoffset);
			ModelRobot::updateFPSMoveDirection();
			SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		}

		glutPostRedisplay();
	}

	// 鼠标引起的TPS相机视角变化
	void GameMouseFunc_TPS(int x, int y) {
		// 通过与窗口中心的位置之间的差值计算xoffset, yoffset
		GLfloat xoffset = x - (WIN_X / 2);
		GLfloat yoffset = (WIN_Y / 2) - y;

		// 当差值的绝对值超过1时，进行移动
		if (xoffset > 1 || xoffset < -1 || yoffset > 1 || yoffset < -1) {
			// 变换相机视角
			cam.ProcessMouseMovement(xoffset, yoffset);
			ModelRobot::updateTPSMoveDirection();
			SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		}

		glutPostRedisplay();
	}

	// 鼠标引起的上帝视角相机视角变化
	void GameMouseFunc_GOD(int x, int y) {
		// 通过与窗口中心的位置之间的差值计算xoffset, yoffset
		GLfloat xoffset = x - (WIN_X / 2);
		GLfloat yoffset = (WIN_Y / 2) - y;

		// 当差值的绝对值超过1时，进行移动
		if (xoffset > 1 || xoffset < -1 || yoffset > 1 || yoffset < -1) {
			// 变换相机视角
			cam.ProcessMouseMovement(xoffset, yoffset);
			SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		}

		glutPostRedisplay();
	}


	// 按下按键时响应事件
	void GameKeyboardDown(unsigned char key, int x, int y) {
		if (key == 033 || key == 'q') {
			exit(EXIT_SUCCESS);
			return;
		}

		// 可见范围内的按键标记为真
		if (32 <= key && key < 127) {
			state_key[(int)key] = true;
		}
	}

	// CoreMouseKeyboard::GameKeyboardUp 松开按键时响应事件
	void GameKeyboardUp(unsigned char key, int x, int y) {
		// 可见范围内的按键标记为假
		if (32 <= key && key < 127) {
			state_key[(int)key] = false;
		}
	}

	// CoreMouseKeyboard::GameKeyboardFunc_GOD
	void GameKeyboardFunc_GOD() {
		// 判定按键并使相机进行相应移动
		if (state_key['w']) {
			cam.ProcessKeyboard(Cam::Camera_Movement::FORWARD);
		}
		if (state_key['s']) {
			cam.ProcessKeyboard(Cam::Camera_Movement::BACKWARD);
		}
		if (state_key['a']) {
			cam.ProcessKeyboard(Cam::Camera_Movement::LEFT);
		}
		if (state_key['d']) {
			cam.ProcessKeyboard(Cam::Camera_Movement::RIGHT);
		}

		// 按下空格时，将跳跃标记状态置为真
		if (ModelRobot::checkCanMove()) {
			if (state_key[' ']) {
				// 按下空格时，将跳跃标记状态置为真
				ModelRobot::is_jump = true;
			}
		}

		// 当机器人没有处于跳跃状态时
		if (ModelRobot::checkCanMove()) {
			// 若当前按键组合与之前的不一致，则立定
			if (GameMouseKeyboard::getState("ikjl") != GameMouseKeyboard::move_key_state) {
				GameMouseKeyboard::move_key_state = GameMouseKeyboard::getState("ikjl");
				ModelRobot::moveStop();
			}

			// 不同的按键组合对应不同的动作
			if (GameMouseKeyboard::move_key_state == 8) {
				ModelRobot::moveForward();
			}
			else if (GameMouseKeyboard::move_key_state == 4) {
				ModelRobot::moveBackward();
			}
			else if (GameMouseKeyboard::move_key_state == 2) {
				ModelRobot::moveLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 1) {
				ModelRobot::moveRight();
			}
			else if (GameMouseKeyboard::move_key_state == 10) {
				ModelRobot::moveForwardLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 9) {
				ModelRobot::moveForwardRight();
			}
			else if (GameMouseKeyboard::move_key_state == 6) {
				ModelRobot::moveBackwardLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 5) {
				ModelRobot::moveBackwardRight();
			}
		}
	}

	// 在TPS视角下响应键盘按键
	void GameKeyboardFunc_TPS() {
		// 按下空格时，将跳跃标记状态置为真
		if (ModelRobot::checkCanMove()) {
			if (state_key[' ']) {
				// 按下空格时，将跳跃标记状态置为真
				ModelRobot::is_jump = true;
			}
		}

		// 当机器人没有处于跳跃状态时
		if (ModelRobot::checkCanMove()) {
			// 若当前按键组合与之前的不一致，则立定
			if (GameMouseKeyboard::getState("wsad") != GameMouseKeyboard::move_key_state) {
				GameMouseKeyboard::move_key_state = GameMouseKeyboard::getState("wsad");
				ModelRobot::moveStop();
			}

			// 不同的按键组合对应不同的动作
			if (GameMouseKeyboard::move_key_state == 8) {
				ModelRobot::moveForward();
			}
			else if (GameMouseKeyboard::move_key_state == 4) {
				ModelRobot::moveBackward();
			}
			else if (GameMouseKeyboard::move_key_state == 2) {
				ModelRobot::moveLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 1) {
				ModelRobot::moveRight();
			}
			else if (GameMouseKeyboard::move_key_state == 10) {
				ModelRobot::moveForwardLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 9) {
				ModelRobot::moveForwardRight();
			}
			else if (GameMouseKeyboard::move_key_state == 6) {
				ModelRobot::moveBackwardLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 5) {
				ModelRobot::moveBackwardRight();
			}
		}

		// 更新机器人移动方向，下同
		ModelRobot::updateTPSMoveDirection();
	}

	// 响应FPS视角下的键盘按键 
	void GameKeyboardFunc_FPS() {
		if (ModelRobot::checkCanMove()) {
			if (state_key[' ']) {
				// 按下空格时，将跳跃标记状态置为真
				ModelRobot::is_jump = true;
			}
		}

		// 当机器人没有处于跳跃状态时
		if (ModelRobot::checkCanMove()) {
			// 若当前按键组合与之前的不一致，则立定
			if (GameMouseKeyboard::getState("wsad") != GameMouseKeyboard::move_key_state) {
				GameMouseKeyboard::move_key_state = GameMouseKeyboard::getState("wsad");
				ModelRobot::moveStop();
			}

			// 不同的按键组合对应不同的动作
			if (GameMouseKeyboard::move_key_state == 8) {
				ModelRobot::moveForward();
			}
			else if (GameMouseKeyboard::move_key_state == 4) {
				ModelRobot::moveBackward();
			}
			else if (GameMouseKeyboard::move_key_state == 2) {
				ModelRobot::moveLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 1) {
				ModelRobot::moveRight();
			}
			else if (GameMouseKeyboard::move_key_state == 10) {
				ModelRobot::moveForwardLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 9) {
				ModelRobot::moveForwardRight();
			}
			else if (GameMouseKeyboard::move_key_state == 6) {
				ModelRobot::moveBackwardLeft();
			}
			else if (GameMouseKeyboard::move_key_state == 5) {
				ModelRobot::moveBackwardRight();
			}
		}
		ModelRobot::updateFPSMoveDirection();
	}

	// 响应特殊按键
	void GameSpecialKeyFunc(int key, int x, int y) {
		if (key == GLUT_KEY_F3) {	// 按F3时切换视角
			cam.changeView();
		} else if(key == GLUT_KEY_F7 && ModelRobot::is_robot_alive == false) {	// 游戏结束时按下F7重启游戏
			ModelRobot::restartRobot();
		} else if(key == GLUT_KEY_F12){		// 按下F12键自杀
			ModelRobot::robot_hp = -1;
			ModelRobot::setRobotOver();
		}
	}

	// 响应鼠标点击事件
	void mouseClickFunc(int button, int state, int x, int y) {
		if (ModelRobot::is_robot_alive == false) {
			return;
		}
		
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			// 按下鼠标左键时尝试更新被选择的方块的效果
			ModelGameBlock::updateGameBlockSelected();
		} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && ModelBomb::bomb_nums > 0) {
			// 按下鼠标右键且此时有手雷时，尝试进行扔手雷动画
			ModelRobot::is_robot_throw_bomb = true;
		} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
			// 松开鼠标右键时尝试扔出手雷
			ModelRobot::ProcessRobotThrowBombStop();
		}
	}

	// 绑定鼠标功能函数
	void mouseFunc(int x, int y) {
		GameMouseFunc(x, y);
	}

	// 绑定键盘功能函数
	void keyboardDown(unsigned char key, int x, int y) {
		GameKeyboardDown(key, x, y);
	}

	void keyboardUp(unsigned char key, int x, int y) {
		GameKeyboardUp(key, x, y);
	}


}

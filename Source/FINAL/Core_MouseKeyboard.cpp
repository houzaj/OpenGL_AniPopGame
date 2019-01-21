#include "Auxiliary_H.h"
#include "Core_MouseKeyboard.h"

// �����������
bool state_key[128];

namespace GameMouseKeyboard {
	// ��¼֮ǰ����ϰ�����Ӧֵ
	int move_key_state;

	// GameMouseKeyboard::getState ����Ŀǰ����ϰ�����Ӧֵ
	int getState(const char* button) {
		int ret = 0;
		for (int i = 0; i < 4; i++) {
			ret <<= 1;
			if (state_key[button[i]]) {		// ���˼������£�����1
				ret ^= 1;
			}
		}
		return ret;
	}
}


namespace CoreMouseKeyboard {
	// ��ʼ�����󶨸�����
	void init() {
		memset(state_key, false, sizeof(state_key));
		GameMouseKeyboard::move_key_state = 0;
		glutPassiveMotionFunc(mouseFunc);
		glutMouseFunc(mouseClickFunc);
		glutKeyboardFunc(keyboardDown);
		glutKeyboardUpFunc(keyboardUp);
		glutSpecialFunc(GameSpecialKeyFunc);
	}

	// ��Ϸ��깦��
	void GameMouseFunc(int x, int y) {
		// �������˴�������״̬��ʱ������Ӧ����ͬ
		if (ModelRobot::is_robot_alive == false) {
			return;
		}

		// ���ݵ�ǰ�ӽǵ��ò�ͬ��������ӽ�
		if (cam.current_view == Cam::Camera_View::FPS_VIEW) {
			GameMouseFunc_FPS(x, y);
		} else if (cam.current_view == Cam::Camera_View::TPS_VIEW) {
			GameMouseFunc_TPS(x, y);
		} else if (cam.current_view == Cam::Camera_View::GOD_VIEW) {
			GameMouseFunc_GOD(x, y);
		}

		// ����Ŀǰָ�����Ϸ����
		ModelGameBlock::updateGameBlockPointed();
	}

	// ��Ϸ���̹���
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

	// ������������ӽǱ仯
	void GameMouseFunc_FPS(int x, int y) {
		// ͨ���봰�����ĵ�λ��֮��Ĳ�ֵ����xoffset, yoffset
		GLfloat xoffset = x - (WIN_X / 2);
		GLfloat yoffset = (WIN_Y / 2) - y;

		// ����ֵ�ľ���ֵ����1ʱ�������ƶ�
		if (xoffset > 1 || xoffset < -1 || yoffset > 1 || yoffset < -1) {
			// �任����ӽ�
			cam.ProcessMouseMovement(xoffset, yoffset);
			ModelRobot::updateFPSMoveDirection();
			SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		}

		glutPostRedisplay();
	}

	// ��������TPS����ӽǱ仯
	void GameMouseFunc_TPS(int x, int y) {
		// ͨ���봰�����ĵ�λ��֮��Ĳ�ֵ����xoffset, yoffset
		GLfloat xoffset = x - (WIN_X / 2);
		GLfloat yoffset = (WIN_Y / 2) - y;

		// ����ֵ�ľ���ֵ����1ʱ�������ƶ�
		if (xoffset > 1 || xoffset < -1 || yoffset > 1 || yoffset < -1) {
			// �任����ӽ�
			cam.ProcessMouseMovement(xoffset, yoffset);
			ModelRobot::updateTPSMoveDirection();
			SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		}

		glutPostRedisplay();
	}

	// ���������ϵ��ӽ�����ӽǱ仯
	void GameMouseFunc_GOD(int x, int y) {
		// ͨ���봰�����ĵ�λ��֮��Ĳ�ֵ����xoffset, yoffset
		GLfloat xoffset = x - (WIN_X / 2);
		GLfloat yoffset = (WIN_Y / 2) - y;

		// ����ֵ�ľ���ֵ����1ʱ�������ƶ�
		if (xoffset > 1 || xoffset < -1 || yoffset > 1 || yoffset < -1) {
			// �任����ӽ�
			cam.ProcessMouseMovement(xoffset, yoffset);
			SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
		}

		glutPostRedisplay();
	}


	// ���°���ʱ��Ӧ�¼�
	void GameKeyboardDown(unsigned char key, int x, int y) {
		if (key == 033 || key == 'q') {
			exit(EXIT_SUCCESS);
			return;
		}

		// �ɼ���Χ�ڵİ������Ϊ��
		if (32 <= key && key < 127) {
			state_key[(int)key] = true;
		}
	}

	// CoreMouseKeyboard::GameKeyboardUp �ɿ�����ʱ��Ӧ�¼�
	void GameKeyboardUp(unsigned char key, int x, int y) {
		// �ɼ���Χ�ڵİ������Ϊ��
		if (32 <= key && key < 127) {
			state_key[(int)key] = false;
		}
	}

	// CoreMouseKeyboard::GameKeyboardFunc_GOD
	void GameKeyboardFunc_GOD() {
		// �ж�������ʹ���������Ӧ�ƶ�
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

		// ���¿ո�ʱ������Ծ���״̬��Ϊ��
		if (ModelRobot::checkCanMove()) {
			if (state_key[' ']) {
				// ���¿ո�ʱ������Ծ���״̬��Ϊ��
				ModelRobot::is_jump = true;
			}
		}

		// ��������û�д�����Ծ״̬ʱ
		if (ModelRobot::checkCanMove()) {
			// ����ǰ���������֮ǰ�Ĳ�һ�£�������
			if (GameMouseKeyboard::getState("ikjl") != GameMouseKeyboard::move_key_state) {
				GameMouseKeyboard::move_key_state = GameMouseKeyboard::getState("ikjl");
				ModelRobot::moveStop();
			}

			// ��ͬ�İ�����϶�Ӧ��ͬ�Ķ���
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

	// ��TPS�ӽ�����Ӧ���̰���
	void GameKeyboardFunc_TPS() {
		// ���¿ո�ʱ������Ծ���״̬��Ϊ��
		if (ModelRobot::checkCanMove()) {
			if (state_key[' ']) {
				// ���¿ո�ʱ������Ծ���״̬��Ϊ��
				ModelRobot::is_jump = true;
			}
		}

		// ��������û�д�����Ծ״̬ʱ
		if (ModelRobot::checkCanMove()) {
			// ����ǰ���������֮ǰ�Ĳ�һ�£�������
			if (GameMouseKeyboard::getState("wsad") != GameMouseKeyboard::move_key_state) {
				GameMouseKeyboard::move_key_state = GameMouseKeyboard::getState("wsad");
				ModelRobot::moveStop();
			}

			// ��ͬ�İ�����϶�Ӧ��ͬ�Ķ���
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

		// ���»������ƶ�������ͬ
		ModelRobot::updateTPSMoveDirection();
	}

	// ��ӦFPS�ӽ��µļ��̰��� 
	void GameKeyboardFunc_FPS() {
		if (ModelRobot::checkCanMove()) {
			if (state_key[' ']) {
				// ���¿ո�ʱ������Ծ���״̬��Ϊ��
				ModelRobot::is_jump = true;
			}
		}

		// ��������û�д�����Ծ״̬ʱ
		if (ModelRobot::checkCanMove()) {
			// ����ǰ���������֮ǰ�Ĳ�һ�£�������
			if (GameMouseKeyboard::getState("wsad") != GameMouseKeyboard::move_key_state) {
				GameMouseKeyboard::move_key_state = GameMouseKeyboard::getState("wsad");
				ModelRobot::moveStop();
			}

			// ��ͬ�İ�����϶�Ӧ��ͬ�Ķ���
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

	// ��Ӧ���ⰴ��
	void GameSpecialKeyFunc(int key, int x, int y) {
		if (key == GLUT_KEY_F3) {	// ��F3ʱ�л��ӽ�
			cam.changeView();
		} else if(key == GLUT_KEY_F7 && ModelRobot::is_robot_alive == false) {	// ��Ϸ����ʱ����F7������Ϸ
			ModelRobot::restartRobot();
		} else if(key == GLUT_KEY_F12){		// ����F12����ɱ
			ModelRobot::robot_hp = -1;
			ModelRobot::setRobotOver();
		}
	}

	// ��Ӧ������¼�
	void mouseClickFunc(int button, int state, int x, int y) {
		if (ModelRobot::is_robot_alive == false) {
			return;
		}
		
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			// ����������ʱ���Ը��±�ѡ��ķ����Ч��
			ModelGameBlock::updateGameBlockSelected();
		} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && ModelBomb::bomb_nums > 0) {
			// ��������Ҽ��Ҵ�ʱ������ʱ�����Խ��������׶���
			ModelRobot::is_robot_throw_bomb = true;
		} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
			// �ɿ�����Ҽ�ʱ�����ӳ�����
			ModelRobot::ProcessRobotThrowBombStop();
		}
	}

	// ����깦�ܺ���
	void mouseFunc(int x, int y) {
		GameMouseFunc(x, y);
	}

	// �󶨼��̹��ܺ���
	void keyboardDown(unsigned char key, int x, int y) {
		GameKeyboardDown(key, x, y);
	}

	void keyboardUp(unsigned char key, int x, int y) {
		GameKeyboardUp(key, x, y);
	}


}

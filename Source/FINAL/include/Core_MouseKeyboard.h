#ifndef __CORE_MOUSEKEYBOARD_H__
#define __CORE_MOUSEKEYBOARD_H__

#include "Auxiliary_H.h"

namespace GameMouseKeyboard {
	// ��¼֮ǰ�İ�����Ϸ���
	extern int move_key_state;
}

namespace CoreMouseKeyboard {
	// ��ʼ��
	void init();
	// ��Ϸ��깦��
	void GameMouseFunc(int x, int y);
	void GameMouseFunc_TPS(int x, int y);
	void GameMouseFunc_FPS(int x, int y);
	void GameMouseFunc_GOD(int x, int y);
	// ��Ϸ���̹���
	void GameKeyboardDown(unsigned char key, int x, int y);
	void GameKeyboardUp(unsigned char key, int x, int y);
	void GameKeyboardFunc();
	void GameKeyboardFunc_TPS();
	void GameKeyboardFunc_FPS();
	void GameKeyboardFunc_GOD();
	
	// ȫ�ּ��̹���
	void keyboardUp(unsigned char key, int x, int y);
	void keyboardDown(unsigned char key, int x, int y);
	void GameSpecialKeyFunc(int key, int x, int y);
	// ȫ����깦��
	void mouseFunc(int x, int y);
	void mouseClickFunc(int button, int state, int x, int y);
}

#endif // !__CORE_MOUSEKEYBOARD_H__

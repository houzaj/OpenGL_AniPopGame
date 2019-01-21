#ifndef __CORE_MOUSEKEYBOARD_H__
#define __CORE_MOUSEKEYBOARD_H__

#include "Auxiliary_H.h"

namespace GameMouseKeyboard {
	// 记录之前的按键组合方案
	extern int move_key_state;
}

namespace CoreMouseKeyboard {
	// 初始化
	void init();
	// 游戏鼠标功能
	void GameMouseFunc(int x, int y);
	void GameMouseFunc_TPS(int x, int y);
	void GameMouseFunc_FPS(int x, int y);
	void GameMouseFunc_GOD(int x, int y);
	// 游戏键盘功能
	void GameKeyboardDown(unsigned char key, int x, int y);
	void GameKeyboardUp(unsigned char key, int x, int y);
	void GameKeyboardFunc();
	void GameKeyboardFunc_TPS();
	void GameKeyboardFunc_FPS();
	void GameKeyboardFunc_GOD();
	
	// 全局键盘功能
	void keyboardUp(unsigned char key, int x, int y);
	void keyboardDown(unsigned char key, int x, int y);
	void GameSpecialKeyFunc(int key, int x, int y);
	// 全局鼠标功能
	void mouseFunc(int x, int y);
	void mouseClickFunc(int button, int state, int x, int y);
}

#endif // !__CORE_MOUSEKEYBOARD_H__

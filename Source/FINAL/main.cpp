#include "Auxiliary_H.h"

#include <cstdlib>
#include <iostream>

using namespace std;

const int WIN_X = 600, WIN_Y = 600;
int last_frame;						// 上一帧的时间
int iterate_frame = 0;				// 迭代器
double sum_delta_frame = 0;			// 帧之间的运行时间
double program_speed = 0.13;		// 游戏速度

Cam::Camera cam;					// 摄像机类


// OpenGL 初始化
void init() {
	// 初始化最后一帧为0
    last_frame = 0;

	// 初始化鼠标键盘方法的重置与绑定等
    CoreMouseKeyboard::init();
	// 初始化图形绘制器类
    CoreMesh::painter.init();

	// 初始化相机
    cam.initShaderCamera();
    // 初始化光源
	CoreLight::initShaderLight();

	// 初始化各模型
    ModelGround::loadModelGound();
    ModelRobot::loadModelRobot();
    ModelSun::loadModelSun();
    ModelGameBlock::loadModelGameBlock();
	ModelGameItem::loadModelGameItem();
    ModelHpBar::loadModelHpBar();
    ModelBoard::loadModelBoard();
	ModelBomb::loadModelBomb();
	ModelSkyBox::loadModelSkyBox();
	
	// 初始化纹理与各模型的初始化着色器方法
    CoreMesh::painter.initMeshShader();

    // 黑色背景
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // OpenGL相应状态设置
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}


// 渲染
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 相机模型矩阵的相关渲染
    cam.displayShaderCamera();
	// 光照效果
    CoreLight::displayShaderLight();
    // 各模型的渲染
	CoreMesh::painter.displayMeshShader();


    glutSwapBuffers();
}


// 重新设置窗口
void reshape(GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);
}


// 空闲回调函数
void idle(void) {
    // 计算两次调用函数中间的差值，只取前1000次求近似平均，得出运行速度
	if (iterate_frame < 1000) {
        int cur_frame = glutGet(GLUT_ELAPSED_TIME);
        sum_delta_frame += (cur_frame - last_frame) / 100.0;
        last_frame = cur_frame;
        iterate_frame++;
    } else if (iterate_frame == 1000) {
        program_speed = sum_delta_frame / 1200;
        iterate_frame++;
    }

	// 处理键盘空闲回调事件
    CoreMouseKeyboard::GameKeyboardFunc();
    // 处理方块的空闲回调事件
	ModelGameBlock::ProcessCoreGame();
    // 处理机器人的空闲回调事件
	ModelRobot::ProcessRobot();
	// 处理手雷飞行动画
	ModelBomb::ProcessBombAway();

    glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WIN_X, WIN_Y);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
    
    glutCreateWindow("OpenGL_AniPop");
	glewExperimental = GL_TRUE;

    glewInit();
    init();

	// 设置鼠标位于窗口中心
    SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
    // 设置鼠标样式为十字准心
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutIdleFunc(idle);

    glutMainLoop();

	// 释放内存
    CoreMesh::painter.destroy();

    return 0;
}

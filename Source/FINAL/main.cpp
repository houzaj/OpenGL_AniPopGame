#include "Auxiliary_H.h"

#include <cstdlib>
#include <iostream>

using namespace std;

const int WIN_X = 600, WIN_Y = 600;
int last_frame;						// ��һ֡��ʱ��
int iterate_frame = 0;				// ������
double sum_delta_frame = 0;			// ֮֡�������ʱ��
double program_speed = 0.13;		// ��Ϸ�ٶ�

Cam::Camera cam;					// �������


// OpenGL ��ʼ��
void init() {
	// ��ʼ�����һ֡Ϊ0
    last_frame = 0;

	// ��ʼ�������̷�����������󶨵�
    CoreMouseKeyboard::init();
	// ��ʼ��ͼ�λ�������
    CoreMesh::painter.init();

	// ��ʼ�����
    cam.initShaderCamera();
    // ��ʼ����Դ
	CoreLight::initShaderLight();

	// ��ʼ����ģ��
    ModelGround::loadModelGound();
    ModelRobot::loadModelRobot();
    ModelSun::loadModelSun();
    ModelGameBlock::loadModelGameBlock();
	ModelGameItem::loadModelGameItem();
    ModelHpBar::loadModelHpBar();
    ModelBoard::loadModelBoard();
	ModelBomb::loadModelBomb();
	ModelSkyBox::loadModelSkyBox();
	
	// ��ʼ���������ģ�͵ĳ�ʼ����ɫ������
    CoreMesh::painter.initMeshShader();

    // ��ɫ����
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // OpenGL��Ӧ״̬����
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}


// ��Ⱦ
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���ģ�;���������Ⱦ
    cam.displayShaderCamera();
	// ����Ч��
    CoreLight::displayShaderLight();
    // ��ģ�͵���Ⱦ
	CoreMesh::painter.displayMeshShader();


    glutSwapBuffers();
}


// �������ô���
void reshape(GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);
}


// ���лص�����
void idle(void) {
    // �������ε��ú����м�Ĳ�ֵ��ֻȡǰ1000�������ƽ�����ó������ٶ�
	if (iterate_frame < 1000) {
        int cur_frame = glutGet(GLUT_ELAPSED_TIME);
        sum_delta_frame += (cur_frame - last_frame) / 100.0;
        last_frame = cur_frame;
        iterate_frame++;
    } else if (iterate_frame == 1000) {
        program_speed = sum_delta_frame / 1200;
        iterate_frame++;
    }

	// ������̿��лص��¼�
    CoreMouseKeyboard::GameKeyboardFunc();
    // ������Ŀ��лص��¼�
	ModelGameBlock::ProcessCoreGame();
    // ��������˵Ŀ��лص��¼�
	ModelRobot::ProcessRobot();
	// �������׷��ж���
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

	// �������λ�ڴ�������
    SetCursorPos(WIN_X / 2 + glutGet(GLUT_WINDOW_X), WIN_Y / 2 + glutGet(GLUT_WINDOW_Y));
    // ���������ʽΪʮ��׼��
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutIdleFunc(idle);

    glutMainLoop();

	// �ͷ��ڴ�
    CoreMesh::painter.destroy();

    return 0;
}

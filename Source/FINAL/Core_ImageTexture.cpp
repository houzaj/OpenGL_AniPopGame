// ͼƬ��������
#include "Auxiliary_H.h"
#include "Core_ImageTexture.h"

namespace CoreImageTexture {
	const char* image_file_name[TOTAL_NUMS] = {
		"texture/BlockGround.jpg",
		"texture/BlockGroundEdge.jpg",
		"texture/Huaji.png",
		"texture/QuestionBox.jpg",
		"texture/ScoreBoard.jpg",
		"texture/GameBlock_Yellow.png",
		"texture/GameBlock_Red.png",
		"texture/GameBlock_Green.png",
		"texture/GameBlock_Purple.png",
		"texture/GameBlock_Pink.png",
		"texture/Bomb.png",
		"texture/ciya.jpg",
		"texture/Robot_TORSO.png",
		"texture/Robot_ARM.png",
		"texture/Robot_LEG.png",
		"texture/Sky_Up.png",
		"texture/Sky_Down.png",
		"texture/Sky_Left.png",
		"texture/Sky_Right.png",
		"texture/Sky_Front.png",
		"texture/Sky_Back.png"
	};
	// ����
	GLuint gl_texture[TOTAL_NUMS];

	// CoreImageTexture::initImageTexture ��ʼ������
	void initImageTexture() {
		// һ���Դ�������ͼƬ
		for (int i = 0; i < TOTAL_NUMS; i++) {
			// ��������
			glGenTextures(1, &gl_texture[i]);
			// ������
			glBindTexture(GL_TEXTURE_2D, gl_texture[i]);
			// ��ͼƬ�л�ȡ����
			loadTexture(image_file_name[i], gl_texture[i]);
		}
	}

	// CoreImageTexture::loadTexture ��ͼƬ�м�������
	void loadTexture(const char* file_name, GLuint& tex_name) {
		//1 ��ȡͼƬ��ʽ
		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name, 0);

		//2 ����ͼƬ
		FIBITMAP *dib = FreeImage_Load(fifmt, file_name, 0);

		//3 ת��Ϊrgb 24ɫ;
		dib = FreeImage_ConvertTo24Bits(dib);

		//4 ��ȡ����ָ��
		BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);

		int width = FreeImage_GetWidth(dib);
		int height = FreeImage_GetHeight(dib);

		// ʹ���������id,���߽а�(����)
		glBindTexture(GL_TEXTURE_2D, tex_name);

		// ��ͼƬ��rgb�����ϴ���opengl.
		glTexImage2D(
			GL_TEXTURE_2D,		// ָ���Ƕ�άͼƬ
			0,					// ָ��Ϊ��һ�������������mipmap,��lod,����ľͲ��ü����ģ�Զ��ʹ�ý�С������
			GL_RGB,				// �����ʹ�õĴ洢��ʽ
			width,				// ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
			height,				// ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
			0,					// �Ƿ�ı�
			GL_BGR_EXT,			// ���ݵĸ�ʽ��bmp�У�windows,����ϵͳ�д洢��������bgr��ʽ
			GL_UNSIGNED_BYTE,	// ������8bit����
			pixels
		);

		// �ͷ��ڴ�
		FreeImage_Unload(dib);
	};
}
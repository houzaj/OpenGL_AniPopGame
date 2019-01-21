// 图片纹理管理库
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
	// 纹理
	GLuint gl_texture[TOTAL_NUMS];

	// CoreImageTexture::initImageTexture 初始化纹理
	void initImageTexture() {
		// 一次性处理所有图片
		for (int i = 0; i < TOTAL_NUMS; i++) {
			// 生成纹理
			glGenTextures(1, &gl_texture[i]);
			// 绑定纹理
			glBindTexture(GL_TEXTURE_2D, gl_texture[i]);
			// 从图片中获取纹理
			loadTexture(image_file_name[i], gl_texture[i]);
		}
	}

	// CoreImageTexture::loadTexture 从图片中加载纹理
	void loadTexture(const char* file_name, GLuint& tex_name) {
		//1 获取图片格式
		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name, 0);

		//2 加载图片
		FIBITMAP *dib = FreeImage_Load(fifmt, file_name, 0);

		//3 转化为rgb 24色;
		dib = FreeImage_ConvertTo24Bits(dib);

		//4 获取数据指针
		BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);

		int width = FreeImage_GetWidth(dib);
		int height = FreeImage_GetHeight(dib);

		// 使用这个纹理id,或者叫绑定(关联)
		glBindTexture(GL_TEXTURE_2D, tex_name);

		// 将图片的rgb数据上传给opengl.
		glTexImage2D(
			GL_TEXTURE_2D,		// 指定是二维图片
			0,					// 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
			GL_RGB,				// 纹理的使用的存储格式
			width,				// 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
			height,				// 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
			0,					// 是否的边
			GL_BGR_EXT,			// 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
			GL_UNSIGNED_BYTE,	// 数据是8bit数据
			pixels
		);

		// 释放内存
		FreeImage_Unload(dib);
	};
}
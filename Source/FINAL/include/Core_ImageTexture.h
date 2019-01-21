#ifndef __CORE_IMAGE_TEXTURE__
#define __CORE_IMAGE_TEXTURE__

namespace CoreImageTexture {
	// ͼƬ��������
	enum TextureType {
		MODEL_GROUND,
		MODEL_GROUND_EDGE,
		MODEL_SUN,
		MODEL_ITEM,
		MODEL_SCORE_BOARD,
		MODEL_GAMEBLOCK_YELLOW,
		MODEL_GAMEBLOCK_RED,
		MODEL_GAMEBLOCK_GREEN,
		MODEL_GAMEBLOCK_PURPLE,
		MODEL_GAMEBLOCK_PINK,
		MODEL_BOMB,
		MODEL_ROBOT_FACE,
		MODEL_ROBOT_TORSO,
		MODEL_ROBOT_ARM,
		MODEL_ROBOT_LEG,
		SKY_UP,
		SKY_DOWN,
		SKY_LEFT,
		SKY_RIGHT,
		SKY_FORWARD,
		SKY_BACK,
		TOTAL_NUMS
	};

	// ͼƬ·��
	extern const char* image_file_name[TOTAL_NUMS];
	// ����
	extern GLuint gl_texture[TOTAL_NUMS];

	// ��ʼ��ͼƬ����
	void initImageTexture();
	// ��ͼƬ�м�������
	void loadTexture(const char* file_name, GLuint& tex_name);
}

#endif // !__CORE_IMAGE_TEXTURE__

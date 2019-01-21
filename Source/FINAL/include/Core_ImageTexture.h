#ifndef __CORE_IMAGE_TEXTURE__
#define __CORE_IMAGE_TEXTURE__

namespace CoreImageTexture {
	// 图片纹理类型
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

	// 图片路径
	extern const char* image_file_name[TOTAL_NUMS];
	// 纹理
	extern GLuint gl_texture[TOTAL_NUMS];

	// 初始化图片纹理
	void initImageTexture();
	// 从图片中加载纹理
	void loadTexture(const char* file_name, GLuint& tex_name);
}

#endif // !__CORE_IMAGE_TEXTURE__

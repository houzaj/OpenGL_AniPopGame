#ifndef __CORE_MESH_H__
#define __CORE_MESH_H__

#include "Auxiliary_H.h"
#include "Core_ImageTexture.h"

struct point3f {
	float x, y, z;
	point3f() : x(0), y(0), z(0) {}
	point3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

	float sqr(float x) {
		return x * x;
	}

	float distance(const point3f& p) {
		return sqrt(sqr(x - p.x) + sqr(y - p.y) + sqr(z - p.z));
	};
};

typedef std::vector<vec3> list_vertex;
typedef std::vector<vec3> list_normal;
typedef std::vector<vec2> list_vt;
typedef std::vector<vec3i> list_face;

typedef std::vector<float> STLVectorf;
typedef std::vector<int> STLVectori;



namespace CoreMesh {
	// 图形类
	class Mesh {
	public:
		bool is_paint_shader;		// 是否绘制阴影
		bool is_lighted;			// 是否受光源影响
		bool is_painted;			// 是否绘制
		bool is_textured;			// 是否绘制纹理

		list_vertex ls_v;			// 顶点信息
		list_normal ls_n;			// 法线信息
		list_face   ls_fs;			// 面片信息
		list_vt		ls_vt;
		mat4		model_matrix;	// 模型矩阵
		vec4		color;			// 颜色信息

		// 纹理类型
		CoreImageTexture::TextureType tex_type;

		Mesh();

		// 初始化
		void init();

		// 设置图形信息
		void setData(const list_vertex& p_ls_v, const list_normal& p_ls_n, const list_face& p_ls_fs, const mat4& p_model_matrix, const vec4& col);
		// 设置图形纹理
		void setTexture(const CoreImageTexture::TextureType& p_tex_type, const list_vt& p_ls_vt);
		
		void initShader();
		void displayShader();

		friend class MeshPainter;

	private:
		GLuint programID;			// 着色器
		GLuint vertexArrayID;		// VAO
		GLuint vertexID;			// 顶点VBO
		GLuint vertexNormalID;		// 法线VBO
		GLuint vertexIndexID;		// 顶点索引VBO
		GLuint vertexTexCoordID;	// VT坐标VBO

		GLuint vPositionID;			// 顶点位置
		GLuint vNormalID;			// 法线位置
		GLuint vTexCoordID;			// VT坐标位置
		GLuint scaleID;				// 光照强度缩放因子位置
		GLuint modelMatrixID;		// 模型矩阵位置
		GLuint draw_color;			// 颜色信息
		GLuint isShadowID;			// 是否为阴影的标记变量的位置
		GLuint isLightedID;			// 是否受光照影响
		GLuint isTexturedID;		// 是否绘制纹理

		GLuint gl_texture;			// 纹理

		
	};

	class MeshPainter {
	public:
		MeshPainter();
		~MeshPainter();
		// 初始化
		void init();
		// 销毁
		void destroy();

		void initMeshShader();
		void displayMeshShader();
		// 新增图形
		void addMesh(CoreMesh::Mesh* mesh, int pos, bool is_paint_shader = false, bool is_lighted = true, bool is_painted = true);
		// 获取在pos位置上的图形
		CoreMesh::Mesh*& getMeshAt(int pos);
		// 获取图形大小
		int getSize();
	private:
		// 获取图形大小
		std::vector<CoreMesh::Mesh*> mp;	
	};
	// 实例化图形绘制器类
	extern MeshPainter painter;
}


#endif // !__CORE_MESH_H__

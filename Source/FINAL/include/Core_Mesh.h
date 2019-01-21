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
	// ͼ����
	class Mesh {
	public:
		bool is_paint_shader;		// �Ƿ������Ӱ
		bool is_lighted;			// �Ƿ��ܹ�ԴӰ��
		bool is_painted;			// �Ƿ����
		bool is_textured;			// �Ƿ��������

		list_vertex ls_v;			// ������Ϣ
		list_normal ls_n;			// ������Ϣ
		list_face   ls_fs;			// ��Ƭ��Ϣ
		list_vt		ls_vt;
		mat4		model_matrix;	// ģ�;���
		vec4		color;			// ��ɫ��Ϣ

		// ��������
		CoreImageTexture::TextureType tex_type;

		Mesh();

		// ��ʼ��
		void init();

		// ����ͼ����Ϣ
		void setData(const list_vertex& p_ls_v, const list_normal& p_ls_n, const list_face& p_ls_fs, const mat4& p_model_matrix, const vec4& col);
		// ����ͼ������
		void setTexture(const CoreImageTexture::TextureType& p_tex_type, const list_vt& p_ls_vt);
		
		void initShader();
		void displayShader();

		friend class MeshPainter;

	private:
		GLuint programID;			// ��ɫ��
		GLuint vertexArrayID;		// VAO
		GLuint vertexID;			// ����VBO
		GLuint vertexNormalID;		// ����VBO
		GLuint vertexIndexID;		// ��������VBO
		GLuint vertexTexCoordID;	// VT����VBO

		GLuint vPositionID;			// ����λ��
		GLuint vNormalID;			// ����λ��
		GLuint vTexCoordID;			// VT����λ��
		GLuint scaleID;				// ����ǿ����������λ��
		GLuint modelMatrixID;		// ģ�;���λ��
		GLuint draw_color;			// ��ɫ��Ϣ
		GLuint isShadowID;			// �Ƿ�Ϊ��Ӱ�ı�Ǳ�����λ��
		GLuint isLightedID;			// �Ƿ��ܹ���Ӱ��
		GLuint isTexturedID;		// �Ƿ��������

		GLuint gl_texture;			// ����

		
	};

	class MeshPainter {
	public:
		MeshPainter();
		~MeshPainter();
		// ��ʼ��
		void init();
		// ����
		void destroy();

		void initMeshShader();
		void displayMeshShader();
		// ����ͼ��
		void addMesh(CoreMesh::Mesh* mesh, int pos, bool is_paint_shader = false, bool is_lighted = true, bool is_painted = true);
		// ��ȡ��posλ���ϵ�ͼ��
		CoreMesh::Mesh*& getMeshAt(int pos);
		// ��ȡͼ�δ�С
		int getSize();
	private:
		// ��ȡͼ�δ�С
		std::vector<CoreMesh::Mesh*> mp;	
	};
	// ʵ����ͼ�λ�������
	extern MeshPainter painter;
}


#endif // !__CORE_MESH_H__

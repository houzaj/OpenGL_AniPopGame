// ͼ�ο�
#include "Auxiliary_H.h"
#include "Core_Mesh.h"

namespace CoreMesh {

	Mesh::Mesh() {
		init();
	}
	// ��ʼ��ͼ��
	void Mesh::init() {
		ls_v.clear();
		ls_n.clear();
		ls_fs.clear();
		is_paint_shader = false;
		is_lighted = true;
		is_painted = true;
		is_textured = false;
	}
	
	// �������ݣ������㣬�������꣬��Ƭ������ģ�;����Լ���ɫ
	void Mesh::setData(const list_vertex& p_ls_v, const list_normal& p_ls_n, const list_face& p_ls_fs, const mat4& p_model_matrix, const vec4& col) {
		init();
		for (auto ele : p_ls_v) {
			ls_v.push_back(ele);
		}
		for (auto ele : p_ls_n) {
			ls_n.push_back(ele);
		}
		for (auto ele : p_ls_fs) {
			ls_fs.push_back(ele);
		}
		model_matrix = p_model_matrix;
		color = col;
	}
	
	// ����ͼ��������Ϣ 
	void Mesh::setTexture(const CoreImageTexture::TextureType& p_tex_type, const list_vt& p_ls_vt) {
		ls_vt.clear();
		is_textured = true;
		tex_type = p_tex_type;
		for (auto ele : p_ls_vt) {
			ls_vt.push_back(ele);
		}
	}

	// ��ʼ����ɫ��
	void Mesh::initShader() {
		programID = InitShader("glsl/vshader.glsl", "glsl/fshader.glsl");

		// �Ӷ�����ɫ����ƬԪ��ɫ���л�ȡ������λ��
		vPositionID = glGetAttribLocation(programID, "vPosition");
		vNormalID = glGetAttribLocation(programID, "vNormal");
		vTexCoordID = glGetAttribLocation(programID, "vTexCoord");
		scaleID = glGetUniformLocation(programID, "scale");
		modelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
		draw_color = glGetUniformLocation(programID, "draw_color");
		isShadowID = glGetUniformLocation(programID, "is_shadow");
		isLightedID = glGetUniformLocation(programID, "is_lighted");
		isTexturedID = glGetUniformLocation(programID, "is_textured");

		// ����VAO
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// ����VBO�����󶨶�������
		glGenBuffers(1, &vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, ls_v.size() * sizeof(vec3f), ls_v.data(), GL_DYNAMIC_DRAW);

		// ����Ҫ��������
		if (is_textured) {
			// ����VBO������VT��������
			glGenBuffers(1, &vertexTexCoordID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexTexCoordID);
			glBufferData(GL_ARRAY_BUFFER, ls_vt.size() * sizeof(vec2), ls_vt.data(), GL_DYNAMIC_DRAW);
		}

		// ����VBO�����󶨷���������
		glGenBuffers(1, &vertexNormalID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
		glBufferData(GL_ARRAY_BUFFER, ls_n.size() * sizeof(vec3f), ls_n.data(), GL_DYNAMIC_DRAW);
		
		// ����VBO�����󶨶�������
		glGenBuffers(1, &vertexIndexID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ls_fs.size() * sizeof(vec3i), ls_fs.data(), GL_DYNAMIC_DRAW);

		if (is_textured) {
			// ���ò�������ֵ
			glUniform1i(glGetUniformLocation(programID, "texture"), 0);
		}
	}

	// ������ɫ��
	void Mesh::displayShader() {
		if (is_painted == false) {
			return;
		}

		// ��VAO
		glBindVertexArray(vertexArrayID);

		// �������Uniform������ֵ
		glUniform1i(isShadowID, 0);
		glUniform1i(isLightedID, is_lighted);
		glUniform1i(isTexturedID, is_textured);
		glUniform4fv(draw_color, 1, &color[0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &model_matrix[0][0]);

		
		// ����Ҫ��������
		if (is_textured) {
			// ȡ������
			gl_texture = CoreImageTexture::gl_texture[tex_type];
			// ������
			glBindTexture(GL_TEXTURE_2D, gl_texture);
			// ָ�����˷���
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// ��������Ԫ��������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gl_texture);
		}

		// ��������ݴ�����ɫ����������ά����
		glEnableVertexAttribArray(vPositionID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glVertexAttribPointer(vPositionID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(vNormalID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
		glVertexAttribPointer(vNormalID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		if (is_textured) {
			glEnableVertexAttribArray(vTexCoordID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexTexCoordID);
			glVertexAttribPointer(vTexCoordID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexID);
		glDrawElements(GL_TRIANGLES, int(ls_fs.size() * 3), GL_UNSIGNED_INT, (void*)0);

		// ����Ҫ������Ӱ
		if (is_paint_shader) {
			glUniform4fv(draw_color, 1, &(vec4(0.0, 0.0, 0.0, 1))[0]);
			glUniform1f(scaleID, 0.0);
			glUniform1i(isShadowID, 1);
			glUniform1i(isLightedID, 1);
			glUniform1i(isTexturedID, 0);
			// �õ���Ӱ��ͶӰ����
			mat4 tmp_matrix = CoreLight::getShadowProjectionMatrix() * model_matrix;
			glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &tmp_matrix[0][0]);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexID);
			glDrawElements(GL_TRIANGLES, int(ls_fs.size() * 3), GL_UNSIGNED_INT, (void*)0);

			glUniform1f(scaleID, 1.2);
		}

	}


	/*
		Mesh Painter
	*/
	MeshPainter::MeshPainter() {
		init();
	}

	MeshPainter::~MeshPainter() {
		destroy();
	}

	void MeshPainter::init() {
		mp.clear();
	}

	void MeshPainter::destroy() {
		for (auto ptr : mp) {
			delete ptr;
		}
	}

	// ������ɫ����Ⱦ��ز���
	void MeshPainter::displayMeshShader() {
		for (auto ptr : mp) {
			ptr->displayShader();
		}
	}

	// ��ʼ����ɫ����ز���
	void MeshPainter::initMeshShader() {
		// ��ʼ��ͼƬ����
		CoreImageTexture::initImageTexture();
		for (auto ptr : mp) {
			ptr->initShader();
		}
	}

	// ��ӻ������Ҫ���Ƶ�ͼ����Ϣ
	void MeshPainter::addMesh(CoreMesh::Mesh* mesh, int pos, bool is_paint_shader, bool is_lighted, bool is_painted) {
		// �ж��Ƿ���Ҫ������Ӱ
		mesh->is_paint_shader = is_paint_shader;
		// �ж��Ƿ���Ҫ�ܹ���Ч��Ӱ��
		mesh->is_lighted = is_lighted;
		mesh->is_painted = is_painted;

		if (pos == (int)mp.size()) {		// ���ڿ�����ĩ�ˣ���Ϊ��ͼ�Σ�ֱ�Ӽ���
			mp.push_back(mesh);
		}
		else {	// ���򣬸��������Ϣ�������ͷŴ����ͼ����ռ�õĿռ�
			mp[pos]->model_matrix = mesh->model_matrix;
			mp[pos]->color = mesh->color;
			mp[pos]->is_lighted = mesh->is_lighted;
			mp[pos]->is_painted = mesh->is_painted;
			mp[pos]->is_paint_shader = mesh->is_paint_shader;
			delete mesh;
		}
	}

	// ��ȡ��ǰ��Ҫ���Ƶ�ͼ������
	int MeshPainter::getSize() {
		return mp.size();
	}
	
	// ��ȡ�±�Ϊpos��ͼ��
	CoreMesh::Mesh*& MeshPainter::getMeshAt(int pos) {
		return mp[pos];
	}

	MeshPainter painter;
}



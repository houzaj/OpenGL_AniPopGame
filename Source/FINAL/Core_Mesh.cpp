// 图形库
#include "Auxiliary_H.h"
#include "Core_Mesh.h"

namespace CoreMesh {

	Mesh::Mesh() {
		init();
	}
	// 初始化图形
	void Mesh::init() {
		ls_v.clear();
		ls_n.clear();
		ls_fs.clear();
		is_paint_shader = false;
		is_lighted = true;
		is_painted = true;
		is_textured = false;
	}
	
	// 放入数据，即顶点，法线坐标，面片索引，模型矩阵以及颜色
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
	
	// 设置图形纹理信息 
	void Mesh::setTexture(const CoreImageTexture::TextureType& p_tex_type, const list_vt& p_ls_vt) {
		ls_vt.clear();
		is_textured = true;
		tex_type = p_tex_type;
		for (auto ele : p_ls_vt) {
			ls_vt.push_back(ele);
		}
	}

	// 初始化着色器
	void Mesh::initShader() {
		programID = InitShader("glsl/vshader.glsl", "glsl/fshader.glsl");

		// 从顶点着色器和片元着色器中获取变量的位置
		vPositionID = glGetAttribLocation(programID, "vPosition");
		vNormalID = glGetAttribLocation(programID, "vNormal");
		vTexCoordID = glGetAttribLocation(programID, "vTexCoord");
		scaleID = glGetUniformLocation(programID, "scale");
		modelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
		draw_color = glGetUniformLocation(programID, "draw_color");
		isShadowID = glGetUniformLocation(programID, "is_shadow");
		isLightedID = glGetUniformLocation(programID, "is_lighted");
		isTexturedID = glGetUniformLocation(programID, "is_textured");

		// 生成VAO
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// 生成VBO，并绑定顶点数据
		glGenBuffers(1, &vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, ls_v.size() * sizeof(vec3f), ls_v.data(), GL_DYNAMIC_DRAW);

		// 若需要生成纹理
		if (is_textured) {
			// 生成VBO，并绑定VT坐标索引
			glGenBuffers(1, &vertexTexCoordID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexTexCoordID);
			glBufferData(GL_ARRAY_BUFFER, ls_vt.size() * sizeof(vec2), ls_vt.data(), GL_DYNAMIC_DRAW);
		}

		// 生成VBO，并绑定法向量数据
		glGenBuffers(1, &vertexNormalID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
		glBufferData(GL_ARRAY_BUFFER, ls_n.size() * sizeof(vec3f), ls_n.data(), GL_DYNAMIC_DRAW);
		
		// 生成VBO，并绑定顶点索引
		glGenBuffers(1, &vertexIndexID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ls_fs.size() * sizeof(vec3i), ls_fs.data(), GL_DYNAMIC_DRAW);

		if (is_textured) {
			// 设置采样器的值
			glUniform1i(glGetUniformLocation(programID, "texture"), 0);
		}
	}

	// 绘制着色器
	void Mesh::displayShader() {
		if (is_painted == false) {
			return;
		}

		// 绑定VAO
		glBindVertexArray(vertexArrayID);

		// 传递相关Uniform变量的值
		glUniform1i(isShadowID, 0);
		glUniform1i(isLightedID, is_lighted);
		glUniform1i(isTexturedID, is_textured);
		glUniform4fv(draw_color, 1, &color[0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &model_matrix[0][0]);

		
		// 若需要绘制纹理
		if (is_textured) {
			// 取出纹理
			gl_texture = CoreImageTexture::gl_texture[tex_type];
			// 绑定纹理
			glBindTexture(GL_TEXTURE_2D, gl_texture);
			// 指定过滤方法
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// 激活纹理单元并绑定纹理
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gl_texture);
		}

		// 将相关数据传入着色器，绘制三维物体
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

		// 若需要绘制阴影
		if (is_paint_shader) {
			glUniform4fv(draw_color, 1, &(vec4(0.0, 0.0, 0.0, 1))[0]);
			glUniform1f(scaleID, 0.0);
			glUniform1i(isShadowID, 1);
			glUniform1i(isLightedID, 1);
			glUniform1i(isTexturedID, 0);
			// 得到阴影的投影矩阵
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

	// 调用着色器渲染相关部分
	void MeshPainter::displayMeshShader() {
		for (auto ptr : mp) {
			ptr->displayShader();
		}
	}

	// 初始化着色器相关部分
	void MeshPainter::initMeshShader() {
		// 初始化图片纹理
		CoreImageTexture::initImageTexture();
		for (auto ptr : mp) {
			ptr->initShader();
		}
	}

	// 添加或更新需要绘制的图形信息
	void MeshPainter::addMesh(CoreMesh::Mesh* mesh, int pos, bool is_paint_shader, bool is_lighted, bool is_painted) {
		// 判断是否需要绘制阴影
		mesh->is_paint_shader = is_paint_shader;
		// 判断是否需要受光照效果影响
		mesh->is_lighted = is_lighted;
		mesh->is_painted = is_painted;

		if (pos == (int)mp.size()) {		// 若在开区间末端，则为新图形，直接加入
			mp.push_back(mesh);
		}
		else {	// 否则，更新相关信息，并且释放传入的图形所占用的空间
			mp[pos]->model_matrix = mesh->model_matrix;
			mp[pos]->color = mesh->color;
			mp[pos]->is_lighted = mesh->is_lighted;
			mp[pos]->is_painted = mesh->is_painted;
			mp[pos]->is_paint_shader = mesh->is_paint_shader;
			delete mesh;
		}
	}

	// 获取当前需要绘制的图形总数
	int MeshPainter::getSize() {
		return mp.size();
	}
	
	// 获取下标为pos的图形
	CoreMesh::Mesh*& MeshPainter::getMeshAt(int pos) {
		return mp[pos];
	}

	MeshPainter painter;
}



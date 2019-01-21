// 辅助创建基本图形库
#include "Auxiliary_H.h"
#include "Auxiliary_GenShape.h"

namespace AuxiliaryGenShape {
	// 创建正方形
	void genRectangle(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs) {
		ls_v.clear();
		ls_n.clear();
		ls_fs.clear();

		// Top
		ls_v.push_back(vec3(-0.5, 0, -0.5));
		ls_n.push_back(vec3(0, 1, 0));
		ls_v.push_back(vec3(0.5, 0, -0.5));
		ls_n.push_back(vec3(0, 1, 0));
		ls_v.push_back(vec3(0.5, 0, 0.5));
		ls_n.push_back(vec3(0, 1, 0));
		ls_v.push_back(vec3(-0.5, 0, 0.5));
		ls_n.push_back(vec3(0, 1, 0));

		// FORWARD
		ls_fs.push_back(vec3i(0, 1, 2));
		ls_fs.push_back(vec3i(0, 3, 2));
	}

	// 生成以原点为中心，边长为1的正方体
	void genCube(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs) {
		ls_v.clear();
		ls_n.clear();
		ls_fs.clear();

		// Left
		ls_v.push_back(vec3(-0.5, 0.5, -0.5));
		ls_n.push_back(vec3(-1, 0, 0));
		ls_v.push_back(vec3(-0.5, 0.5, 0.5));
		ls_n.push_back(vec3(-1, 0, 0));
		ls_v.push_back(vec3(-0.5, -0.5, 0.5));
		ls_n.push_back(vec3(-1, 0, 0));
		ls_v.push_back(vec3(-0.5, -0.5, -0.5));
		ls_n.push_back(vec3(-1, 0, 0));
		// Top
		ls_v.push_back(vec3(-0.5, 0.5, -0.5));
		ls_n.push_back(vec3(0, 1, 0));
		ls_v.push_back(vec3(0.5, 0.5, -0.5));
		ls_n.push_back(vec3(0, 1, 0));
		ls_v.push_back(vec3(0.5, 0.5, 0.5));
		ls_n.push_back(vec3(0, 1, 0));
		ls_v.push_back(vec3(-0.5, 0.5, 0.5));
		ls_n.push_back(vec3(0, 1, 0));
		// Right
		ls_v.push_back(vec3(0.5, 0.5, 0.5));
		ls_n.push_back(vec3(1, 0, 0));
		ls_v.push_back(vec3(0.5, 0.5, -0.5));
		ls_n.push_back(vec3(1, 0, 0));
		ls_v.push_back(vec3(0.5, -0.5, -0.5));
		ls_n.push_back(vec3(1, 0, 0));
		ls_v.push_back(vec3(0.5, -0.5, 0.5));
		ls_n.push_back(vec3(1, 0, 0));
		// Bottom
		ls_v.push_back(vec3(-0.5, -0.5, 0.5));
		ls_n.push_back(vec3(0, -1, 0));
		ls_v.push_back(vec3(0.5, -0.5, 0.5));
		ls_n.push_back(vec3(0, -1, 0));
		ls_v.push_back(vec3(0.5, -0.5, -0.5));
		ls_n.push_back(vec3(0, -1, 0));
		ls_v.push_back(vec3(-0.5, -0.5, -0.5));
		ls_n.push_back(vec3(0, -1, 0));
		// FORWARD
		ls_v.push_back(vec3(-0.5, 0.5, 0.5));
		ls_n.push_back(vec3(0, 0, 1));
		ls_v.push_back(vec3(0.5, 0.5, 0.5));
		ls_n.push_back(vec3(0, 0, 1));
		ls_v.push_back(vec3(0.5, -0.5, 0.5));
		ls_n.push_back(vec3(0, 0, 1));
		ls_v.push_back(vec3(-0.5, -0.5, 0.5));
		ls_n.push_back(vec3(0, 0, 1));
		// BACK
		ls_v.push_back(vec3(0.5, 0.5, -0.5));
		ls_n.push_back(vec3(0, 0, -1));
		ls_v.push_back(vec3(-0.5, 0.5, -0.5));
		ls_n.push_back(vec3(0, 0, -1));
		ls_v.push_back(vec3(-0.5, -0.5, -0.5));
		ls_n.push_back(vec3(0, 0, -1));
		ls_v.push_back(vec3(0.5, -0.5, -0.5));
		ls_n.push_back(vec3(0, 0, -1));

		// Left
		ls_fs.push_back(vec3i(0, 1, 3));
		ls_fs.push_back(vec3i(2, 1, 3));
		// Top
		ls_fs.push_back(vec3i(4, 5, 6));
		ls_fs.push_back(vec3i(4, 7, 6));
		// Right
		ls_fs.push_back(vec3i(8, 9, 11));
		ls_fs.push_back(vec3i(10, 9, 11));
		// Bottom
		ls_fs.push_back(vec3i(12, 13, 14));
		ls_fs.push_back(vec3i(12, 15, 14));
		// FORWARD
		ls_fs.push_back(vec3i(16, 17, 18));
		ls_fs.push_back(vec3i(16, 19, 18));
		// BACK
		ls_fs.push_back(vec3i(20, 21, 22));
		ls_fs.push_back(vec3i(20, 23, 22));
	}

	// 创建圆柱体
	void genCylinder(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs, int k) {
		ls_v.clear();
		ls_n.clear();
		ls_fs.clear();

		list_vertex v[2];
		for (int i = 0, dir = 1; i < 2; i++, dir *= -1) {
			double delta = 2 * M_PI / k;
			double angle = 0;
			for (int j = 0; j < k; j++) {
				v[i].push_back(vec3(0.5 * cos(angle), 0.5 * dir, 0.5 * sin(angle)));
				ls_v.push_back(vec3(0.5 * cos(angle), 0.5 * dir, 0.5 * sin(angle)));
				ls_n.push_back(vec3(0, dir, 0));
				angle += delta;
			}
			for (int j = 1; j + 1 < k; j++) {
				ls_fs.push_back(vec3i(i * k + 0, i * k + j, i * k + j + 1));
			}
		}

		int pos = (int)ls_v.size();
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < k; j++) {
				ls_v.push_back(v[i][j]);
				ls_n.push_back(vec3(v[i][j].x, 0, v[i][j].z));
			}
		}

		for (int j = 0; j + 1 < k; j++) {
			ls_fs.push_back(vec3i(pos + j, pos + j + 1, pos + k + j));
			ls_fs.push_back(vec3i(pos + k + j, pos + k + j + 1, pos + j + 1));
		}
		ls_fs.push_back(vec3i(pos + k - 1, pos, pos + k + k - 1));
		ls_fs.push_back(vec3i(pos + k + k - 1, pos + k, pos));
	}

	// 创建双四棱锥
	void genRectangularPyramid(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs) {
		ls_v.clear();
		ls_n.clear();
		ls_fs.clear();

		float len = 0.5 / 1.414;

		// UP
		// FORWARD
		ls_v.push_back(vec3(0, 0.5, 0));
		ls_n.push_back(vec3(0, 1, 1.414));
		ls_v.push_back(vec3(-len, 0, len));
		ls_n.push_back(vec3(0, 1, 1.414));
		ls_v.push_back(vec3(len, 0, len));
		ls_n.push_back(vec3(0, 1, 1.414));
		// LEFT
		ls_v.push_back(vec3(0, 0.5, 0));
		ls_n.push_back(vec3(-1.414, 1, 0));
		ls_v.push_back(vec3(-len, 0, -len));
		ls_n.push_back(vec3(-1.414, 1, 0));
		ls_v.push_back(vec3(-len, 0, len));
		ls_n.push_back(vec3(-1.414, 1, 0));
		// BACKWARD
		ls_v.push_back(vec3(0, 0.5, 0));
		ls_n.push_back(vec3(0, 1, -1.414));
		ls_v.push_back(vec3(len, 0, -len));
		ls_n.push_back(vec3(0, 1, -1.414));
		ls_v.push_back(vec3(-len, 0, -len));
		ls_n.push_back(vec3(0, 1, -1.414));
		// RIGHT
		ls_v.push_back(vec3(0, 0.5, 0));
		ls_n.push_back(vec3(1.414, 1, 0));
		ls_v.push_back(vec3(len, 0, len));
		ls_n.push_back(vec3(1.414, 1, 0));
		ls_v.push_back(vec3(len, 0, -len));
		ls_n.push_back(vec3(1.414, 1, 0));

		// BOTTOM
		// FORWARD
		ls_v.push_back(vec3(0, -0.5, 0));
		ls_n.push_back(vec3(0, -1, 1.414));
		ls_v.push_back(vec3(-len, 0, len));
		ls_n.push_back(vec3(0, -1, 1.414));
		ls_v.push_back(vec3(len, 0, len));
		ls_n.push_back(vec3(0, -1, 1.414));
		// LEFT
		ls_v.push_back(vec3(0, -0.5, 0));
		ls_n.push_back(vec3(-1.414, -1, 0));
		ls_v.push_back(vec3(-len, 0, -len));
		ls_n.push_back(vec3(-1.414, -1, 0));
		ls_v.push_back(vec3(-len, 0, len));
		ls_n.push_back(vec3(-1.414, -1, 0));
		// BACKWARD
		ls_v.push_back(vec3(0, -0.5, 0));
		ls_n.push_back(vec3(0, -1, -1.414));
		ls_v.push_back(vec3(len, 0, -len));
		ls_n.push_back(vec3(0, -1, -1.414));
		ls_v.push_back(vec3(-len, 0, -len));
		ls_n.push_back(vec3(0, -1, -1.414));
		// RIGHT
		ls_v.push_back(vec3(0, -0.5, 0));
		ls_n.push_back(vec3(1.414, -1, 0));
		ls_v.push_back(vec3(len, 0, len));
		ls_n.push_back(vec3(1.414, -1, 0));
		ls_v.push_back(vec3(len, 0, -len));
		ls_n.push_back(vec3(1.414, -1, 0));

		ls_fs.push_back(vec3i(0, 1, 2));
		ls_fs.push_back(vec3i(3, 4, 5));
		ls_fs.push_back(vec3i(6, 7, 8));
		ls_fs.push_back(vec3i(9, 10, 11));

		ls_fs.push_back(vec3i(12, 13, 14));
		ls_fs.push_back(vec3i(15, 16, 17));
		ls_fs.push_back(vec3i(18, 19, 20));
		ls_fs.push_back(vec3i(21, 22, 23));
	}
}

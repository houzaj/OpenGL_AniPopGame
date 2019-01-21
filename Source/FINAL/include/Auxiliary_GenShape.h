#ifndef __AUXILIARY_Shape_H__
#define __AUXILIARY_Shape_H__

namespace AuxiliaryGenShape {
	void genRectangle(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs);
	void genCube(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs);
	void genCylinder(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs, int k);
	void genRectangularPyramid(list_vertex& ls_v, list_normal& ls_n, list_face& ls_fs);
}

#endif // !__AUXILIARY_Shape_H__

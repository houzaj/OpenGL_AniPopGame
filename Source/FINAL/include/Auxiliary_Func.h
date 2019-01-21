#ifndef __AUXILIARY_FUNC_H__
#define __AUXILIARY_FUNC_H__

#include "Angel.h"

namespace AuxiliaryFunc {
	int rand();
	float randFloat(int l = -1000000000, int r = 1000000000);
	void genRandomSeries(std::vector<int>& vec, int l, int r);
	float sqr(float x);
}

#endif // !__AUXILIARY_FUNC_H__

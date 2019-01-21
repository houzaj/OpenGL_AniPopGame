// ����������
#include "Auxiliary_H.h"
#include "Auxiliary_Func.h"
#include <random>

namespace AuxiliaryFunc {
	// �����
	int rand() {
		static int seed = (unsigned long long)glutGet(GLUT_ELAPSED_TIME) * glutGet(GLUT_ELAPSED_TIME) % 9901;
		return seed = (int)seed * 482711LL % 2147483647;
	}

	// ���������
	float randFloat(int l, int r) {
		static long long seed = (unsigned long long)glutGet(GLUT_ELAPSED_TIME) * glutGet(GLUT_ELAPSED_TIME) % 9901;
		seed = seed * 482711LL % 2147483647;
		return (seed % ((r - l) * 100)) / 100.0f + l;
	}

	// ���[l,r]��Χ�ڵĲ��ظ�����
	void genRandomSeries(std::vector<int>& vec, int l, int r) {
		static std::default_random_engine seed(rand());
		vec.clear();
		for (int i = l; i <= r; i++) {
			vec.push_back(i);
		}
		std::shuffle(vec.begin(), vec.end(), seed);
	}

	// ��x��ƽ��
	float sqr(float x) {
		return x * x;
	}
}

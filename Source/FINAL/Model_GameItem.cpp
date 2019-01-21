// ��Ϸ����
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelGameItem {
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	int pos_begin;

	const int NUMS_MODEL_GAME_ITEM_NUMS = 3;					// ��Ϸ��������
	vec3 game_item_position[NUMS_MODEL_GAME_ITEM_NUMS];			// ��Ϸ����λ��
	GAME_ITEM_TYPE  game_item_type[NUMS_MODEL_GAME_ITEM_NUMS];	// ��Ϸ��������
	const float GAME_ITEM_SCALE = 0.5;							// ��Ϸ��������ֵ

	// ����UV����
	void loadModelGameItemTexCoord() {
		ls_vt.clear();
		int n = ls_v.size();
		for (int i = 0; i < n; i += 4) {
			ls_vt.push_back(vec2(0, 1));
			ls_vt.push_back(vec2(1, 1));
			ls_vt.push_back(vec2(1, 0));
			ls_vt.push_back(vec2(0, 0));
		}
	}

	// ������Ϸ����
	void loadModelGameItem() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelGameItemTexCoord();
		pos_begin = CoreMesh::painter.getSize();
		// �������
		for (int i = 0; i < NUMS_MODEL_GAME_ITEM_NUMS; i++) {
			reloadModelGameItemAt(i);
		}
	}

	// �ؼ����±�Ϊindex����Ϸ����
	void reloadModelGameItemAt(int index) {
		updateGameItemAt(index);
		CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(game_item_position[index] +
			vec3(0, ModelGameBlock::game_block_offset_y * 5 + 0.5 * GAME_ITEM_SCALE, 0)) *
			Scale(GAME_ITEM_SCALE, GAME_ITEM_SCALE, GAME_ITEM_SCALE), vec3(1.0, 1.0, 1.0));
		mesh->setTexture(CoreImageTexture::TextureType::MODEL_ITEM, ls_vt);
		CoreMesh::painter.addMesh(mesh, pos_begin + index, true);
	}

	// ������ɵ���
	void updateGameItemAt(int index) {
		// ������ɵ��ߵ�λ�ã���ȷ���䲻����Ϸ������һ����Χ����ײ���Ϊ��
		float x, z;
		while(true){
			x = AuxiliaryFunc::randFloat(-4 * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale, 4 * ModelGround::block_scale + 0.5 * ModelGameBlock::game_block_scale);
			z = AuxiliaryFunc::randFloat(-4 * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale, 4 * ModelGround::block_scale + 0.5 * ModelGameBlock::game_block_scale);
			
			// ����ȫ���ڷ�Χ����Ϊ�Ϸ�
			int cnt = 0;
			for (int i = -4; i <= 4; i++) {
				for (int j = -4; j <= 4; j++) {
					if (i * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale <= x &&
						x <= i * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale &&
						j * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale <= z &&
						z <= j * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale) {
						cnt++;
					}
				}
			}
			if (cnt == 0) {
				break;
			}
		}
		game_item_position[index] = vec3(x, 0, z);

		// ������ɵ��ߵ�����
		int rand_res = AuxiliaryFunc::rand() % 10 + 1;
		if (1 <= rand_res && rand_res <= 3) {		// ��0.3�ĸ������ɿ�Ѫ����
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_HP_DOWN;
		}else if(4 <= rand_res && rand_res <= 6){	// ��0.3�ĸ������ɼ��ٵ���
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_SPEED_DOWN;
		}else if(7 <= rand_res && rand_res <= 8){	// ��0.2�ĸ������ɻ�Ѫ����
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_HP_UP;
		}else if(9 <= rand_res && rand_res <= 10){	// ��0.2�ĸ������ɼ��ٵ���
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_SPEED_UP;
		}
	}

	// ����Ϸ���ߵ���ײ���
	int checkPointIsInItem(vec3 position) {
		for (int i = 0; i < NUMS_MODEL_GAME_ITEM_NUMS; i++) {
			// ά�����ֵ��Сֵ
			float minx = game_item_position[i].x - GAME_ITEM_SCALE;
			float maxx = game_item_position[i].x + GAME_ITEM_SCALE;
			float minz = game_item_position[i].z - GAME_ITEM_SCALE;
			float maxz = game_item_position[i].z + GAME_ITEM_SCALE;
			float maxy = GAME_ITEM_SCALE + ModelGameBlock::game_block_offset_y;
			// ���ڷ�Χ����˵����������ײ
			if (minx <= position.x && position.x <= maxx && minz <= position.z && position.z <= maxz && position.y <= maxy) {
				return i;
			}
		}
		return -1;
	}
}

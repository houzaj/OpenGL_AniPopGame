// 游戏道具
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelGameItem {
	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	int pos_begin;

	const int NUMS_MODEL_GAME_ITEM_NUMS = 3;					// 游戏道具数量
	vec3 game_item_position[NUMS_MODEL_GAME_ITEM_NUMS];			// 游戏道具位置
	GAME_ITEM_TYPE  game_item_type[NUMS_MODEL_GAME_ITEM_NUMS];	// 游戏道具类型
	const float GAME_ITEM_SCALE = 0.5;							// 游戏道具缩放值

	// 加载UV坐标
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

	// 加载游戏道具
	void loadModelGameItem() {
		AuxiliaryGenShape::genCube(ls_v, ls_n, ls_fs);
		loadModelGameItemTexCoord();
		pos_begin = CoreMesh::painter.getSize();
		// 随机生成
		for (int i = 0; i < NUMS_MODEL_GAME_ITEM_NUMS; i++) {
			reloadModelGameItemAt(i);
		}
	}

	// 重加载下标为index的游戏道具
	void reloadModelGameItemAt(int index) {
		updateGameItemAt(index);
		CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
		mesh->setData(ls_v, ls_n, ls_fs, Translate(game_item_position[index] +
			vec3(0, ModelGameBlock::game_block_offset_y * 5 + 0.5 * GAME_ITEM_SCALE, 0)) *
			Scale(GAME_ITEM_SCALE, GAME_ITEM_SCALE, GAME_ITEM_SCALE), vec3(1.0, 1.0, 1.0));
		mesh->setTexture(CoreImageTexture::TextureType::MODEL_ITEM, ls_vt);
		CoreMesh::painter.addMesh(mesh, pos_begin + index, true);
	}

	// 随机生成道具
	void updateGameItemAt(int index) {
		// 随机生成道具的位置，并确保其不与游戏方块在一定范围内碰撞检测为真
		float x, z;
		while(true){
			x = AuxiliaryFunc::randFloat(-4 * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale, 4 * ModelGround::block_scale + 0.5 * ModelGameBlock::game_block_scale);
			z = AuxiliaryFunc::randFloat(-4 * ModelGround::block_scale - 0.5 * ModelGameBlock::game_block_scale, 4 * ModelGround::block_scale + 0.5 * ModelGameBlock::game_block_scale);
			
			// 若完全不在范围内则为合法
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

		// 随机生成道具的属性
		int rand_res = AuxiliaryFunc::rand() % 10 + 1;
		if (1 <= rand_res && rand_res <= 3) {		// 以0.3的概率生成扣血道具
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_HP_DOWN;
		}else if(4 <= rand_res && rand_res <= 6){	// 以0.3的概率生成减速道具
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_SPEED_DOWN;
		}else if(7 <= rand_res && rand_res <= 8){	// 以0.2的概率生成回血道具
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_HP_UP;
		}else if(9 <= rand_res && rand_res <= 10){	// 以0.2的概率生成加速道具
			game_item_type[index] = GAME_ITEM_TYPE::ITEM_SPEED_UP;
		}
	}

	// 对游戏道具的碰撞检测
	int checkPointIsInItem(vec3 position) {
		for (int i = 0; i < NUMS_MODEL_GAME_ITEM_NUMS; i++) {
			// 维护最大值最小值
			float minx = game_item_position[i].x - GAME_ITEM_SCALE;
			float maxx = game_item_position[i].x + GAME_ITEM_SCALE;
			float minz = game_item_position[i].z - GAME_ITEM_SCALE;
			float maxz = game_item_position[i].z + GAME_ITEM_SCALE;
			float maxy = GAME_ITEM_SCALE + ModelGameBlock::game_block_offset_y;
			// 若在范围内则说明发生了碰撞
			if (minx <= position.x && position.x <= maxx && minz <= position.z && position.z <= maxz && position.y <= maxy) {
				return i;
			}
		}
		return -1;
	}
}

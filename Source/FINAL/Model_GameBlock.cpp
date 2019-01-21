// ��Ϸ����
#include "Auxiliary_H.h"
#include "Model.h"

namespace ModelGameBlock {

	float game_block_scale = 2;						// ��Ϸ��������ֵ
	float game_block_offset_y = 0.1;				// ��Ϸ������y���ƫ�Ʒ���

	/* ��Ϸ����ָ����� */
	int	game_block_pointed_index = -1;

	/* ��Ϸ�����Ƴ���� */
	int game_block_removed_step = 1;				// �Ƴ���������
	float game_block_removed_time = 0;				// �Ƴ�����ʱ��
	bool is_game_block_removed = false;				// ����Ƿ�����Ϸ���鱻�Ƴ�
	std::vector<int> game_block_removed_ls;			// ��¼��Ҫ���Ƴ�����Ϸ������±�
	const float GAME_BLOCK_REMOVED_MAX_TIME = 50;	// �Ƴ������������ʱ

	/* ��Ϸ���齻����� */
	int game_block_swaped_index[2] = { -1, -1 };	// ����������������±�
	int game_block_swaped_direction_x = 0;			// ���������������Ƿ�Ϊx��
	int game_block_swaped_direction_z = 0;			// ���������������Ƿ�Ϊz��
	float game_block_swaped_time = 0;				// �������������鶯��ʱ��
	bool is_game_block_swaped = false;				// ����Ƿ����ڽ��н���
	const float GAME_BLOCK_SWAPED_MAX_TIME = 40;	// ���������������ʱ

	/* ��Ϸ����ѡ����� */
	int game_block_selected_index = -1;				// ��ѡ�����Ϸ�����±�
	float game_block_selected_time = 0;				// ��ǰ��ɫ�仯����ʱ��
	bool  is_game_block_selected = false;			// ����Ƿ��з��鱻ѡ��

	
	const float PUSH_ROBOT_SPEED = 30.0;			// �ƿ������˵��ٶ�

	const int NUMS_GAME_BLOCKS_TYPE = 5;			// ��Ϸ������������
	const int NUMS_GAME_BLOCKS = 81;				// ��Ϸ��������

	int  game_block_type[NUMS_GAME_BLOCKS];			// ��Ϸ��������
	mat4 game_block_model_matrix[NUMS_GAME_BLOCKS];	// ��Ϸ����ģ�;���

	// �������Ͷ�Ӧ����������
	CoreImageTexture::TextureType game_block_tex_type[NUMS_GAME_BLOCKS_TYPE] = {
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_YELLOW,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_RED,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_GREEN,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_PURPLE,
		CoreImageTexture::TextureType::MODEL_GAMEBLOCK_PINK
	};

	int pos_begin;						// ��¼STL��������Ϸ�����е�һ��ͼ�ε�λ��

	std::vector<vec3> ls_v;
	std::vector<vec3> ls_n;
	std::vector<vec3i> ls_fs;
	std::vector<vec2> ls_vt;

	// ��(x,z)����ӳ�䵽�����±�index
	int getIndex(int x, int z) {
		x += 4, z += 4;
		return z * 9 + x;
	}

	// ��index��ȡ(x.z)����
	void getXZ(int index, int& x, int& z) {
		x = index % 9 - 4;
		z = index / 9 - 4;
	}

	// ����UV����
	void loadModelGameBlockTexCoord() {
		ls_vt.clear();
		int n = ls_v.size();
		for (int i = 0; i < n; i += 3) {
			ls_vt.push_back(vec2(0.5, 1));
			ls_vt.push_back(vec2(0, 0));
			ls_vt.push_back(vec2(1, 0));
		}
	}

	// ������Ϸ����
	void loadModelGameBlock() {
		AuxiliaryGenShape::genRectangularPyramid(ls_v, ls_n, ls_fs);
		loadModelGameBlockTexCoord();

		pos_begin = CoreMesh::painter.getSize();		// ��¼��һ������λ��

		// ���������Ϸ��������
		for (int i = 0; i < NUMS_GAME_BLOCKS; i++) {
			game_block_type[i] = AuxiliaryFunc::rand() % NUMS_GAME_BLOCKS_TYPE;
		}

		// �ؼ���������Ϸ����
		for (int z = -4; z <= 4; z++) {
			for (int x = -4; x <= 4; x++) {
				reloadModelGameBlockAt(x, z);
			}
		}
	}

	// ModelGameBlock::reloadModelGameBlockAt ���¼���λ��(x, z)����Ϸ����
	void reloadModelGameBlockAt(int x, int z, vec3 color, float rotation_y, float scale, vec3 translate_vec) {
		// ��ȡ��Ӧ�±�
		int index = getIndex(x, z);
		game_block_model_matrix[index] =
			Translate(x * ModelGround::block_scale, game_block_scale * 0.5 + game_block_offset_y, z * ModelGround::block_scale)	// �ƶ����������Ӧλ��
			* Translate(translate_vec) //ƽ��translate_vec����
			* RotateY(45) // ��ʼ��ת45��
			* RotateY(rotation_y)	// ��y����תrotation_y��
			* Scale(game_block_scale, game_block_scale, game_block_scale) //����game_block_scaleֵ
			* Scale(scale);	//���ݴ����������
		
		if (pos_begin + getIndex(x, z) == CoreMesh::painter.getSize()) {
			CoreMesh::Mesh* mesh = new CoreMesh::Mesh;
			mesh->setData(ls_v, ls_n, ls_fs, game_block_model_matrix[index], color);
			mesh->setTexture(game_block_tex_type[game_block_type[index]], ls_vt);
			CoreMesh::painter.addMesh(mesh, getIndex(x, z) + pos_begin, true);
		} else {
			CoreMesh::Mesh* mesh = CoreMesh::painter.getMeshAt(pos_begin + getIndex(x, z));
			mesh->model_matrix = game_block_model_matrix[index];
			mesh->tex_type = game_block_tex_type[game_block_type[index]];
			mesh->color = color;
		}
		
	}

	// ��ȡ��ѡ�����Ϸ���飨ģ���ӵ���
	int getIndexGameBlockPointed() {
		// ����������
		int max_time = 40;
		// �ӵ���Զ�˶�����
		float max_distance = 2.5 * ModelGround::block_scale + 0.5 * game_block_scale;
		// �ӵ��˶��ٶ�
		float v = max_distance / max_time;

		// ģ���˶�
		for (int iterate_time = 1; iterate_time <= max_time; iterate_time++) {
			// ��ǰλ��
			vec3 position = cam.position + v * iterate_time * cam.front;

			// ��֦�Ż�����������y�ķ�Χֱ�Ӽ�������
			if (position.y < game_block_offset_y || position.y > game_block_offset_y + game_block_scale) {
				continue;
			}

			// ��ȡ�����е���Ϸ����
			int x = -5, z = -5;
			for (int i = -4; i <= 4; i++) {
				if (i * ModelGround::block_scale - game_block_scale * 0.5 <= position.x &&
					position.x <= i * ModelGround::block_scale + game_block_scale * 0.5) {
					x = i;
				}
				if (i * ModelGround::block_scale - game_block_scale * 0.5 <= position.z &&
					position.z <= i * ModelGround::block_scale + game_block_scale * 0.5) {
					z = i;
				}
			}

			// �����У��򷵻ر����е���Ϸ�����±�
			if (x != -5 && z != -5) {
				return getIndex(x, z);
			}
		}
		// ��û���У�����-1
		return -1;
	}


	// ��������Ƿ��п���������Ϸ����
	bool checkGameBlockRowRemoved() {
		// ��ռ�¼��STL����
		game_block_removed_ls.clear();

		for (int z = -4; z <= 4; z++) {
			int cnt = 1;	// ��¼ֵĬ��Ϊ1
			for (int x = -3; x <= 4; x++) {
				int t1 = game_block_type[getIndex(x - 1, z)];
				int t2 = game_block_type[getIndex(x, z)];
				if (t1 == t2) {		// �����ڵ���ͬ
					cnt++;			// ���¼ֵ+1
				}
				else {			// �����¼ֵ����Ϊ1
					cnt = 1;
				}

				// ����3��������ͬ�������STL������
				if (cnt == 3) {
					game_block_removed_ls.push_back(getIndex(x - 2, z));
					game_block_removed_ls.push_back(getIndex(x - 1, z));
					game_block_removed_ls.push_back(getIndex(x, z));
				}
				// ������3������ǰ��ҲӦ�ñ�����STL������
				if (cnt > 3) {
					game_block_removed_ls.push_back(getIndex(x, z));
				}
			}
		}
		// ��STL�����ǿգ���˵���пɱ���������Ϸ����
		return !game_block_removed_ls.empty();
	}

	// ��������Ƿ��п���������Ϸ����,ͬ��
	bool checkGameBlockColRemoved() {
		game_block_removed_ls.clear();
		for (int x = -4; x <= 4; x++) {
			int cnt = 1;
			for (int z = -3; z <= 4; z++) {
				int t1 = game_block_type[getIndex(x, z - 1)];
				int t2 = game_block_type[getIndex(x, z)];
				if (t1 == t2) {
					cnt++;
				} else {
					cnt = 1;
				}

				if (cnt == 3) {
					game_block_removed_ls.push_back(getIndex(x, z - 2));
					game_block_removed_ls.push_back(getIndex(x, z - 1));
					game_block_removed_ls.push_back(getIndex(x, z));
				}
				if (cnt > 3) {
					game_block_removed_ls.push_back(getIndex(x, z));
				}
			}
		}
		return !game_block_removed_ls.empty();
	}

	// ����ѡ��ķ���
	void updateGameBlockPointed() {
		if (is_game_block_removed == true || is_game_block_swaped == true) {
			return;
		}

		// ����һ����ָ�򷽿飬����û�б�ѡ�еķ��飬��ָ���һ��ָ��ķ��鱾�����ɫ
		if (game_block_pointed_index != -1 && game_block_pointed_index != game_block_selected_index) {
			int x, z;
			getXZ(game_block_pointed_index, x, z);
			reloadModelGameBlockAt(x, z);
		}

		// ��ȡ���α�ָ��ķ���
		game_block_pointed_index = getIndexGameBlockPointed();

		// �������ڻ����뱻ѡ�е���Ϸ������ͬһ���������޸�
		if (game_block_pointed_index == -1 || game_block_pointed_index == game_block_selected_index) {
			return;
		}

		// ��������Ӧ�޸�
		int x, z;
		getXZ(game_block_pointed_index, x, z);
		reloadModelGameBlockAt(x, z, vec3(1.5, 1.5, 1.5));
	}

	// ���±�ѡ��ʱ����Ϸ����
	void updateGameBlockSelected() {
		// ������Ϸ�������ڱ��Ƴ�����ô������
		if (is_game_block_removed == true || is_game_block_swaped == true) {
			return;
		}
		
		int x, z;

		// ��ָ�����Ϸ��������һ�α�ѡ�����Ϸ���鲻ͬ
		if (game_block_pointed_index != game_block_selected_index) {
			// ����һ�α�ѡ��ķ�����ڣ���ȡ���������Ч��
			if (game_block_selected_index != -1) {
				getXZ(game_block_selected_index, x, z);
				reloadModelGameBlockAt(x, z);
				is_game_block_selected = false;
			}
			// �����ε�ѡ���ܷ�������
			if (!updateGameBlockIsSwaped()) {
				getXZ(game_block_pointed_index, x, z);
				is_game_block_selected = true;
				game_block_selected_index = game_block_pointed_index;
			}
		} else if(game_block_pointed_index != -1){
			// ��ѡ��ķ�������һ����ͬ����ôȡ����һ�����Ч��
			getXZ(game_block_pointed_index, x, z);
			reloadModelGameBlockAt(x, z);
			game_block_selected_index = -1;
			is_game_block_selected = false;
		}
	}

	// ��Ϸ�����ƿ�������
	void pushRobot(int game_block_removed_series_x, int game_block_removed_series_z) {
		// ����ʵ��(x,z)����
		float game_block_position_x = game_block_removed_series_x * ModelGround::block_scale;
		float game_block_position_z = game_block_removed_series_z * ModelGround::block_scale;
		// �����ƿ��ķ���
		vec3 game_block_push_direction = ModelRobot::move_position - vec3(game_block_position_x, 0, game_block_position_z);
		game_block_push_direction.y = 0;
		game_block_push_direction = normalize(game_block_push_direction);
		// ���»����˵�λ��
		ModelRobot::move_position += program_speed * PUSH_ROBOT_SPEED * game_block_push_direction;
		// ���»����˵�Ѫ��
		ModelRobot::robot_hp -= 0.05;

		if (ModelRobot::robot_hp <= 0.0) {
			ModelRobot::setRobotOver();
		}
	}

	// ������Ϸ������������
	void ProcessGameBlockRemove() {
		// ���㶯��ʱ�䣬�����ٶ�����ת�ٶ�
		float animation_time = GAME_BLOCK_REMOVED_MAX_TIME;
		float animation_scale_v = 1 / animation_time;
		float animation_rotation_v = 540 / animation_time;

		if (game_block_removed_step == 1) {		// ���ڶ�������һ
			if (game_block_removed_time <= animation_time) {	// ���˶���ʱ�仹δ����
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					// ���㵱ǰ����ֵ����תֵ
					float game_block_removed_scale = std::max(1 - animation_scale_v * game_block_removed_time, 0.001f);
					float game_block_removed_rotation = animation_rotation_v * game_block_removed_time;
					// ���·���
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					reloadModelGameBlockAt(game_block_removed_series_x, game_block_removed_series_z, vec3(1, 1, 1), game_block_removed_rotation, game_block_removed_scale);
				}
				// ʱ������
				game_block_removed_time += program_speed * 1.5;
			} else {	// ����������һ����
				// ת�Ƶ����������
				game_block_removed_step++;
				// �������ķ������������������
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					game_block_type[game_block_removed_series_index] = rand() % NUMS_GAME_BLOCKS_TYPE;
				}
			}
		}

		if (game_block_removed_step == 2) {	// ���ڶ��������
			// �������ʱ����y�������ƫ�ƣ�ʹ��Ϸ���������������˶�
			float time = game_block_removed_time - animation_time;
			float game_block_removed_translate_y = 150 * program_speed - 0.5 * 9.8 * time * time;
			// ����Ϸ���黹δ�ŵ�
			if (game_block_removed_translate_y >= game_block_offset_y) {
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					// �ؼ�����Ϸ����
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					reloadModelGameBlockAt(game_block_removed_series_x, game_block_removed_series_z, vec3(1.0, 1.0, 1.0), 0.0, 1.0, vec3(0, game_block_removed_translate_y, 0));
					// ����������ײ���ƿ�������
					if (ModelGameBlock::checkPointIsInGameBlock(ModelRobot::move_position +
						vec3(0, ModelRobot::TORSO_HEIGHT + ModelRobot::UPPER_LEG_HEIGHT + ModelRobot::LOWER_LEG_HEIGHT, 0), 0, game_block_removed_series_index)) {
						pushRobot(game_block_removed_series_x, game_block_removed_series_z);
					}
				}
				// ʱ������
				game_block_removed_time += program_speed * 1.2;
			} else {	// ���������������
				// �ؼ�����Ϸ����
				for (auto game_block_removed_series_index : game_block_removed_ls) {
					int game_block_removed_series_x, game_block_removed_series_z;
					getXZ(game_block_removed_series_index, game_block_removed_series_x, game_block_removed_series_z);
					reloadModelGameBlockAt(game_block_removed_series_x, game_block_removed_series_z, vec3(1.0, 1.0, 1.0), 0.0, 1.0, 0.0);
				}

				// ʱ����0��������һ����Ǳ�����Ϊ��
				game_block_removed_time = 0;
				game_block_removed_step = 1;
				is_game_block_removed = false;
			}
		}
	}

	// �����������
	void ProcessGameBlockRandomBoom() {
		// ���ѡ������Ϊ[1, 5]
		int nums = AuxiliaryFunc::rand() % 5 + 1;
		// ��¼���ѡ��ķ����±�
		std::vector<int> nums_series;
		// �������[0, NUMS_GAME_BLOCKS)������ΧΪ������Ϸ����
		AuxiliaryFunc::genRandomSeries(nums_series, 0, NUMS_GAME_BLOCKS - 1);

		// ���Ƿ�����Ϸ����������Ǳ���Ϊ��
		is_game_block_removed = true;
		// �����¼�����������±��STL������������������ɵ���Ϸ�����±�
		game_block_removed_ls.clear();
		for (int i = 0; i < nums; i++) {
			game_block_removed_ls.push_back(nums_series[i]);
		}
	}

	// ��ѡ�е���Ϸ����Ķ���Ч��
	void ProcessGameBlockSelected() {
		if (game_block_selected_index == -1) {
			return;
		}

		// �仯ֵ
		float delta_sita = 0.3 * sin(game_block_selected_time) + 1;
		vec3  delta_vec = vec3(delta_sita, delta_sita, delta_sita);
		// ������ɫ
		int x, z;
		getXZ(game_block_selected_index, x, z);
		reloadModelGameBlockAt(x, z, delta_vec);
		// ʱ������
		game_block_selected_time += program_speed * 0.2;
	}

	// ModelGameBlock::ProcessCoreGame ��Ϸ�����߼����лص�����
	void ProcessCoreGame() {
		if (ModelRobot::is_robot_alive == false) {
			return;
		}

		if (is_game_block_swaped) {	// ���з��齻�������������
			ProcessGameBlockSwap();
		} else if (is_game_block_removed) {	// �������з������������������������
			ProcessGameBlockRemove();
		} else if (AuxiliaryFunc::rand() % (int)(1000 / program_speed) == 0){	// ������һ�����ʽ����������
			ProcessGameBlockRandomBoom();
		} else {	// �������Ƿ��з���ɽ�����������������Ϸ���鱻ѡ��Ķ���
			updateGameBlockIsRemoved();
			if (is_game_block_selected) {
				ProcessGameBlockSelected();
			}
		}
	}

	// ������Ϸ���齻������
	void ProcessGameBlockSwap() {
		// ���㶯��ʱ�����ٶ�
		float animation_time = GAME_BLOCK_SWAPED_MAX_TIME;
		float animation_v = ModelGround::block_scale / animation_time;
		if (game_block_swaped_time <= animation_time) {		// �������ڶ���ʱ����
			// ����x����z���ƫ����
			float delta_x = game_block_swaped_direction_x * std::min(animation_v * game_block_swaped_time, ModelGround::block_scale);
			float delta_z = game_block_swaped_direction_z * std::min(animation_v * game_block_swaped_time, ModelGround::block_scale);
			float delta_rotation = animation_v * game_block_swaped_time;
			// �ؼ�����Ϸ����ģ��
			int x0, z0, x1, z1;
			getXZ(game_block_swaped_index[0], x0, z0);
			getXZ(game_block_swaped_index[1], x1, z1);
			reloadModelGameBlockAt(x0, z0, vec3(1.0, 1.0, 1.0), delta_rotation, 1, vec3(delta_x, 0, delta_z));
			reloadModelGameBlockAt(x1, z1, vec3(1.0, 1.0, 1.0), -delta_rotation, 1, vec3(-delta_x, 0, -delta_z));
			game_block_swaped_time += program_speed * 2;
		} else {	// ����������
			// �������������ͣ��������ƶ��������ؼ���ģ��
			int x0, z0, x1, z1;
			getXZ(game_block_swaped_index[0], x0, z0);
			getXZ(game_block_swaped_index[1], x1, z1);
			std::swap(game_block_type[game_block_swaped_index[0]], game_block_type[game_block_swaped_index[1]]);
			reloadModelGameBlockAt(x0, z0, vec3(1.0, 1.0, 1.0), 0.0, 1, vec3(0, 0, 0));
			reloadModelGameBlockAt(x1, z1, vec3(1.0, 1.0, 1.0), 0.0, 1, vec3(0, 0, 0));
			// ���ö���ʱ�䣬���Ƿ���н����ı�Ǳ�����Ϊ�٣����Ƿ�����Ƴ��ı�Ǳ�����Ϊ��
			game_block_swaped_time = 0;
			is_game_block_swaped = false;
			is_game_block_removed = true;
		}
	}

	// ���±��Ƴ��ķ���
	void updateGameBlockIsRemoved() {
		// ����Ƿ��пɱ��Ƴ��ķ���
		if (checkGameBlockRowRemoved() || checkGameBlockColRemoved()) {
			// ��Ǳ�������Ϊ��
			is_game_block_removed = true;
			// ��������
			ModelRobot::score += game_block_removed_ls.size();
			// ���¼Ƿְ����
			ModelBoard::ProcessScoreUpdate();
			// ������������
			ModelBomb::updateBombNums();
		}
	}

	// �жϷ����Ƿ�ɽ��������
	bool updateGameBlockIsSwaped() {
		// ��ǰָ�򷽿����һ��ָ�򷽿�����һ���������򲻼�������
		if (game_block_pointed_index == -1 || game_block_selected_index == -1) {
			return false;
		}

		// ��ȡ���������(x,z)����
		int x1, z1, x2, z2;
		getXZ(game_block_pointed_index, x1, z1);
		getXZ(game_block_selected_index, x2, z2);

		// ���������پ���
		int delta_x = (x1 - x2 > 0 ? x1 - x2 : x2 - x1);
		int delta_z = (z1 - z2 > 0 ? z1 - z2 : z2 - z1);

		// ���������򲻼�������
		if (!((delta_x == 1 && delta_z == 0) || (delta_x == 0 && delta_z == 1))) {
			return false;
		}

		// ���Ƚ���������������ͣ��������ܷ�����������������򲻼��������������Ҫ�������������������
		std::swap(game_block_type[game_block_pointed_index], game_block_type[game_block_selected_index]);
		if (!checkGameBlockColRemoved() && !checkGameBlockRowRemoved()) {
			std::swap(game_block_type[game_block_pointed_index], game_block_type[game_block_selected_index]);
			return false;
		}
		std::swap(game_block_type[game_block_pointed_index], game_block_type[game_block_selected_index]);

		// �ɽ��������ӷ�����������������������
		ModelRobot::score += ModelGameBlock::game_block_removed_ls.size();
		ModelBoard::ProcessScoreUpdate();
		ModelBomb::updateBombNums();
		
		// ��������������±�ֵ����������ʼֵ
		game_block_swaped_index[0] = game_block_pointed_index;
		game_block_swaped_index[1] = game_block_selected_index;
		game_block_swaped_direction_x = 1;
		game_block_swaped_direction_z = 0;
		// ��xֵ��zֵС�ķ��������һ�����0λ��
		if ((delta_z && z1 > z2) || (delta_x && x1 > x2)) {
			std::swap(game_block_swaped_index[0], game_block_swaped_index[1]);
		}
		// ���Ǵ���z��ķ�������ô��x�������Ϊ0��z�������Ϊ1
		// ��������ʼֵ
		if (delta_z) {
			std::swap(game_block_swaped_direction_x, game_block_swaped_direction_z);
		}
		// ���Ƿ����ڽ����ı�Ǳ�����Ϊ��
		is_game_block_swaped = true;
		game_block_selected_index = -1;
		return true;
	}

	// ModelGameBlock::checkPointIsInGameBlock �Ե�position���ڴ�Сƫ��Ϊdelta�����ⷽ�����ײ���
	int checkPointIsInGameBlock(vec3 position, float delta) {
		// ��ȡ�����ڵ��淽�������(x,z)
		int x = -5, z = -5;
		for (int i = -4; i <= 4; i++) {
			if (i * ModelGround::block_scale - 0.5 * ModelGround::block_scale <= position.x
				&& position.x <= i * ModelGround::block_scale + 0.5 * ModelGround::block_scale) {
				x = i;
				break;
			}
		}
		for (int i = -4; i <= 4; i++) {
			if (i * ModelGround::block_scale - 0.5 * ModelGround::block_scale <= position.z
				&& position.z <= i * ModelGround::block_scale + 0.5 * ModelGround::block_scale) {
				z = i;
				break;
			}
		}

		// ����λ��9x9�����ϣ��򷵻�-1����ʾ������
		if (x == -5 || z == -5) {
			return -1;
		}

		// ����ɨ����Χ3x3�����ڵ���Ϸ���飬����ײ���
		for (int i = std::max(x - 1, -4); i <= 4 && i <= x + 1; i++) {
			for (int j = std::max(z - 1, -4); j <= 4 && j <= z + 1; j++) {
				if (checkPointIsInGameBlock(position, delta, getIndex(i, j))) {
					return getIndex(i, j);
				}
			}
		}

		// ����ײ�򷵻�-1
		return -1;
	}

	// �Ե�position�����±�index����Ϸ�����Сƫ��Ϊdelta����ײ���
	bool checkPointIsInGameBlock(vec3 position, float delta, int index) {
		// ��ʼ��Ϸ��������
		vec4 game_block_position[8] = {
			vec4(-0.5, -0.5, -0.5, 1),
			vec4(-0.5, -0.5, 0.5, 1),
			vec4(-0.5, 0.5, -0.5, 1),
			vec4(-0.5, 0.5, 0.5, 1),
			vec4(0.5, -0.5, -0.5, 1),
			vec4(0.5, -0.5, 0.5, 1),
			vec4(0.5, 0.5, -0.5, 1),
			vec4(0.5, 0.5, 0.5, 1)
		};

		// x, z�������Сֵ���ֵ, y�������Сֵ
		float minx = 1e9, maxx = -1e9;
		float minz = 1e9, maxz = -1e9;
		float miny = 1e9;
		// ά���������ֵ
		for (int k = 0; k < 8; k++) {
			vec4 point = game_block_model_matrix[index] * game_block_position[k];
			minx = std::min(minx, point.x);
			maxx = std::max(maxx, point.x);
			minz = std::min(minz, point.z);
			maxz = std::max(maxz, point.z);
			miny = std::min(miny, point.y);
		}
		// �������У�˵��������ײ��������
		if (minx + delta <= position.x && position.x <= maxx - delta && minz + delta <= position.z &&
			position.z <= maxz - delta && position.y >= miny + delta) {
			return true;
		}
		return false;
	}

	// ��ʼ����Ϸ���������
	void initGameBlock() {
		game_block_pointed_index = -1;
		game_block_removed_step = 1;				
		game_block_removed_time = 0;				
		is_game_block_removed = false;				
		game_block_removed_ls.clear();			
		game_block_swaped_index[0] = game_block_swaped_index[0] = -1;	
		game_block_swaped_direction_x = 0;			
		game_block_swaped_direction_z = 0;			
		game_block_swaped_time = 0;				
		is_game_block_swaped = false;
												
		game_block_selected_index = -1;				
		game_block_selected_time = 0;				
		is_game_block_selected = false;			
	}
}

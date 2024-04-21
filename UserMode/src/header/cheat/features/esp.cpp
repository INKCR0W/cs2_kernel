#include <vector>

#include "../cheat.hpp"
#include "../../util/structs.hpp"
#include "../../offset/offset.hpp"

namespace cheat {
	void Cheat::esp() {
		using namespace cs2_dumper;
		view_matrix_t view_matrix = read_memory<view_matrix_t>(client_dll + offsets::client_dll::dwViewMatrix);
		Vec3 previous, current;

		uint8_t self_team = read_memory<uint8_t>(local_player_pawn + schemas::client_dll::C_BaseEntity::m_iTeamNum);

		for (const entity& current_entity : entity_list) {
			if (read_memory<uint8_t>(current_entity.pawn + schemas::client_dll::C_BaseEntity::m_iTeamNum) == self_team)
				continue;

			const uintptr_t game_scene_node = read_memory<uintptr_t>(current_entity.pawn + schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
			const uintptr_t bone_array = read_memory<uintptr_t>(game_scene_node + schemas::client_dll::CSkeletonInstance::m_modelState + schemas::client_dll::CGameSceneNode::m_vecOrigin);

			for (std::vector<int> current_group : bone_groups::all_groups) {
				previous = { 0.f, 0.f, 0.f };

				for (int currentBone : current_group) {
					current = read_memory<Vec3>(bone_array + currentBone * 32);

					if (previous.x == 0 && previous.y == 0) {
						previous = current;
						continue;
					}

					Vec2 current_screen_pos = world_to_screen(view_matrix, current);
					Vec2 previous_screen_pos = world_to_screen(view_matrix, previous);

					if (current_screen_pos.x > 0.f && previous_screen_pos.x > 0.f)
						overlay->draw_line(previous_screen_pos.x, previous_screen_pos.y, current_screen_pos.x, current_screen_pos.y, overlay::colors::write);

					previous = current;
				}
			}
		}

	}
}
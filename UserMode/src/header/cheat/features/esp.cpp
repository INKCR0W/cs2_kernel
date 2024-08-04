#include <vector>

#include "../cheat.hpp"
#include "../../util/structs.hpp"
#include "../../offset/offset.hpp"

namespace cheat {
	void Cheat::esp() {
		using namespace cs2_dumper;
		view_matrix_t view_matrix = read_memory<view_matrix_t>(client_dll + offsets::client_dll::dwViewMatrix);
		Vec3 previous, current;

		const uint8_t self_team = read_memory<uint8_t>(local_player_pawn + schemas::client_dll::C_BaseEntity::m_iTeamNum);

		auto draw_list = ImGui::GetBackgroundDrawList();

		for (const entity& current_entity : entity_list) {
			if (read_memory<uint8_t>(current_entity.pawn + schemas::client_dll::C_BaseEntity::m_iTeamNum) == self_team)
				continue;

			const uintptr_t game_scene_node = read_memory<uintptr_t>(current_entity.pawn + schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
			//const uintptr_t bone_array = read_memory<uintptr_t>(game_scene_node + schemas::client_dll::CSkeletonInstance::m_modelState + schemas::client_dll::CGameSceneNode::m_vecOrigin);
			const uintptr_t bone_array = read_memory<uintptr_t>(game_scene_node + 0x1F0);

			float left = 99999.f;
			float right = -1.f;
			float top = 99999.f;
			float bottom = -1.f;

			for (std::vector<int> current_group : bone_groups::all_groups) {
				previous = { 0.f, 0.f, 0.f };

				for (int currentBone : current_group) {
					current = read_memory<Vec3>(bone_array + currentBone * sizeof(BoneJointData));

					if (previous.x == 0 && previous.y == 0) {
						previous = current;
						continue;
					}

					Vec2 current_screen_pos = world_to_screen(view_matrix, current);
					Vec2 previous_screen_pos = world_to_screen(view_matrix, previous);

					if (current_screen_pos.x > 0.f) {
						//left = left > current_screen_pos.x ? current_screen_pos.x : left;
						right = right < current_screen_pos.x ? current_screen_pos.x : right;
						top = top > current_screen_pos.y ? current_screen_pos.y : top;
						//bottom = bottom < current_screen_pos.y ? current_screen_pos.y : bottom;
					}

					if (current_screen_pos.x > 0.f && previous_screen_pos.x > 0.f)
						draw_list->AddLine({ previous_screen_pos.x, previous_screen_pos.y }, { current_screen_pos.x, current_screen_pos.y }, IM_COL32(255, 0, 0, 255), 1.2);

					previous = current;
				}
			}

			if (top != -1.f) {
				/*overlay->draw_box((left + right) / 2, (top + bottom) / 2, right - left, bottom - top, overlay::colors::red);*/
				int current_health = read_memory<int>(current_entity.pawn + schemas::client_dll::C_BaseEntity::m_iHealth);

				float healthPercentage = static_cast<float>(current_health) / 100.f;

				uint8_t r = static_cast<uint8_t>((1.0f - healthPercentage) * 255);
				uint8_t g = static_cast<uint8_t>(healthPercentage * 255);

				draw_list->AddText({ right, top }, IM_COL32(r, g, 0, 255), std::to_string(current_health).c_str());
			}
		}

	}
}
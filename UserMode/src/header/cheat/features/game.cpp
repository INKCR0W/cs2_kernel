#include "../cheat.hpp"
#include "../../offset/offset.hpp"

namespace cheat {
	bool Cheat::update_entity() {
		using namespace cs2_dumper;

		const uintptr_t game_rules = read_memory<uintptr_t>(client_dll + offsets::client_dll::dwGameRules);

		in_match = read_memory<bool>(game_rules + schemas::client_dll::C_CSGameRules::m_bHasMatchStarted);

		if (!in_match)
			return false;


		local_player_pawn = read_memory<uintptr_t>(client_dll + offsets::client_dll::dwLocalPlayerPawn);
		local_player_controller = read_memory<uintptr_t>(client_dll + offsets::client_dll::dwLocalPlayerController);

		if (local_player_pawn == 0)
			return false;

		const auto entity_list_address = read_memory<uintptr_t>(client_dll + offsets::client_dll::dwEntityList);

		if (entity_list_address == 0)
			return false;

		entity_list.clear();

		for (unsigned long long i = 0; i < 64; ++i) {
			const uintptr_t entity_address = read_memory<uintptr_t>(entity_list_address + (0x8ULL * (i & 0x7FFF) >> 9) + 0x10);
			const uintptr_t entity_controller = read_memory<uintptr_t>(entity_address + 0x78ULL * (i & 0x1FF));

			if (entity_controller == local_player_controller) {
				continue;
			}

			if (!read_memory<bool>(entity_controller + schemas::client_dll::CCSPlayerController::m_bPawnIsAlive))
				continue;

			const uint32_t C_CSPlayerPawn = read_memory<std::uint32_t>(entity_controller + schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
			const uintptr_t pawn_entity_address = read_memory<uintptr_t>(entity_list_address + 0x8ULL * ((C_CSPlayerPawn & 0x7FFF) >> 9) + 0x10);
			const uintptr_t entity_pawn = read_memory<uintptr_t>(pawn_entity_address + 0x78ULL * (C_CSPlayerPawn & 0x1FF));

			if (read_memory<int>(entity_pawn + schemas::client_dll::C_BaseEntity::m_iHealth) < 0)
				continue;

			entity_list.push_back({ entity_controller, entity_pawn });
		}

		return true;
	}
}
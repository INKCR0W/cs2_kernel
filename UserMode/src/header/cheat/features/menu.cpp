#include <string>

#include "../cheat.hpp"

namespace cheat {
	void Cheat::menu() {
		using namespace features;


		for (const auto &iter : hotkeys) {
			if (GetAsyncKeyState(iter.vk_code)) {
				if (hotkey_pushed ^ iter.feature_code) {
					hotkey_pushed |= iter.feature_code;
					feature ^= iter.feature_code;
				}
			}
			else {
				hotkey_pushed &= (~iter.feature_code);
			}
		}


		if (!(feature & MENU)) {
			return;
		}

		auto draw_list = ImGui::GetBackgroundDrawList();

		draw_list->AddText({ 10, menu_render_y }, IM_COL32(124, 124, 124, 255), reinterpret_cast<const char*>(u8"[INSERT] 开/关本菜单"));
		draw_list->AddText({ 10, menu_render_y + 15 }, feature & ESP ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255), "MOUSE4 ESP");
	}
}
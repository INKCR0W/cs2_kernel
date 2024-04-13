#include <string>

#include "../cheat.hpp"

namespace cheat {
	using namespace overlay::colors;
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

		overlay->draw_text(10, menu_render_y, grey, "[INSERT] 开/关本菜单");
		overlay->draw_text(10, menu_render_y + 15, feature & ESP ? green : red, "F1 ESP");

		overlay->end_scene();
	}
}
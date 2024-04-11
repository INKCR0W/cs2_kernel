#include <string>

#include "../cheat.hpp"

namespace cheat {
	using namespace overlay::colors;
	void Cheat::menu() {
		using namespace features;
		using namespace hotkeys;

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

		overlay->draw_text(10, menu_render_y, grey, "[INSERT] TO OPEN/CLOSE MENU ÖÐÎÄ²âÊÔ");
		overlay->draw_text(10, menu_render_y + 15, feature & GLOW ? green : red, "F1 GLOW");
		overlay->draw_text(10, menu_render_y + 30, feature & ESP ? green : red, "F2 ESP");
		overlay->draw_text(10, menu_render_y + 45, feature & AIMBOT ? green : red, "F5 AIMBOT");
		overlay->draw_text(10, menu_render_y + 60, feature & NO_FLASH ? green : red, "F6 NO FLASH");

		overlay->end_scene();
	}
}
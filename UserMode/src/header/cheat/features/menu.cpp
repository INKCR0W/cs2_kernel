#include <string>

#include "../cheat.hpp"

namespace cheat {
	using namespace overlay::colors;
	void Cheat::menu() {
		using namespace features;

		for (const auto &iter : hotkeys) {
			if (GetAsyncKeyState(iter.vk_code)) {
				if (feature_hotkey ^ iter.feature_code) {
					feature_hotkey |= iter.feature_code;
					feature ^= iter.feature_code;
				}
			}
			else {
				feature_hotkey &= (~iter.feature_code);
			}
		}


		if (!(feature & MENU)) {
			return;
		}

		overlay->draw_text(10, menu_render_y, grey, "[INSERT] TO OPEN/CLOSE MENU");
		overlay->draw_text(10, menu_render_y + 15, feature & GLOW ? green : red, "F1 GLOW");
		overlay->draw_text(10, menu_render_y + 30, feature & ESP ? green : red, "F2 ESP");
		overlay->draw_text(10, menu_render_y + 45, feature & AIMBOT ? green : red, "F5 AIMBOT");
		overlay->draw_text(10, menu_render_y + 60, feature & NO_FLASH ? green : red, "F6 NO FLASH");


		overlay->draw_circle(screen_size_x / 2.f + 0.5f, screen_size_y / 2.f + 0.5f, 60, 50, red);
		// overlay->draw_box(screen_size_x / 2.f + 0.5, screen_size_y / 2.f + 0.5, 20, 20, red);
		overlay->draw_line(0, 0, screen_size_x, screen_size_y, red);


		overlay->end_scene();
	}
}
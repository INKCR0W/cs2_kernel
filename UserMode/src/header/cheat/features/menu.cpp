#include <string>

#include "../cheat.hpp"

namespace cheat {
	void Cheat::menu() {
		using namespace features;

		overlay->begin_scene();
		overlay->clear_scene();

		//if (GetAsyncKeyState(VK_INSERT)) {
		//	if (feature_hotkey ^ features::MENU) {
		//		feature_hotkey |= features::MENU;
		//		feature ^= features::MENU;
		//	}
		//}
		//else {
		//	feature_hotkey &= (~features::MENU);
		//}

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
			overlay->end_scene();
			return;
		}

		//constexpr int MENU = 1 << 0;
		//constexpr int GLOW = 1 << 1;
		//constexpr int ESP = 1 << 2;
		//constexpr int AIMBOT = 1 << 3;
		//constexpr int NO_FLASH = 1 << 4;

		//overlay->draw_text(10, menu_render_y, "PUSH INSERT TO OPEN/CLOSE THIS MENU", D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 15, (std::string("F1 GLOW ") + (feature & GLOW ? "ON" : "OFF")).c_str(), D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 30, (std::string("F2 ESP  ") + (feature & ESP ? "ON" : "OFF")).c_str(), D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 45, (std::string("F5 AIMBOT ") + (feature & AIMBOT ? "ON" : "OFF")).c_str(), D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 60, (std::string("F6 NO FLASH ") + (feature & NO_FLASH ? "ON" : "OFF")).c_str(), D2D1::ColorF(D2D1::ColorF::Red));

		//overlay->draw_text(10, menu_render_y, "[INSERT] TO OPEN/CLOSE MENU", feature & GLOW ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 15, "F1 GLOW", feature & ESP ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 30, "F2 ESP", feature & AIMBOT ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 45, "F5 AIMBOT", feature & NO_FLASH ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red));
		//overlay->draw_text(10, menu_render_y + 60, "F6 NO FLASH", D2D1::ColorF(D2D1::ColorF::Gray));

		overlay->draw_text(10, menu_render_y, D2D1::ColorF(D2D1::ColorF::Gray), "[INSERT] TO OPEN/CLOSE MENU");
		overlay->draw_text(10, menu_render_y + 15, feature & GLOW ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red), "F1 GLOW");
		overlay->draw_text(10, menu_render_y + 30, feature & ESP ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red), "F2 ESP");
		overlay->draw_text(10, menu_render_y + 45, feature & AIMBOT ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red), "F5 AIMBOT");
		overlay->draw_text(10, menu_render_y + 60, feature & NO_FLASH ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red), "F6 NO FLASH");
		overlay->end_scene();
	}
}
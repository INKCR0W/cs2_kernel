#pragma once

#include <array>

#include "../driver/driver.hpp"
#include "../overlay/overlay.hpp"

namespace cheat {
	namespace features {
		constexpr int MENU = 1 << 0;
		constexpr int GLOW = 1 << 1;
		constexpr int ESP = 1 << 2;
		constexpr int AIMBOT = 1 << 3;
		constexpr int NO_FLASH = 1 << 4;
	}

	class hotkey_data {
	public:
		int vk_code;
		int feature_code;

		hotkey_data(const int _vk, const int _feature) : vk_code(_vk), feature_code(_feature) {}
	};

	class Cheat : public driver::Driver {
	public:
		Cheat();
		Cheat(const Cheat& _) = delete;
		~Cheat();

		bool run();

	private:
		overlay::Overlay* overlay;
		std::array<hotkey_data, 5> hotkeys;

		const int screen_size_x;
		const int screen_size_y;
		const int menu_render_y;

		int feature;
		int feature_hotkey;

		void menu();
		void render();


		void print_error_info(const int error_code);
	};
}
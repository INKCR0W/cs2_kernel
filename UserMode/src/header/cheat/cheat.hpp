#pragma once

#include "../driver/driver.hpp"
#include "../overlay/overlay.hpp"

namespace cheat {
	namespace features {
		constexpr int MENU_OPEN = 1 << 0;
		constexpr int GLOW = 1 << 1;
		constexpr int ESP = 1 << 2;
		constexpr int AIMBOT = 1 << 3;
		constexpr int NO_FLASH = 1 << 4;
	}

	class Cheat : public driver::Driver {
	public:
		Cheat();
		Cheat(const Cheat& _) = delete;
		~Cheat();

		bool run();

	private:
		Overlay* overlay;



		void print_error_info(const int error_code);
	};
}
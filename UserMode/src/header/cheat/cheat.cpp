#include <iostream>
#include <thread>
#include <chrono>

#include "cheat.hpp"
// L"Consolas", 14.f
namespace cheat {
	Cheat::Cheat() : Driver(L"\\\\.\\BabyDriver", L"cs2.exe"), overlay(new overlay::Overlay()),
		screen_size_x(static_cast<float>(GetSystemMetrics(SM_CXSCREEN))), screen_size_y(static_cast<float>(GetSystemMetrics(SM_CYSCREEN))),
		menu_render_y(screen_size_y / 3), client_dll(0), in_match(false), local_player_pawn(0), local_player_controller(0), entity_list({}),
		feature(features::MENU), hotkey_pushed(0), 
		hotkeys({
		hotkey_data({VK_INSERT, features::MENU}),
		hotkey_data({VK_F9, features::ESP})
			})
	{
		std::cout
			<< R"(  ____      _    __   __   ____     _____    _   _    ____     _____ )" << "\n"
			<< R"( / ___|    / \   \ \ / /  / ___|   | ____|  | \ | |  / ___|   | ____|)" << "\n"
			<< R"(| |  _    / _ \   \ V /   \___ \   |  _|    |  \| |  \___ \   |  _|)" << "\n"
			<< R"(| |_| |  / ___ \   | |     ___) |  | |___   | |\  |   ___) |  | |___)" << "\n"
			<< R"( \____| /_ /  \_\  |_|    |____/   |_____|  |_| \_|  |____/   |_____|)" << "\n\n\n";

		if (!this->isAttached()) {
			print_error_info(this->getError());
			system("pause");
			exit(1);
		}

		std::cout << "[+] Attachment successful.\n";

		if (client_dll = get_module_base(L"client.dll"); client_dll != 0) {
			std::cout << "[+] Client.dll found, address : 0x" << std::hex << std::uppercase << client_dll << "\n";
		}
		else {
			std::cerr << "[-] Failed to find Client.dll.\n";
			system("pause");
			exit(1);
		}

		if (!overlay->init()) {
			system("pause");
			exit(1);
		}

		// std::cout << "[+] Overlay initialization successful, CS2 window : 0x" << std::hex << std::uppercase << overlay->window() << "\n";

		std::cout << "[+] All the shit is done, press DELETE to stop the program.\n";
	}

	Cheat::~Cheat() {
		delete overlay;
	}

	Vec2 Cheat::world_to_screen(view_matrix_t matrix, Vec3 position) const {
		float View = 0.f;
		float SightX = screen_size_x / 2.f;
		float SightY = screen_size_y / 2;

		View = matrix[3][0] * position.x + matrix[3][1] * position.y + matrix[3][2] * position.z + matrix[3][3];

		if (View <= 0.01)
			return { -1.0f, -1.0f };

		float final_x = SightX + (matrix[0][0] * position.x + matrix[0][1] * position.y + matrix[0][2] * position.z + matrix[0][3]) / View * SightX;
		float final_y = SightY - (matrix[1][0] * position.x + matrix[1][1] * position.y + matrix[1][2] * position.z + matrix[1][3]) / View * SightY;

		return { final_x, final_y };
	}

	void Cheat::print_error_info(const int error_code) {
		const auto error = GetLastError();

		switch (error_code)
		{
		case driver::error_codes::ACCESS:
			break;

		case driver::error_codes::GET_DRIVER_ERROR:
			std::cerr << "[-] Failed to create driver handler: " << error << "\n";
			break;

		case driver::error_codes::GET_PROCESSID_ERROR:
			std::cerr << "[-] Failed to find cs2 process: " << error << "\n";
			break;

		default:
			std::cerr << "[-] Unknown error: " << error << "\n";
			break;
		}
	}

	void Cheat::render() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(15)); // 

			if (GetAsyncKeyState(VK_DELETE))
				break;

			//if (overlay->window() != GetForegroundWindow()) {
			//	overlay->clear_screen();
			//	continue;
			//}

			update_entity();

			overlay->begin_scene();
			overlay->clear_scene();

			menu();
			if (feature & features::ESP)
				esp();

			overlay->end_scene();
		}
	}

	void Cheat::aimbot() {

	}

	bool Cheat::run() {
		std::thread t_render(&Cheat::render, this);
		std::thread t_aimbot(&Cheat::aimbot, this);

		t_render.join();
		t_aimbot.join();

		return true;
	}


}
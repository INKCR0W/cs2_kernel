#include <iostream>
#include <thread>
#include <chrono>

#include "cheat.hpp"
// L"Consolas", 14.f
namespace cheat {
	Cheat::Cheat() : Driver(L"\\\\.\\BabyDriver", L"cs2.exe"), overlay(new overlay::Overlay()), feature(features::MENU),
		screen_size_x(GetSystemMetrics(SM_CXSCREEN)), screen_size_y(GetSystemMetrics(SM_CYSCREEN)), menu_render_y(screen_size_y / 3), feature_hotkey(0),
		hotkeys({ hotkey_data(VK_INSERT, features::MENU), hotkey_data(VK_F1, features::GLOW), hotkey_data(VK_F2, features::ESP), hotkey_data(VK_F5, features::AIMBOT), hotkey_data(VK_F6, features::NO_FLASH) })
	{
		if (!this->isAttached()) {
			print_error_info(this->getError());
			system("pause");
			exit(1);
		}

		std::cout << "[+] Attachment successful.\n";

		if (!overlay->init()) {
			system("pause");
			exit(1);
		}
	}

	Cheat::~Cheat() {
		delete overlay;
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
			if (GetAsyncKeyState(VK_END))
				break;

			overlay->begin_scene();
			overlay->clear_scene();

			menu();

			overlay->end_scene();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	bool Cheat::run() {
		std::thread t_render(&Cheat::render, this);

		t_render.join();

		return true;
	}


}
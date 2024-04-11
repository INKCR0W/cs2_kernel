#include <iostream>
#include <thread>
#include <chrono>

#include "cheat.hpp"
// L"Consolas", 14.f
namespace cheat {
	Cheat::Cheat() : Driver(L"\\\\.\\BabyDriver", L"cs2.exe"), overlay(new overlay::Overlay()),
		screen_size_x(static_cast<float>(GetSystemMetrics(SM_CXSCREEN))), screen_size_y(static_cast<float>(GetSystemMetrics(SM_CYSCREEN))),
		menu_render_y(screen_size_y / 3)
		//feature(hotkeys::MENU), feature_hotkey(0), hotkey_pushed(0),
		//hotkeys(
		//	{
		//		hotkey_data(VK_ADD, hotkeys::MENU),
		//		hotkey_data(VK_NUMPAD0, hotkeys::PAD_0),
		//		hotkey_data(VK_NUMPAD1, hotkeys::PAD_1),
		//		hotkey_data(VK_NUMPAD2, hotkeys::PAD_2),
		//		hotkey_data(VK_NUMPAD4, hotkeys::PAD_4),
		//		hotkey_data(VK_NUMPAD5, hotkeys::PAD_5),
		//		hotkey_data(VK_NUMPAD6, hotkeys::PAD_6),
		//		hotkey_data(VK_NUMPAD7, hotkeys::PAD_7),
		//		hotkey_data(VK_NUMPAD8, hotkeys::PAD_8),
		//	})
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
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (GetAsyncKeyState(VK_DELETE))
				break;

			if (overlay->window() != GetForegroundWindow()) {
				overlay->clear_screen();
				continue;
			}

			overlay->begin_scene();
			overlay->clear_scene();

			menu();

			overlay->end_scene();
		}
	}

	bool Cheat::run() {
		std::thread t_render(&Cheat::render, this);

		t_render.join();

		return true;
	}


}
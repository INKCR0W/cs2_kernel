#include <iostream>
#include <thread>
#include <chrono>

#include "cheat.hpp"
// L"Consolas", 14.f
namespace cheat {
	Cheat::Cheat() : Driver(L"\\\\.\\BabyDriver", L"cs2.exe"), overlay(new overlay::Overlay()),
		screen_size_x(static_cast<float>(GetSystemMetrics(SM_CXSCREEN))), screen_size_y(static_cast<float>(GetSystemMetrics(SM_CYSCREEN))),
		menu_render_y(screen_size_y / 3),
		client_dll(0), in_match(false), local_player_pawn(0), local_player_controller(0), entity_list({})
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

		if (!overlay->init()) {
			system("pause");
			exit(1);
		}

		std::cout << "[+] Overlay initialization successful, CS2 window : 0x" << std::hex << std::uppercase << overlay->window() << "\n";

		std::cout << "[+] All the shit is done, press DELETE to stop the program.\n";
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
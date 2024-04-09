#include <iostream>
#include <thread>
// #include <WinUser.h>

#include "cheat.hpp"

// , screen_size_x(GetSystemMetrics(SM_CXSCREEN)), screen_size_y(GetSystemMetrics(SM_CYSCREEN))
namespace cheat {
	Cheat::Cheat() : Driver(L"\\\\.\\BabyDriver", L"cs2.exe"), overlay(new Overlay(L"Consolas", 14.f)), feature(1 << features::MENU) {
		if (!this->isAttached()) {
			print_error_info(this->getError());
			system("pause");
			exit(1);
		}

		std::cout << "[+] Attachment successful.\n";

		if (!overlay->init()) {
			std::cout << "[-] Failed to initialization overlay.\n";
			system("pause");
			exit(1);
		}

		if (!overlay->startup_d2d()) {
			std::cout << "[-] Failed to startup Direct2D.\n";
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
			std::cout << "[-] Failed to create driver handler: " << error << "\n";
			break;

		case driver::error_codes::GET_PROCESSID_ERROR:
			std::cout << "[-] Failed to find cs2 process: " << error << "\n";
			break;

		default:
			std::cout << "[-] Unknown error: " << error << "\n";
			break;
		}
	}

	void Cheat::menu() {
		overlay->begin_scene();
		overlay->clear_scene();
		overlay->draw_text(10, 300, "LIKDAHSGBOLIASWEKJHFGDI", D2D1::ColorF(D2D1::ColorF::Red));
		overlay->end_scene();
	}

	void Cheat::render() {
		while (true) {
			menu();
			if (GetAsyncKeyState(VK_END))
				break;
		}
	}

	bool Cheat::run() {
		std::thread t_render(&Cheat::render, this);

		t_render.join();

		return true;
	}


}
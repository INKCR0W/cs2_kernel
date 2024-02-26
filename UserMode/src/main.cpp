#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#include "header/client.dll.hpp"
#include "header/offsets.hpp"
#include "header/driver.hpp"


static void print_error_info(const int error_code) {
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


int main() {
	using driver::driver;

	driver myDriver(L"\\\\.\\BabyDriver", L"cs2.exe");

	if (!myDriver.isAttached()) {
		print_error_info(myDriver.getError());
		std::cin.get();
		return 1;
	}

	std::cout << "[+] Attachment successful.\n";

	if (const std::uintptr_t client = myDriver.get_module_base(L"client.dll"); client != 0) {
		std::cout << "[+] Client.dll found, address : 0x" << std::hex << std::uppercase << client << "\n";
		std::cout << "[+] Press END to stop this process.\n";

		while (true) {
			Sleep(1);
			if (GetAsyncKeyState(VK_END))
				break;

			const auto local_player_pawn = myDriver.read_memory<std::uintptr_t>(client + client_dll::dwLocalPlayerPawn);

			if (local_player_pawn == 0)
				continue;

			const auto flags = myDriver.read_memory<std::uint32_t>(local_player_pawn + C_BaseEntity::m_fFlags);

			const bool in_air = flags & (1 << 0);
			const bool space_pressed = GetAsyncKeyState(VK_SPACE);
			const auto force_jump = myDriver.read_memory<DWORD>(client + client_dll::dwForceJump);

			if (space_pressed && in_air) {
				Sleep(5);
				myDriver.write_memory(client + client_dll::dwForceJump, 65537);
			}
			else if (space_pressed && !in_air) {
				myDriver.write_memory(client + client_dll::dwForceJump, 256);
			}
			else if (!space_pressed && force_jump == 65537) {
				myDriver.write_memory(client + client_dll::dwForceJump, 256);
			}
		}
	}
	else {
		std::cout << "[-] Failed to find client.dll " << GetLastError() << "\n";
	}

	std::cout << "[*] Process stopped. Press any key to close this window.\n";
	std::cin.get();
	return 0;
}
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#include "client.dll.hpp"
#include "offsets.hpp"

static DWORD get_process_id(const wchar_t* process_name) {
	DWORD process_id = 0;

	const HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snap_shot == INVALID_HANDLE_VALUE)
		return process_id;

	PROCESSENTRY32 entry = {};
	entry.dwSize = sizeof(decltype(entry));

	if (Process32First(snap_shot, &entry)) {
		do {
			if (_wcsicmp(process_name, entry.szExeFile) == 0) {
				process_id = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(snap_shot, &entry));
	}

	CloseHandle(snap_shot);

	return process_id;
}

static std::uintptr_t get_module_base(const DWORD pid, const wchar_t* module_name) {
	std::uintptr_t module_base = 0;

	const HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (snap_shot == INVALID_HANDLE_VALUE)
		return module_base;

	MODULEENTRY32 entry = {};
	entry.dwSize = sizeof(decltype(entry));

	if (Module32First(snap_shot, &entry)) {
		do {
			if (wcsstr(module_name, entry.szModule) != nullptr) {
				module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		} while (Module32Next(snap_shot, &entry));
	}

	CloseHandle(snap_shot);

	return module_base;
}

namespace driver {
	namespace codes {
		// Used to setup the driver.
		constexpr ULONG attach =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		// Read process memory.
		constexpr ULONG read =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		// Write process memory.
		constexpr ULONG write =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
	}  // namespace codes

	// Shared between user mode & kernel mode.
	struct Request {
		HANDLE process_id;

		PVOID target;
		PVOID buffer;

		SIZE_T size;
		SIZE_T return_size;
	};

	bool attach_to_process(HANDLE driver_handle, const DWORD pid) {
		Request r;
		r.process_id = reinterpret_cast<HANDLE>(pid);

		return DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
	}

	template <typename T>
	T read_memory(HANDLE driver_handle, const std::uintptr_t addr) {
		T temp = {};

		Request r;
		r.target = reinterpret_cast<PVOID>(addr);
		r.buffer = &temp;
		r.size = sizeof(T);

		DeviceIoControl(driver_handle, codes::read, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);

		return temp;
	}

	template <typename T>
	void write_memory(HANDLE driver_handle, const std::uintptr_t addr, const T& value) {
		Request r;
		r.target = reinterpret_cast<PVOID>(addr);
		r.buffer = (PVOID)&value;
		r.size = sizeof(T);

		DeviceIoControl(driver_handle, codes::write, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
	}
}  // namespace driver

int main() {
	const HANDLE myDriver = CreateFile(L"\\\\.\\BabyDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (myDriver == INVALID_HANDLE_VALUE) {
		const auto error = GetLastError();
		std::cout << "Failed to create driver handler: " << error << "\n";

		std::cin.get();
		return 1;
	}

	const DWORD pid = get_process_id(L"cs2.exe");

	if (pid == 0) {
		std::cout << "Failed to find cs2 process.\n";
		std::cin.get();
		return 1;
	}

	if (driver::attach_to_process(myDriver, pid) == true) {
		std::cout << "Attachment successful.\n";

		if (const std::uintptr_t client = get_module_base(pid, L"client.dll"); client != 0) {
			std::cout << "Client.dll found, address : 0x" << std::hex << std::uppercase <<  client << "\n";
			std::cout << "Press END to stop this process.\n";

			while (true) {
				Sleep(1);
				if (GetAsyncKeyState(VK_END))
					break;

				const auto local_player_pawn = driver::read_memory<std::uintptr_t>(myDriver, client + client_dll::dwLocalPlayerPawn);

				if (local_player_pawn == 0)
					continue;

				const auto flags = driver::read_memory<std::uint32_t>(myDriver, local_player_pawn + C_BaseEntity::m_fFlags);

				const bool in_air = flags & (1 << 0);
				const bool space_pressed = GetAsyncKeyState(VK_SPACE);
				const auto force_jump = driver::read_memory<DWORD>(myDriver, client + client_dll::dwForceJump);

				if (space_pressed && in_air) {
					Sleep(5);
					driver::write_memory(myDriver, client + client_dll::dwForceJump, 65537);
				}
				else if (space_pressed && !in_air) {
					driver::write_memory(myDriver, client + client_dll::dwForceJump, 256);
				}
				else if (!space_pressed && force_jump == 65537) {
					driver::write_memory(myDriver, client + client_dll::dwForceJump, 256);
				}

				const auto local_player_controller = driver::read_memory<std::uintptr_t>(myDriver, client + client_dll::dwLocalPlayerController);

				if (local_player_controller == 0)
					continue;

				driver::write_memory(myDriver, local_player_controller + CCSPlayerController::m_iPing, 999);
			}
		}
		else {
			std::cout << "Failed to find client.dll.\n";
		}
	}

	CloseHandle(myDriver);

	std::cout << "Process stopped. Press any key to close this window.\n";

	std::cin.get();
	return 0;
}
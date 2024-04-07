/*
*  ┏┓　　　┏┓
*┏┛┻━━━┛┻┓
*┃              ┃ 　
*┃　　　━　　　┃
*┃　┳┛　┗┳　┃
*┃              ┃
*┃　　　┻　　　┃
*┃              ┃
*┗━┓　　　┏━┛
*　　┃　　　┃神兽保佑		THE MYTHICAL BEAST BLESS
*　　┃　　　┃代码无BUG！	THE CODE WITHOUT BUGS!
*　　┃　　　┗━━━┓
*　　┃              ┣┓
*　　┃              ┏┛
*　　┗┓┓┏━┳┓┏┛
*　　　┃┫┫　┃┫┫
*　　　┗┻┛　┗┻┛ 　
*/


#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>

#include "header/client.dll.hpp"
#include "header/offsets.hpp"
#include "header/driver.hpp"
#include "header/buttons.hpp"
#include "header/structs.hpp"

constexpr float M_PI = 3.14159265358979;


template <typename T>
void Log(T val) {
	std::cout << val << "\n";
}


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
			Sleep(10);
			if (GetAsyncKeyState(VK_END))
				break;

			if (!GetAsyncKeyState(VK_XBUTTON1))
				continue;

			// system("cls");

			//const auto local_player_pawn = myDriver.read_memory<std::uintptr_t>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);

			//if (local_player_pawn == 0)
			//	continue;

			//const auto flags = myDriver.read_memory<std::uint32_t>(local_player_pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);

			//const bool in_air = flags & (1 << 0);
			//const bool space_pressed = GetAsyncKeyState(VK_SPACE);
			//const auto force_jump = myDriver.read_memory<DWORD>(client + cs2_dumper::buttons::jump);

			//if (space_pressed && in_air) {
			//	Sleep(5);
			//	myDriver.write_memory(client + cs2_dumper::buttons::jump, 65537);
			//}
			//else if (space_pressed && !in_air) {
			//	myDriver.write_memory(client + cs2_dumper::buttons::jump, 256);
			//}
			//else if (!space_pressed && force_jump == 65537) {
			//	myDriver.write_memory(client + cs2_dumper::buttons::jump, 256);
			//}

			const auto local_player_pawn = myDriver.read_memory<std::uintptr_t>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
			const auto local_player_controller = myDriver.read_memory<std::uintptr_t>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerController);
			const auto view_angle_addr = client + cs2_dumper::offsets::client_dll::dwViewAngles;

			if (local_player_pawn == 0 || view_angle_addr == 0)
				continue;

			const auto entity_list = myDriver.read_memory<std::uintptr_t>(client + cs2_dumper::offsets::client_dll::dwEntityList);

			for (unsigned long long i = 0; i < 64; ++i) {
				const std::uintptr_t entry_address = myDriver.read_memory<std::uintptr_t>(entity_list + 0x8 * ((i & 0x7FFF) >> 9) + 0x10);
				const std::uintptr_t entry_controller = myDriver.read_memory<std::uintptr_t>(entry_address + 0x78 * (i & 0x1FF));

				if (entry_controller == local_player_controller)
					continue;

				if (!myDriver.read_memory<bool>(entry_controller + cs2_dumper::schemas::client_dll::CCSPlayerController::m_bPawnIsAlive))
					continue;

				const uint32_t C_CSPlayerPawn = myDriver.read_memory<std::uint32_t>(entry_controller + cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
				const std::uintptr_t pawn_entry_address = myDriver.read_memory<std::uintptr_t>(entity_list + 0x8 * ((C_CSPlayerPawn & 0x7FFF) >> 9) + 0x10);
				const std::uintptr_t entry_pawn = myDriver.read_memory<std::uintptr_t>(pawn_entry_address + 0x78 * (C_CSPlayerPawn & 0x1FF));

				if (myDriver.read_memory<int>(entry_pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth) < 0)
					continue;

				const std::uintptr_t game_scene_node = myDriver.read_memory<std::uintptr_t>(entry_pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
				const std::uintptr_t bone_array_address = myDriver.read_memory<std::uintptr_t>(game_scene_node + cs2_dumper::schemas::client_dll::CSkeletonInstance::m_modelState + cs2_dumper::schemas::client_dll::CGameSceneNode::m_vecOrigin);

				Vec2 view_angle = myDriver.read_memory<Vec2>(view_angle_addr);

				//std::string pawn_name;
				//const std::uintptr_t pawn_name_address = myDriver.read_memory<std::uintptr_t>(entry_controller + cs2_dumper::schemas::client_dll::CCSPlayerController::m_sSanitizedPlayerName);
				//if (pawn_name_address) {
				//	char buf[260] = {};
				//	myDriver.read_memory_size(pawn_name_address, buf, 260);
				//	pawn_name = std::string(buf);
				//}
				//else {
				//	pawn_name = "Unknow";
				//}
				// Log(pawn_name);


				Vec3 head_pos = myDriver.read_memory<Vec3>(bone_array_address + 6 * 32);
				Vec3 local_pos = myDriver.read_memory<Vec3>(local_player_pawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_vecLastClipCameraPos);


				float yaw = 0;
				float pitch = 0;
				float distance = 0;
	
				Vec3 OppPos;

				OppPos = head_pos - local_pos;
				distance = sqrt(pow(OppPos.x, 2) + pow(OppPos.y, 2));
				yaw = atan2(OppPos.y, OppPos.x) * 180 / M_PI;
				pitch = -atan2(OppPos.z, distance) * 180 / M_PI;

				if (yaw < -180)
					yaw += 360;
				else if (yaw > 180)
					yaw -= 360;

				if (pitch < -89)
					pitch = -89;
				else if (pitch > 89)
					pitch = 89;

				Vec2 target = { pitch, yaw };

				//Log(yaw);
				//Log(pitch);

				myDriver.write_memory(view_angle_addr, target);
				break;
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
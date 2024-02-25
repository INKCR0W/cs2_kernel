#include "driver.hpp"


namespace driver {
	DWORD driver::get_process_id(const wchar_t* process_name) {
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

	std::uintptr_t driver::get_module_base(const DWORD pid, const wchar_t* module_name) {
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

	driver::driver() : driver_handle(nullptr), pid(0), attached(false) {}

	driver::driver(const wchar_t* driver_path) : driver(driver_path, static_cast<DWORD>(0)) {}

	driver::driver(const wchar_t* driver_path, const wchar_t* process_name) : driver(driver_path, get_process_id(process_name)) {}

	driver::driver(const wchar_t* driver_path, const DWORD pid) : pid(pid), attached(false) {
		this->driver_handle = CreateFile(driver_path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (this->driver_handle == INVALID_HANDLE_VALUE)
			return;

		if (pid == 0)
			return;

		Request r;
		r.process_id = reinterpret_cast<HANDLE>(pid);

		if (DeviceIoControl(this->driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr)) {
			this->attached = true;
		}
	}

	driver::~driver() {
		CloseHandle(driver_handle);
	}

	bool driver::setDriver(const wchar_t* driver_path) {
		this->driver_handle = CreateFile(driver_path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		return (this->driver_handle != INVALID_HANDLE_VALUE);
	}

	bool driver::attach(const wchar_t* process_name) {
		const DWORD pid = this->get_process_id(process_name);

		if (pid == 0)
			return false;

		return driver::attach(pid);
	}

	bool driver::attach(const DWORD pid) {
		Request r;
		r.process_id = reinterpret_cast<HANDLE>(pid);

		this->attached = DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);

		return this->attached;
	}

	const HANDLE driver::_driver() const {
		return this->driver_handle;
	}

	const DWORD driver::_pid() const {
		return this->pid;
	}

	const bool driver::isAttached() const {
		return this->attached;
	}
}  // namespace driver
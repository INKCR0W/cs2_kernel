#include <ntifs.h>
#include <wdm.h>

typedef unsigned char BYTE;

extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);

	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress,
											 SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);

	//NTKERNELAPI PPEB NTAPI PsGetProcessPeb(IN PEPROCESS Process);
	NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(IN PEPROCESS Process);
	NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process(IN PEPROCESS Process);

	typedef NTSTATUS(*PsLookupProcessByProcessNamePtr)(IN PUNICODE_STRING ProcessName, OUT PEPROCESS* Process);
	typedef PPEB(*PsGetProcessPebPtr)(IN PEPROCESS Process);

	typedef void(__stdcall* PPS_POST_PROCESS_INIT_ROUTINE)(void);
}

typedef struct _PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID Reserved4[3];
	PVOID AtlThunkSListPtr;
	PVOID Reserved5;
	ULONG Reserved6;
	PVOID Reserved7;
	ULONG Reserved8;
	ULONG AtlThunkSListPtr32;
	PVOID Reserved9[45];
	BYTE Reserved10[96];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved11[128];
	PVOID Reserved12[1];
	ULONG SessionId;
} PEB, * PPEB;


typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    PVOID SectionPointer;
    ULONG CheckSum;
    ULONG TimeDateStamp;
    PVOID LoadedImports;
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;




PsLookupProcessByProcessNamePtr PsLookupProcessByProcessName = NULL;
PsGetProcessPebPtr PsGetProcessPeb = NULL;






void debug_print(PCSTR text) {
#ifndef DEBUG
	UNREFERENCED_PARAMETER(text);
#endif // !DEBUG

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

namespace driver {
	namespace codes {
		// Used to setup the driver.
		constexpr ULONG attach =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x676, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		constexpr ULONG attach2 =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x677, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		// Read process memory.
		constexpr ULONG read =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x678, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		
		// Write process memory.
		constexpr ULONG write =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x679, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		constexpr ULONG get_moudle =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x680, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
	}  // namespace codes

	// Shared between user mode & kernel mode.
	struct Request {
		HANDLE process_id;

		PVOID target;
		PVOID buffer;

		SIZE_T size;
		SIZE_T return_size;
	};

	NTSTATUS AttachProcess(PEPROCESS* target_process) {
		NTSTATUS Status = STATUS_UNSUCCESSFUL;
		PEPROCESS Process = NULL;
		HANDLE process_id = 0;

		if (PsLookupProcessByProcessName)
		{
			UNICODE_STRING game_process_name = {};
			RtlInitUnicodeString(&game_process_name, L"cs2.exe");
			Status = PsLookupProcessByProcessName(&game_process_name, &Process);
			if (NT_SUCCESS(Status))
			{
				process_id = PsGetProcessId(Process);
				ObDereferenceObject(Process);
			}
		}

		if (!NT_SUCCESS(Status))
		{
			return Status;
		}

		return PsLookupProcessByProcessId(process_id, target_process);
	}


	NTSTATUS GetModuleBaseProcess(__in PEPROCESS Process, __in LPCWSTR ModuleName, __out ULONG64* Module_Base) {
		//NTSTATUS Status = STATUS_UNSUCCESSFUL;

		//UNICODE_STRING module_name;
		//RtlInitUnicodeString(&module_name, ModuleName);

		//if (Process != NULL)
		//{
		//	PPEB pPeb = (PPEB)PsGetProcessPeb(Process);

		//	if (pPeb != NULL)
		//	{
		//		PPEB_LDR_DATA pLdr = pPeb->Ldr;

		//		if (pLdr != NULL)
		//		{
		//			PLIST_ENTRY pListEntry = pLdr->InLoadOrderModuleList.Flink;
		//			while (pListEntry != &pLdr->InLoadOrderModuleList)
		//			{
		//				PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		//				if (RtlEqualUnicodeString(&pEntry->BaseDllName, &module_name, TRUE))
		//				{
		//					*Module_Base = reinterpret_cast<ULONG64>(pEntry->DllBase);
		//					return STATUS_SUCCESS;
		//				}

		//				pListEntry = pListEntry->Flink;
		//			}
		//		}
		//	}
		//}

		//return Status;

		NTSTATUS status = STATUS_SUCCESS;
		UNICODE_STRING moduleNameUnicode;
		PVOID moduleBase = NULL;

		RtlInitUnicodeString(&moduleNameUnicode, ModuleName);

		PPEB peb = PsGetProcessPeb(Process);
		if (!peb) {
			status = STATUS_UNSUCCESSFUL;
			return status;
		}

		PPEB_LDR_DATA ldr = peb->Ldr;
		if (!ldr) {
			status = STATUS_UNSUCCESSFUL;
			return status;
		}

		// 锁定 PEB_LDR_DATA 数据结构
		KAPC_STATE apcState;
		KeStackAttachProcess(Process, &apcState);

		PLIST_ENTRY moduleList = &ldr->InMemoryOrderModuleList;
		PLIST_ENTRY currEntry = moduleList->Flink;

		while (currEntry != moduleList) {
			// 获取 LDR_DATA_TABLE_ENTRY 结构体
			PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(currEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			// 比较模块名
			if (RtlCompareUnicodeString(&entry->BaseDllName, &moduleNameUnicode, TRUE) == 0) {
				// 找到了指定模块
				moduleBase = entry->DllBase;
				break;
			}

			currEntry = currEntry->Flink;
		}

		// 解锁
		KeUnstackDetachProcess(&apcState);

		if (moduleBase) {
			*Module_Base = (ULONG64)moduleBase;
		}
		else {
			status = STATUS_NOT_FOUND;
		}

		return status;
	}

	NTSTATUS create(PDEVICE_OBJECT device_object, PIRP irp) {
		UNREFERENCED_PARAMETER(device_object);

		IoCompleteRequest(irp, IO_NO_INCREMENT);

		return irp->IoStatus.Status; 
	}

	NTSTATUS close(PDEVICE_OBJECT device_object, PIRP irp) {
		UNREFERENCED_PARAMETER(device_object);

		IoCompleteRequest(irp, IO_NO_INCREMENT);

		return irp->IoStatus.Status;
	}

	 NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp) {
		UNREFERENCED_PARAMETER(device_object);

		debug_print("[+] Device control called.");

		NTSTATUS status = STATUS_UNSUCCESSFUL;

		// We need this to determine which code was passed through.
		PIO_STACK_LOCATION stack_irp = IoGetCurrentIrpStackLocation(irp);

		// Access the request object sent from user mode.
		auto request = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer);

		if (stack_irp == nullptr || request == nullptr) {
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return status;
		}


		// The target process we want access to.
		static PEPROCESS target_process = nullptr;

		 
		const ULONG control_code = stack_irp->Parameters.DeviceIoControl.IoControlCode;

		switch (control_code) {
		case codes::attach:
			status = PsLookupProcessByProcessId(request->process_id, &target_process);
			break;

		case codes::attach2:
			status = AttachProcess(&target_process);
			break;

		case codes::read:
			if (target_process != nullptr)
				status = MmCopyVirtualMemory(target_process, request->target, PsGetCurrentProcess(), request->buffer, request->size, KernelMode, &request->return_size);
			break;

		case codes::write:
			if (target_process != nullptr)
				status = MmCopyVirtualMemory(PsGetCurrentProcess(), request->buffer, target_process, request->target, request->size, KernelMode, &request->return_size);
			break;

		case codes::get_moudle:
			if (target_process != nullptr)
				status = GetModuleBaseProcess(target_process, reinterpret_cast<LPCWSTR>(request->target), reinterpret_cast<ULONG64*>(request->buffer));
			break;

		default:
			break;
		}

		irp->IoStatus.Status = status;
		irp->IoStatus.Information = sizeof(Request);

		IoCompleteRequest(irp, IO_NO_INCREMENT);

		return status;
	}

}  // namespace driver

NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
	UNREFERENCED_PARAMETER(registry_path);

	UNICODE_STRING device_name = {};
	RtlInitUnicodeString(&device_name, L"\\Device\\BabyDriver");

	// Create driver device object.
	PDEVICE_OBJECT device_object = nullptr;
	NTSTATUS status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);

	if (status != STATUS_SUCCESS) {
		debug_print("[-] Failed to create driver device.\n");
		return status;
	}

	debug_print("[+] Driver device successfully created.\n");

	UNICODE_STRING symbolic_link = {};
	RtlInitUnicodeString(&symbolic_link, L"\\DosDevices\\BabyDriver");

	status = IoCreateSymbolicLink(&symbolic_link, &device_name);
	if (status != STATUS_SUCCESS) {
		debug_print("[-] Failed to establish symbolic link.\n");
		return status;
	}

	debug_print("[+] Driver symbolic link successfully established.\n");

	// Allow us to send small amounts of data between um/km.
	SetFlag(device_object->Flags, DO_BUFFERED_IO);

	// Set the driver handlers to our functions with our logic.
	driver_object->MajorFunction[IRP_MJ_CREATE] = driver::create;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = driver::close;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::device_control;

	// We have initialized our device.
	ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

	debug_print("[+] Driver initialized successfully.\n");

	return status;
}

NTSTATUS DriverEntry() {
	debug_print("[+] HERE IS SOMEONE WHO IS WILL BLOW UP YOUR KERNEL.\n");

	UNICODE_STRING process_by_name = {};
	RtlInitUnicodeString(&process_by_name, L"PsLookupProcessByProcessName");
	PsLookupProcessByProcessName = (PsLookupProcessByProcessNamePtr)MmGetSystemRoutineAddress(&process_by_name);

	UNICODE_STRING get_process_peb = {};
	RtlInitUnicodeString(&get_process_peb, L"PsGetProcessPeb");
	PsGetProcessPeb = (PsGetProcessPebPtr)MmGetSystemRoutineAddress(&get_process_peb);

	UNICODE_STRING driver_name = {};
	RtlInitUnicodeString(&driver_name, L"\\Driver\\BabyDriver");


	return IoCreateDriver(&driver_name, driver_main);
}
#include <ntifs.h>
#include <wdm.h>
#include <stdio.h>

typedef unsigned char BYTE;

extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);

	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress,
											 SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);

	NTKERNELAPI NTSTATUS PsLookupProcessByProcessName(PUNICODE_STRING ProcessName, PEPROCESS* Process);

	NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS Process);
	// NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(IN PEPROCESS Process);
	// NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process(IN PEPROCESS Process);

	//typedef NTSTATUS(*PsLookupProcessByProcessNamePtr)(IN PUNICODE_STRING ProcessName, OUT PEPROCESS* Process);
	//typedef PPEB(*PsGetProcessPebPtr)(IN PEPROCESS Process);

	// typedef void(__stdcall* PPS_POST_PROCESS_INIT_ROUTINE)(void);
}




//0x18 bytes (sizeof)
struct _CURDIR
{
	struct _UNICODE_STRING DosPath;                                         //0x0
	VOID* Handle;                                                           //0x10
};

//0x18 bytes (sizeof)
struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;                                                           //0x0
	USHORT Length;                                                          //0x2
	ULONG TimeStamp;                                                        //0x4
	struct _STRING DosPath;                                                 //0x8
};

//0x58 bytes (sizeof)
typedef struct _PEB_LDR_DATA
{
	ULONG Length;                                                           //0x0
	UCHAR Initialized;                                                      //0x4
	VOID* SsHandle;                                                         //0x8
	struct _LIST_ENTRY InLoadOrderModuleList;                               //0x10
	struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x20
	struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x30
	VOID* EntryInProgress;                                                  //0x40
	UCHAR ShutdownInProgress;                                               //0x48
	VOID* ShutdownThreadId;                                                 //0x50
} PEB_LDR_DATA, * PPEB_LDR_DATA;


//0x440 bytes (sizeof)
typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;                                                    //0x0
	ULONG Length;                                                           //0x4
	ULONG Flags;                                                            //0x8
	ULONG DebugFlags;                                                       //0xc
	VOID* ConsoleHandle;                                                    //0x10
	ULONG ConsoleFlags;                                                     //0x18
	VOID* StandardInput;                                                    //0x20
	VOID* StandardOutput;                                                   //0x28
	VOID* StandardError;                                                    //0x30
	struct _CURDIR CurrentDirectory;                                        //0x38
	struct _UNICODE_STRING DllPath;                                         //0x50
	struct _UNICODE_STRING ImagePathName;                                   //0x60
	struct _UNICODE_STRING CommandLine;                                     //0x70
	VOID* Environment;                                                      //0x80
	ULONG StartingX;                                                        //0x88
	ULONG StartingY;                                                        //0x8c
	ULONG CountX;                                                           //0x90
	ULONG CountY;                                                           //0x94
	ULONG CountCharsX;                                                      //0x98
	ULONG CountCharsY;                                                      //0x9c
	ULONG FillAttribute;                                                    //0xa0
	ULONG WindowFlags;                                                      //0xa4
	ULONG ShowWindowFlags;                                                  //0xa8
	struct _UNICODE_STRING WindowTitle;                                     //0xb0
	struct _UNICODE_STRING DesktopInfo;                                     //0xc0
	struct _UNICODE_STRING ShellInfo;                                       //0xd0
	struct _UNICODE_STRING RuntimeData;                                     //0xe0
	struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];                  //0xf0
	ULONGLONG EnvironmentSize;                                              //0x3f0
	ULONGLONG EnvironmentVersion;                                           //0x3f8
	VOID* PackageDependencyData;                                            //0x400
	ULONG ProcessGroupId;                                                   //0x408
	ULONG LoaderThreads;                                                    //0x40c
	struct _UNICODE_STRING RedirectionDllName;                              //0x410
	struct _UNICODE_STRING HeapPartitionName;                               //0x420
	ULONGLONG* DefaultThreadpoolCpuSetMasks;                                //0x430
	ULONG DefaultThreadpoolCpuSetMaskCount;                                 //0x438
	ULONG DefaultThreadpoolThreadMaximum;                                   //0x43c
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;


typedef struct _PEB
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PVOID ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PVOID FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	PVOID CrossProcessFlags;
	PVOID KernelCallbackTable;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	PVOID ApiSetMap;
} PEB, * PPEB;


typedef struct _LDR_DATA_TABLE_ENTRY
{
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
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;


// PsLookupProcessByProcessNamePtr PsLookupProcessByProcessName = NULL;
// PsGetProcessPebPtr PsGetProcessPeb = NULL;






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

	//NTSTATUS AttachProcess(PEPROCESS* target_process) {
	//	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	//	PEPROCESS Process = NULL;
	//	HANDLE process_id = 0;

	//	debug_print("\n1");

	//	if (PsLookupProcessByProcessName)
	//	{
	//		debug_print("\n2");
	//		UNICODE_STRING game_process_name = {};
	//		RtlInitUnicodeString(&game_process_name, L"notepad.exe");
	//		Status = PsLookupProcessByProcessName(&game_process_name, &Process);
	//		debug_print("\n3");
	//		if (NT_SUCCESS(Status))
	//		{
	//			process_id = PsGetProcessId(Process);
	//			debug_print("\n4");
	//			ObDereferenceObject(Process);
	//		}
	//	}

	//	if (!NT_SUCCESS(Status))
	//	{
	//		return Status;
	//	}

	//	char buffer[20];
	//	sprintf_s(buffer, sizeof(buffer), "%p", process_id);
	//	debug_print(buffer);

	//	return PsLookupProcessByProcessId(process_id, target_process);
	//}


	ULONG64 GetModuleBasex64(PEPROCESS proc, UNICODE_STRING module_name, bool get_size) {
		PPEB pPeb = (PPEB)PsGetProcessPeb(proc);

		if (!pPeb) {
			return 0;
		}

		KAPC_STATE state;

		KeStackAttachProcess(proc, &state);

		PPEB_LDR_DATA pLdr = (PPEB_LDR_DATA)pPeb->Ldr;

		if (!pLdr) {
			KeUnstackDetachProcess(&state);
			return 0;
		}

		UNICODE_STRING name;

		// loop the linked list
		for (PLIST_ENTRY list = (PLIST_ENTRY)pLdr->InLoadOrderModuleList.Flink;
			list != &pLdr->InLoadOrderModuleList; list = (PLIST_ENTRY)list->Flink)
		{
			PLDR_DATA_TABLE_ENTRY pEntry =
				CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			if (RtlCompareUnicodeString(&pEntry->BaseDllName, &module_name, TRUE) ==
				0) {
				ULONG64 baseAddr = (ULONG64)pEntry->DllBase;
				ULONG64 moduleSize = (ULONG64)pEntry->SizeOfImage; // get the size of the module
				KeUnstackDetachProcess(&state);
				if (get_size) {
					return moduleSize; // return the size of the module if get_size is TRUE
				}
				return baseAddr;
			}
		}

		KeUnstackDetachProcess(&state);

		return 0; // failed
	}


	NTSTATUS GetModuleBaseProcess(PEPROCESS proc, ULONG64* buffer) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		UNICODE_STRING module_name = {};

		RtlInitUnicodeString(&module_name, L"client.dll");

		ULONG64 addr = GetModuleBasex64(proc, module_name, false);

		if (addr == 0) {
			return status;
		}

		*buffer = addr;

		status = STATUS_SUCCESS;

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
			//debug_print("22222");
			//status = AttachProcess(&target_process);
			status = STATUS_SUCCESS;
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
				status = GetModuleBaseProcess(target_process, reinterpret_cast<ULONG64*>(request->buffer));
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

	//UNICODE_STRING process_by_name = {};
	//RtlInitUnicodeString(&process_by_name, L"PsLookupProcessByProcessName");
	//PsLookupProcessByProcessName = (PsLookupProcessByProcessNamePtr)MmGetSystemRoutineAddress(&process_by_name);

	//UNICODE_STRING get_process_peb = {};
	//RtlInitUnicodeString(&get_process_peb, L"PsGetProcessPeb");
	//PsGetProcessPeb = (PsGetProcessPebPtr)MmGetSystemRoutineAddress(&get_process_peb);

	UNICODE_STRING driver_name = {};
	RtlInitUnicodeString(&driver_name, L"\\Driver\\BabyDriver");


	return IoCreateDriver(&driver_name, driver_main);
}
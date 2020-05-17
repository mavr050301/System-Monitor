#include <stdio.h>
#include <Windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <winnt.h>
#include <tchar.h>
#include<iostream>
#include<iomanip>
#include "Exception.h"
#pragma comment(lib,"ntdll.lib") 
#define DIV 1024
#define WIDTH 7
#define NT_SUCCESS(x) ((x) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004

#define SystemHandleInformation 16
#define ObjectBasicInformation 0
#define ObjectNameInformation 1
#define ObjectTypeInformation 2
////#ifndef UNICODE
////#define UNICODE
//#endif
  typedef NTSTATUS(NTAPI *_NtQuerySystemInformation)(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);
typedef NTSTATUS(NTAPI *_NtDuplicateObject)(
	HANDLE SourceProcessHandle,
	HANDLE SourceHandle,
	HANDLE TargetProcessHandle,
	PHANDLE TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG Attributes,
	ULONG Options
	);
  typedef NTSTATUS(NTAPI *_NtQueryObject)(
	HANDLE ObjectHandle,
	ULONG ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength
	);


typedef struct _SYSTEM_HANDLE {
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG HandleCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef enum _POOL_TYPE {
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS
} POOL_TYPE, *PPOOL_TYPE;

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING Name;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccess;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;
	USHORT MaintainTypeList;
	POOL_TYPE PoolType;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

PVOID GetLibraryProcAddress(PSTR LibraryName, PSTR ProcName) {
	return GetProcAddress(GetModuleHandleA(LibraryName), ProcName);
}
void getProcesses();
int getDLLofProcess(DWORD PID);
void PrintMemoryInfo();
void getCpuUsage();
int getFilesofProcess(ULONG pid);
ULONGLONG ft2ull(FILETIME &ft);
 int numberofProcesses = 0;
using namespace std;
int main()
{
	system("color F0");
	int selection = 0;
	BadInput a(0);
	do {
		printf("\n1)Get List of Processes");
		printf("\n2)Get Files of Process by PID");
		printf("\n3)Get DLLs  of Process by PID");
		printf("\n4)Get Memory Info");
		printf("\n5)Get Cpu usage");
		printf("\n6)Exit");
		printf("\nSelect:");
		selection= a.input(0, 6);
			switch (selection) 
			{
			case 1:
			{
				system("cls");
				getProcesses();
				break;
			}
			case 2: 
			{
				int flagofproc=0,proc=0;
				printf("\nInput pid:");
				proc = a.input(0, 50000);
				system("cls");
				flagofproc = getFilesofProcess(proc);
				if (flagofproc) { printf("Can't get the Information\n"); }
				break;
			}
			case 3:
			{
				int flagofproc = 0, proc = 0;
				printf("\nInput pid:");
				proc = a.input(0, 50000);
				system("cls");
				flagofproc = getDLLofProcess(proc);
				if (flagofproc) { printf("No DLLs"); }
				break;
			}
			case 4:
			{
				system("cls");
				PrintMemoryInfo();
				break;
			}
			case 5:
			{
				system("cls");
				printf("CPU Usage:");
				getCpuUsage();
				break;
			}

			}
		} while (selection != 6);
	return 0;
}
void getProcesses()
{
	numberofProcesses = 0;
	HANDLE pSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bool flag = false;
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);
	
	flag = Process32First(pSnap, &ProcEntry);
	cout << setiosflags(ios::left) << setw(10) << "PID"<< setw(70) << "PROCESSNAME"<<endl;
	while (flag)
	{
		cout << setiosflags(ios::left) << setw(10) << ProcEntry.th32ProcessID << setw(70) << ProcEntry.szExeFile;
		flag = Process32Next(pSnap, &ProcEntry);
		printf("\n");
		numberofProcesses++;
		printf("\n\n");
	}
	printf("NUMBER OF PROCESSES: %d\n", numberofProcesses);
	CloseHandle(pSnap);
}
int getDLLofProcess(DWORD PID)
{
	HANDLE pMdlSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	bool flag = false,flagofdll=1;
	MODULEENTRY32 MdlEntry;
	MdlEntry.dwSize = sizeof(MODULEENTRY32);
	flag = Module32First(pMdlSnap, &MdlEntry);
	while (flag)
	{	
		flagofdll = 0;
		printf("%s\n", MdlEntry.szModule);
		flag = Module32Next(pMdlSnap, &MdlEntry);
	}
	CloseHandle(pMdlSnap);
	return flagofdll;
}
void PrintMemoryInfo()
{
	MEMORYSTATUSEX memoryInfo;

	memoryInfo.dwLength = sizeof(memoryInfo);

	GlobalMemoryStatusEx(&memoryInfo);

	_tprintf(TEXT("There is  %*ld percent of memory in use.\n"),
		WIDTH, memoryInfo.dwMemoryLoad);
	_tprintf(TEXT("There are %*I64d total KB of physical memory.\n"),
		WIDTH, memoryInfo.ullTotalPhys / DIV);
	_tprintf(TEXT("There are %*I64d free  KB of physical memory.\n"),
		WIDTH, memoryInfo.ullAvailPhys / DIV);
	_tprintf(TEXT("There are %*I64d total KB of paging file.\n"),
		WIDTH, memoryInfo.ullTotalPageFile / DIV);
	_tprintf(TEXT("There are %*I64d free  KB of paging file.\n"),
		WIDTH, memoryInfo.ullAvailPageFile / DIV);
	_tprintf(TEXT("There are %*I64d total KB of virtual memory.\n"),
		WIDTH, memoryInfo.ullTotalVirtual / DIV);
	_tprintf(TEXT("There are %*I64d free  KB of virtual memory.\n"),
		WIDTH, memoryInfo.ullAvailVirtual / DIV);
}
void getCpuUsage()
{

	FILETIME idle, prev_idle;
	FILETIME kernel, prev_kernel;
	FILETIME user, prev_user;
	GetSystemTimes(&prev_idle, &prev_kernel, &prev_user);
	Sleep(1000);
	GetSystemTimes(&idle, &kernel, &user);
	ULONGLONG sys = (ft2ull(user) - ft2ull(prev_user)) +
		(ft2ull(kernel) - ft2ull(prev_kernel));
	std::cout << int((sys - ft2ull(idle) + ft2ull(prev_idle)) * 100.0 / sys) << "%" << std::endl;
}
ULONGLONG ft2ull(FILETIME &ft) {
	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;
	return ul.QuadPart;
}
int getFilesofProcess(ULONG pid) {
	_NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetLibraryProcAddress("ntdll.dll", "NtQuerySystemInformation");
	_NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetLibraryProcAddress("ntdll.dll", "NtDuplicateObject");
	_NtQueryObject NtQueryObject = (_NtQueryObject)GetLibraryProcAddress("ntdll.dll", "NtQueryObject");

	NTSTATUS status;
	PSYSTEM_HANDLE_INFORMATION handleInfo;
	ULONG handleInfoSize = 0x10000;
	HANDLE processHandle;
	ULONG i;

	if (!(processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid))) {
		printf("Could not open PID %d! (Don't try to open a system process.)\n", pid);
		return 1;
	}

	handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);

	// NtQuerySystemInformation won't give us the correct buffer size,
	//  so we guess by doubling the buffer size.
	while ((status = NtQuerySystemInformation(
		SystemHandleInformation,
		handleInfo,
		handleInfoSize,
		NULL
	)) == STATUS_INFO_LENGTH_MISMATCH)
		handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize *= 2);

	// NtQuerySystemInformation stopped giving us STATUS_INFO_LENGTH_MISMATCH.
	if (!NT_SUCCESS(status)) {
		printf("NtQuerySystemInformation failed!\n");
		return 1;
	}

	for (i = 0; i < handleInfo->HandleCount; i++) {
		SYSTEM_HANDLE handle = handleInfo->Handles[i];
		HANDLE dupHandle = NULL;
		POBJECT_TYPE_INFORMATION objectTypeInfo;
		PVOID objectNameInfo;
		UNICODE_STRING objectName;
		ULONG returnLength;
		// Check if this handle belongs to the PID the user specified.
		if (handle.ProcessId != pid)
			continue;

		// Duplicate the handle so we can query it.
		if (!NT_SUCCESS(NtDuplicateObject(
			processHandle,
			(void*)handle.Handle,
			GetCurrentProcess(),
			&dupHandle,
			0,
			0,
			0
		))) {
			continue;
		}

		// Query the object type.
		objectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(0x1000);
		if (!NT_SUCCESS(NtQueryObject(
			dupHandle,
			ObjectTypeInformation,
			objectTypeInfo,
			0x1000,
			NULL
		))) {
			CloseHandle(dupHandle);
			continue;
		}

		// Query the object name (unless it has an access of
		//   0x0012019f, on which NtQueryObject could hang.
		if (handle.GrantedAccess == 0x0012019f) {

			// We have the type, so display that.
			free(objectTypeInfo);
			CloseHandle(dupHandle);
			continue;
		}

		objectNameInfo = malloc(0x1000);
		if (!NT_SUCCESS(NtQueryObject(
			dupHandle,
			ObjectNameInformation,
			objectNameInfo,
			0x1000,
			&returnLength
		))) {

			// Reallocate the buffer and try again.
			objectNameInfo = realloc(objectNameInfo, returnLength);
			if (!NT_SUCCESS(NtQueryObject(
				dupHandle,
				ObjectNameInformation,
				objectNameInfo,
				returnLength,
				NULL
			))) {

				// We have the type name, so just display that.
				free(objectTypeInfo);
				free(objectNameInfo);
				CloseHandle(dupHandle);
				continue;
			}
		}

		// Cast our buffer into an UNICODE_STRING.
		objectName = *(PUNICODE_STRING)objectNameInfo;

		// Print the information!
		if (objectName.Length)
		{
			// The object has a name.
			printf("[%#x] %.*S: %.*S\n", handle.Handle, objectTypeInfo->Name.Length / 2, objectTypeInfo->Name.Buffer, objectName.Length / 2, objectName.Buffer);
		}
		free(objectTypeInfo);
		free(objectNameInfo);
		CloseHandle(dupHandle);
	}

	free(handleInfo);
	CloseHandle(processHandle);
	return 0;
}
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
void getProcesses();
void getFilesofProcess(DWORD PID);
void PrintMemoryInfo();
void getCpuUsage();
ULONGLONG ft2ull(FILETIME &ft);
 int printdll = 0;
 int numberofProcesses = 0;
using namespace std;
int main()
{
	int selection = 0;
	BadInput a(0);
	do {
		printf("\n1)Get Processes");
		printf("\n2)Get Processes Files");
		printf("\n3)Get Memory Info");
		printf("\n4)Get Cpu usage");
		printf("\n5)Exit");
		printf("\nSelect:");
		selection= a.input(0, 5);
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
				system("cls");
				printdll = 1;
				getProcesses();
				printdll = 0;
				break;
			}
			case 3:
			{
				system("cls");
				PrintMemoryInfo();
				break;
			}
			case 4:
			{
				system("cls");
				printf("CPU Usage:");
				getCpuUsage();
				break;
			}

			}
		} while (selection != 5);
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
		if(printdll) getFilesofProcess(ProcEntry.th32ProcessID);
		numberofProcesses++;
		printf("\n\n");
	}
	CloseHandle(pSnap);
}
void getFilesofProcess(DWORD PID)
{
	HANDLE pMdlSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	bool flag = false;
	MODULEENTRY32 MdlEntry;
	MdlEntry.dwSize = sizeof(MODULEENTRY32);
	flag = Module32First(pMdlSnap, &MdlEntry);
	while (flag)
	{	
		printf("%s\n", MdlEntry.szModule);
		flag = Module32Next(pMdlSnap, &MdlEntry);
	}
	CloseHandle(pMdlSnap);
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
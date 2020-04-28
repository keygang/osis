// L2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <stdlib.h>
#include <locale.h>  
#include <stdio.h>
#include <windows.h>
#include <tchar.h>


void ErrorMes(LPTSTR lpszFunction)
{

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);


	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	wprintf(L"%s failed with error %d: %s",
		lpszFunction, dw, lpMsgBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

}

BOOL FindFileByClaster(TCHAR* volume, LONGLONG cluster) {

	HANDLE hDevice = CreateFile(volume,      // какой диск открываем
		GENERIC_READ,                        // доступ к диску
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // режим обмена
		NULL,                                // атрибуты безопасности по умолчанию
		OPEN_EXISTING,                       // расположение
		FILE_FLAG_BACKUP_SEMANTICS,          // атрибуты файла
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		ErrorMes((LPTSTR)L"CreateFile");
		return FALSE;
	}

	LOOKUP_STREAM_FROM_CLUSTER_INPUT input = { 0 };
	input.NumberOfClusters = 1;
	input.Cluster[0].QuadPart = cluster;

	BYTE output[5000] = {};
	DWORD dwRes = 0;
	LOOKUP_STREAM_FROM_CLUSTER_OUTPUT result = { 0 };

	BOOL bRes = DeviceIoControl((HANDLE)hDevice,	// дескриптор файла, каталога или тома 
		FSCTL_LOOKUP_STREAM_FROM_CLUSTER,			// dwIoControlCode
		(LPVOID)&input,								// входной буфер 
		(DWORD)       sizeof(input),				// размер входного буфера 
		(LPVOID)output,								// выходной буфер
		(DWORD)5000,								// размер выходного буфера 
		(LPDWORD)&dwRes,							// количество возвращенных байтов 
		NULL);										// Перекрытая структура

	if (bRes == FALSE) {
		ErrorMes((LPTSTR)L"DeviceIoControl");
		return FALSE;
	}

	memcpy(&result, output, sizeof(LOOKUP_STREAM_FROM_CLUSTER_OUTPUT));

	if (result.NumberOfMatches == 0) {
		wprintf(L"File not found\n");
		return FALSE;
	}

	wprintf(L"File information:\n");

	BYTE* p = (BYTE*)output + result.Offset;
	LOOKUP_STREAM_FROM_CLUSTER_ENTRY* pentry = (LOOKUP_STREAM_FROM_CLUSTER_ENTRY*)p;

	wprintf(L"Flags: 0x%x ", (UINT)pentry->Flags);

	if ((pentry->Flags & LOOKUP_STREAM_FROM_CLUSTER_ENTRY_FLAG_PAGE_FILE) > 0) wprintf(L"(Pagefile)");
	else if ((pentry->Flags & LOOKUP_STREAM_FROM_CLUSTER_ENTRY_FLAG_FS_SYSTEM_FILE) > 0)  wprintf(L"(Internal filesystem file)");
	else if ((pentry->Flags & LOOKUP_STREAM_FROM_CLUSTER_ENTRY_FLAG_TXF_SYSTEM_FILE) > 0) wprintf(L"(Internal TXF file)");
	else wprintf(L"(Normal file)");

	wprintf(L"\nFile: %s\n", pentry->FileName);
	return TRUE;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	LONGLONG inp = 0;
	wprintf(L"Введите номер кластера: \n");
	scanf_s("%llu", &inp, 30);

	FindFileByClaster((TCHAR*)L"\\\\.\\C:", inp);

	system("PAUSE");
	return 0;
}
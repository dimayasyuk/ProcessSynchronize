// OSProcCons.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <tchar.h>
#include <fstream>

using namespace std;


#define BUF_SIZE 10
#define MAPPING_FILE_NAME "File"
#define EVENT_CONSOLE_NAME "eventConsole"
#define EVENT_MAIN_NAME "eventMain"
#define WAIT_TIME 5000
HANDLE hMapFile;
HANDLE mainEvent;
HANDLE consoleEvent;
LPVOID pBuf;
HANDLE hFile;
DWORD dwWaitResult;

void closeSource(){
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(mainEvent);
	CloseHandle(consoleEvent);
}
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");
	hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT(MAPPING_FILE_NAME));


	if (hMapFile == NULL)
	{
		printf("�� ������� ������� ������������ � ������ ����. ��� ������: %lu\n", GetLastError());
		return GetLastError();
	}

	pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);

	if (pBuf == NULL)
	{
		printf("�� ������� ������� ������������� ����� ����. ��� ������: %lu\n", GetLastError());
		return GetLastError();
	}

	mainEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT(EVENT_MAIN_NAME));
	consoleEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT(EVENT_CONSOLE_NAME));
	if (mainEvent == NULL || consoleEvent == NULL){
		printf("�������� ������� ���������� (%d).\n",
			GetLastError());
		return GetLastError();

	}

	while (true)
	{
		switch (WaitForSingleObject(mainEvent, WAIT_TIME))
		{
		case WAIT_OBJECT_0:
			
			printf("%i\n", *((LPSTR)pBuf));
			ResetEvent(mainEvent);
			SetEvent(consoleEvent);
			break;
		case WAIT_FAILED:
			case WAIT_TIMEOUT:
				closeSource();
				return GetLastError();
			}
	}
	_getch();
	return 0;
}


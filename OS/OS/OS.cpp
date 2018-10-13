// OS.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <sstream>

using namespace std;

#define BUF_SIZE 10
#define MAPPING_FILE_NAME "File"
#define CONSOLE_PROCESS_NAME "E:\\учеба\\3курс\\ос\\OS\\Debug\\OSProcCons.exe"
#define FILE_PROCESS_NAME "E:\\учеба\\3курс\\ос\\OSProcFile\\Debug\\OSProcFile.exe"
#define EVENT_FILE_NAME "eventFile"
#define EVENT_CONSOLE_NAME "eventConsole"
#define EVENT_MAIN_NAME "eventMain"
#define SLEEP_TIME 1000
#define WAIT_TIME 5000
HANDLE hMapFile;
HANDLE mainEvent;
HANDLE consoleEvent;
HANDLE fileEvent;
LPVOID pBuf;
HANDLE events[2];
FILE *myfile;
#define FILE_NAME "OS.txt"

void closeSource(){
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(mainEvent);
	CloseHandle(consoleEvent);
	CloseHandle(fileEvent);
}
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), TEXT(MAPPING_FILE_NAME));
	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
	{
		printf("Не может создать  отраженный в памяти объект (%d).\n",
			GetLastError());
		return GetLastError();
	}

	pBuf = MapViewOfFile(hMapFile, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);

	if (pBuf == NULL)
	{
		printf("Представление проецированного файла невозможно (%d).\n",
			GetLastError());
		return GetLastError();
	}

	mainEvent = CreateEvent(NULL, TRUE, FALSE, TEXT(EVENT_MAIN_NAME));
	consoleEvent = CreateEvent(NULL, TRUE, FALSE, TEXT(EVENT_CONSOLE_NAME));
	fileEvent = CreateEvent(NULL, TRUE, FALSE, TEXT(EVENT_FILE_NAME));
	if (mainEvent == NULL || consoleEvent == NULL || fileEvent == NULL){
		printf("Создание события невозможно (%d).\n",
			GetLastError());
		return GetLastError();
	}
	events[0] = consoleEvent;
	events[1] = fileEvent;

	STARTUPINFO si = { sizeof(si) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	PROCESS_INFORMATION piProcessB, piProcessC;
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	if (CreateProcess(TEXT(FILE_PROCESS_NAME), NULL, &saProcess, &saThread, FALSE, 0, NULL, NULL, &si, &piProcessC) == 0){
		printf("Создание процесса невозможно (%d).\n",
		GetLastError());
		closeSource();
		return GetLastError();
		}

	if (CreateProcess(TEXT(CONSOLE_PROCESS_NAME), NULL, &saProcess, &saThread, FALSE, 0, NULL, NULL, &si, &piProcessB) == 0){
		printf("Создание процесса невозможно (%d).\n",
			GetLastError());
		closeSource();
		return GetLastError();
	}
	int number;
	while(true){
	
		number = rand() % 50 + 1;
		CopyMemory(pBuf, &number, sizeof(int));
		SetEvent(mainEvent);
		if (!WaitForMultipleObjects(2, events, TRUE, WAIT_TIME)){
			ResetEvent(consoleEvent);
			ResetEvent(fileEvent);
		}
		else{
			return GetLastError();
		}
	}
	closeSource();
	CloseHandle(piProcessB.hProcess);
	CloseHandle(piProcessB.hThread);
	CloseHandle(piProcessC.hProcess);
	CloseHandle(piProcessC.hThread);
	_getch();
	return 0;
}

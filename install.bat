@echo off
echo [INFO] Creating new config file (THIS WILL CLEAR OLD DATA)...
rmdir /S "C:\Program Files\PTinside"
mkdir "C:\Program Files\PTinside"
mkdir "C:\Program Files\PTinside\reports"
echo FORCE>> "C:\Program Files\PTinside\version.txt"
echo HERE ENTER YOUR FTP ADDRESS>> "C:\Program Files\PTinside\config.txt"
echo HERE ENTER YOUR FTP USERNAME>> "C:\Program Files\PTinside\config.txt"
echo HERE ENTER YOUR FTP PASSWORD>> "C:\Program Files\PTinside\config.txt"
if %errorlevel%==1 (
echo [ERROR] Cannot create config file
color C
pause
exit
)
echo [INFO] Setting permissions
icacls "C:\Program Files\PTinside" /grant *WD:(F) /t
echo [INFO] Downloading PTupdater
powershell Start-BitsTransfer 'https://github.com/PetrusTryb/PTinside/raw/master/PTupdater.exe' 'C:/Program Files/PTinside/PTupdater.exe'
echo [INFO] Launching PTupdater to download PTinside
"C:/Program Files/PTinside/PTupdater.exe"
echo [INFO] Adding PTinside and PTupdater to autostart...
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v PTinside /t REG_SZ /d "C:/Program Files/PTinside/PTinside.exe"
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v PTinside /t REG_SZ /d "C:/Program Files/PTinside/PTupdater.exe"
echo [INFO] Disabling Windows Defender...
reg add "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender" /v DisableAntiSpyware /t DWORD /d 1
reg add "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableRealtimeMonitoring /t DWORD /d 1
echo [SUCCESS] Installation complete!
echo [SUCCESS] Please check if PTupdater and PTinside are present in Task Manager
echo [SUCCESS] After that please restart the computer and check if autostart works
color A
pause
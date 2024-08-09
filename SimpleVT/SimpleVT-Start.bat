@echo off
set FILE_NAME=SimpleVT.sys
set SOURCE=\\192.168.29.194\BaiduNetdiskDownload\hack\code\SimpleVT\x64\Debug\%FILE_NAME%
set DESTINATION=%USERPROFILE%\Desktop
set SERVICE_NAME=SimpleVT

xcopy    %SOURCE%    %DESTINATION%\    /Y

sc create    %SERVICE_NAME%     binPath= %USERPROFILE%\Desktop\%FILE_NAME% type= kernel
sc start      %SERVICE_NAME% 
sc stop      %SERVICE_NAME% 
sc delete    %SERVICE_NAME% 

pause
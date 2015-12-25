@echo off
set WHITE=E:\hara-buyaodong\hara.exe
set BLACK=E:\badukGo_0.1.1\BadukGO.exe
set TWOGTP=""D:\Program Files (x86)\GoGui\gogui-twogtp.exe"" -black ""%BLACK%"" -white ""%WHITE%"" -size 13
"D:\Program Files (x86)\GoGui\GoGui.exe" -program "%TWOGTP%" -size 13 -computer-both -verbose
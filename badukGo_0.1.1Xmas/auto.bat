@echo off
set BLACK=E:\goxt\goAI.exe
set WHITE=E:\badukGo_0.1.1\BadukGO.exe
set TWOGTP=""D:\Program Files (x86)\GoGui\gogui-twogtp.exe"" -black ""%BLACK%"" -white ""%WHITE%"" -size 13
"D:\Program Files (x86)\GoGui\GoGui.exe" -program "%TWOGTP%" -size 13 -computer-both -verbose
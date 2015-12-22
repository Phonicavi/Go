@echo off
set WHITE=E:\goxt\goAI.exe
set BLACK=E:\hara-battle\BauukGO.exe
set TWOGTP=""D:\Program Files (x86)\GoGui\gogui-twogtp.exe"" -black ""%BLACK%"" -white ""%WHITE%"" -size 13
"D:\Program Files (x86)\GoGui\GoGui.exe" -program "%TWOGTP%" -size 13 -computer-both -verbose
@echo off
set BLACK=F:\Course\2015-2016_Autumn\Artificial_Intelligence\Go\other-function\pachi-10.00-win32\pachi.exe
set WHITE=F:\Course\2015-2016_Autumn\Artificial_Intelligence\Go\brown-1.0_cpp\brown.exe
set TWOGTP=""D:\Program Files (x86)\GoGui\gogui-twogtp.exe"" -black ""%BLACK%"" -white ""%WHITE%"" -size 7
"D:\Program Files (x86)\GoGui\GoGui.exe" -program "%TWOGTP%" -size 7 -computer-both -verbose
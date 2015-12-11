@echo off
set BLACK=F:\Course\2015-2016_Autumn\Artificial_Intelligence\Go\resource\Hara-master\hara.exe
set WHITE=F:\Course\2015-2016_Autumn\Artificial_Intelligence\Go\resource\gnuaplus-revised\aplus_final_code\aplus.exe
set TWOGTP=""D:\Program Files (x86)\GoGui\gogui-twogtp.exe"" -black ""%BLACK%"" -white ""%WHITE%"" -size 13
"D:\Program Files (x86)\GoGui\GoGui.exe" -program "%TWOGTP%" -size 13 -computer-both -verbose
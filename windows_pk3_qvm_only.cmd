SET PAKNAME=upak1_test
cd /D "%~dp0windows_scripts"
echo.tmp>>"%~dp0windows_scripts\q3silent.txt"
cmd.exe "/c windows_compile_cgame.bat"
cmd.exe "/c windows_compile_game.bat"
cmd.exe "/c windows_compile_ui.bat"
del "%~dp0windows_scripts\q3silent.txt"
del "%~dp0%PAKNAME%.zip"
del "%~dp0%PAKNAME%.pk3"
del "%~dp0DATA_DEPLOY_THIS\%PAKNAME%.pk3"
cd "%~dp0windows\base\vm"
powershell Compress-Archive '.' '%~dp0%PAKNAME%.zip'
powershell Compress-Archive '%~dp0data_to_include_%PAKNAME%\*' -Update '%~dp0%PAKNAME%.zip'
md "%~dp0DATA_DEPLOY_THIS"
move "%~dp0%PAKNAME%.zip" "%~dp0DATA_DEPLOY_THIS\%PAKNAME%.pk3"
del "%~dp0windows\base\vm\*.qvm"
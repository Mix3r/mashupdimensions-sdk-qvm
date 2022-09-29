cd /D "%~dp0"
cmd.exe "/c windows_pk3_qvm_only.cmd"
SET PAKNAME=upak0
del "%~dp0%PAKNAME%.zip"
del "%~dp0%PAKNAME%.pk3"
del "%~dp0DATA_DEPLOY_THIS\%PAKNAME%.pk3"
cd "%~dp0windows\base\vm"
powershell Compress-Archive '%~dp0data_to_include_%PAKNAME%\*' '%~dp0%PAKNAME%.zip'
md "%~dp0DATA_DEPLOY_THIS"
move "%~dp0%PAKNAME%.zip" "%~dp0DATA_DEPLOY_THIS\%PAKNAME%.pk3"
cls
dir "%~dp0DATA_DEPLOY_THIS"
pause


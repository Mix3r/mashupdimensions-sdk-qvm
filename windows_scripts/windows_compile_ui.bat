SET Q3ASM=windows\build\q3_ui.q3asm
SET GAMEDIR=base
cd /D "%~dp0"
cd ..
mkdir windows
mkdir windows\build
mkdir windows\%GAMEDIR%
mkdir windows\%GAMEDIR%\vm
mkdir windows\build\q3_ui
copy windows_scripts\lcc.exe  windows\build\q3_ui\
copy windows_scripts\q3cpp.exe  windows\build\q3_ui\
copy windows_scripts\q3rcc.exe  windows\build\q3_ui\
copy windows_scripts\q3asm.exe  windows\build\q3_ui\

echo -o "../../%GAMEDIR%/vm/ui">%Q3ASM%
echo ui_main>>%Q3ASM%
echo ..\ui_syscalls>>%Q3ASM%
echo ui_gameinfo>>%Q3ASM%
echo ui_atoms>>%Q3ASM%
echo ui_cinematics>>%Q3ASM%
echo ui_connect>>%Q3ASM%
echo ui_controls2>>%Q3ASM%
echo ui_challenges>>%Q3ASM%
echo ui_demo2>>%Q3ASM%
echo ui_mfield>>%Q3ASM%
echo ui_credits>>%Q3ASM%
echo ui_menu>>%Q3ASM%
echo ui_ingame>>%Q3ASM%
echo ui_confirm>>%Q3ASM%
echo ui_setup>>%Q3ASM%
echo ui_display>>%Q3ASM%
echo ui_firstconnect>>%Q3ASM%
echo ui_sound>>%Q3ASM%
echo ui_network>>%Q3ASM%
echo ui_password>>%Q3ASM%
echo ui_playermodel>>%Q3ASM%
echo ui_players>>%Q3ASM%
echo ui_playersettings>>%Q3ASM%
echo ui_preferences>>%Q3ASM%
echo ui_qmenu>>%Q3ASM%
echo ui_serverinfo>>%Q3ASM%
echo ui_servers2>>%Q3ASM%
echo ui_sparena>>%Q3ASM%
echo ui_specifyserver>>%Q3ASM%
echo ui_sppostgame>>%Q3ASM%
echo ui_splevel>>%Q3ASM%
echo ui_spskill>>%Q3ASM%
echo ui_startserver>>%Q3ASM%
echo ui_team>>%Q3ASM%
echo ui_video>>%Q3ASM%
echo ui_addbots>>%Q3ASM%
echo ui_removebots>>%Q3ASM%
echo ui_teamorders>>%Q3ASM%
echo ui_cdkey>>%Q3ASM%
echo ui_mods>>%Q3ASM%
echo ui_votemenu>>%Q3ASM%
echo ui_votemenu_custom>>%Q3ASM%
echo ui_votemenu_gametype>>%Q3ASM%
echo ui_votemenu_fraglimit>>%Q3ASM%
echo ui_votemenu_kick>>%Q3ASM%
echo ui_votemenu_map>>%Q3ASM%
echo ui_votemenu_timelimit>>%Q3ASM%
echo bg_misc>>%Q3ASM%
echo bg_lib>>%Q3ASM%
echo q_math>>%Q3ASM%
echo q_shared>>%Q3ASM%

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\q3_ui -I..\..\..\code\qcommon %1

cd windows\build\q3_ui

%cc%  ../../../code/q3_ui/ui_addbots.c
%cc%  ../../../code/q3_ui/ui_atoms.c
%cc%  ../../../code/q3_ui/ui_cdkey.c
%cc%  ../../../code/q3_ui/ui_challenges.c
%cc%  ../../../code/q3_ui/ui_cinematics.c
%cc%  ../../../code/q3_ui/ui_confirm.c
%cc%  ../../../code/q3_ui/ui_connect.c
%cc%  ../../../code/q3_ui/ui_controls2.c
%cc%  ../../../code/q3_ui/ui_credits.c
%cc%  ../../../code/q3_ui/ui_demo2.c
%cc%  ../../../code/q3_ui/ui_display.c
%cc%  ../../../code/q3_ui/ui_firstconnect.c
%cc%  ../../../code/q3_ui/ui_gameinfo.c
%cc%  ../../../code/q3_ui/ui_ingame.c
%cc%  ../../../code/q3_ui/ui_loadconfig.c
%cc%  ../../../code/q3_ui/ui_login.c
%cc%  ../../../code/q3_ui/ui_main.c
%cc%  ../../../code/q3_ui/ui_menu.c
%cc%  ../../../code/q3_ui/ui_mfield.c
%cc%  ../../../code/q3_ui/ui_mods.c
%cc%  ../../../code/q3_ui/ui_network.c
rem %cc%  ../../../code/q3_ui/ui_options.c
%cc%  ../../../code/q3_ui/ui_password.c
%cc%  ../../../code/q3_ui/ui_playermodel.c
%cc%  ../../../code/q3_ui/ui_players.c
%cc%  ../../../code/q3_ui/ui_playersettings.c
%cc%  ../../../code/q3_ui/ui_preferences.c
%cc%  ../../../code/q3_ui/ui_qmenu.c
%cc%  ../../../code/q3_ui/ui_rankings.c
%cc%  ../../../code/q3_ui/ui_rankstatus.c
%cc%  ../../../code/q3_ui/ui_removebots.c
%cc%  ../../../code/q3_ui/ui_saveconfig.c
%cc%  ../../../code/q3_ui/ui_serverinfo.c
%cc%  ../../../code/q3_ui/ui_servers2.c
%cc%  ../../../code/q3_ui/ui_setup.c
rem %cc%  ../../../code/q3_ui/ui_signup.c
%cc%  ../../../code/q3_ui/ui_sound.c
%cc%  ../../../code/q3_ui/ui_sparena.c
%cc%  ../../../code/q3_ui/ui_specifyleague.c
%cc%  ../../../code/q3_ui/ui_specifyserver.c
%cc%  ../../../code/q3_ui/ui_splevel.c
%cc%  ../../../code/q3_ui/ui_sppostgame.c
%cc%  ../../../code/q3_ui/ui_spreset.c
%cc%  ../../../code/q3_ui/ui_spskill.c
%cc%  ../../../code/q3_ui/ui_startserver.c
%cc%  ../../../code/q3_ui/ui_team.c
%cc%  ../../../code/q3_ui/ui_teamorders.c
%cc%  ../../../code/q3_ui/ui_video.c
%cc%  ../../../code/q3_ui/ui_votemenu.c
%cc%  ../../../code/q3_ui/ui_votemenu_fraglimit.c
%cc%  ../../../code/q3_ui/ui_votemenu_custom.c
%cc%  ../../../code/q3_ui/ui_votemenu_gametype.c
%cc%  ../../../code/q3_ui/ui_votemenu_kick.c
%cc%  ../../../code/q3_ui/ui_votemenu_map.c
%cc%  ../../../code/q3_ui/ui_votemenu_timelimit.c

copy  ..\..\..\code\ui\ui_syscalls.asm ..

%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../q3_ui

cd ..\..\..

rmdir .\windows\build /S /Q
cd /D "%~dp0"
cd "..\windows\%GAMEDIR%\vm"
IF NOT EXIST "%~dp0q3silent.txt" (
pause
"%WINDIR%\..\Program Files\Totalcmd_p\doublecmd.exe" -C "%CD%"
)
SET Q3ASM=windows\build\game.q3asm
SET GAMEDIR=base
cd /D "%~dp0"
cd ..
mkdir windows
mkdir windows\build
mkdir windows\%GAMEDIR%
mkdir windows\%GAMEDIR%\vm
mkdir windows\build\game
copy windows_scripts\lcc.exe  windows\build\game\
copy windows_scripts\q3cpp.exe  windows\build\game\
copy windows_scripts\q3rcc.exe  windows\build\game\
copy windows_scripts\q3asm.exe  windows\build\game\

echo -o "../../%GAMEDIR%/vm/qagame.qvm">%Q3ASM%
echo g_main>>%Q3ASM%
echo ..\g_syscalls>>%Q3ASM%
echo bg_misc>>%Q3ASM%
echo bg_lib>>%Q3ASM%
echo bg_pmove>>%Q3ASM%
echo bg_slidemove>>%Q3ASM%
echo q_math>>%Q3ASM%
echo q_shared>>%Q3ASM%
echo ai_dmnet>>%Q3ASM%
echo ai_dmq3>>%Q3ASM%
echo ai_team>>%Q3ASM%
echo ai_main>>%Q3ASM%
echo ai_chat>>%Q3ASM%
echo ai_cmd>>%Q3ASM%
echo ai_vcmd>>%Q3ASM%
echo g_active>>%Q3ASM%
echo g_admin>>%Q3ASM%
echo g_arenas>>%Q3ASM%
echo g_bot>>%Q3ASM%
echo g_client>>%Q3ASM%
echo g_cmds>>%Q3ASM%
echo g_cmds_ext>>%Q3ASM%
echo g_combat>>%Q3ASM%
echo g_elimination>>%Q3ASM%
echo g_items>>%Q3ASM%
echo bg_alloc>>%Q3ASM%
echo g_fileops>>%Q3ASM%
echo g_killspree>>%Q3ASM%
echo g_misc>>%Q3ASM%
echo g_missile>>%Q3ASM%
echo g_mover>>%Q3ASM%
echo g_playerstore>>%Q3ASM%
echo g_possession>>%Q3ASM%
echo g_session>>%Q3ASM%
echo g_spawn>>%Q3ASM%
echo g_svcmds>>%Q3ASM%
echo g_svcmds_ext>>%Q3ASM%
echo g_target>>%Q3ASM%
echo g_team>>%Q3ASM%
echo g_trigger>>%Q3ASM%
echo g_unlagged>>%Q3ASM%
echo g_utils>>%Q3ASM%
echo g_vote>>%Q3ASM%
echo g_weapon>>%Q3ASM%

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\game -I..\..\..\code\qcommon %1

cd windows\build\game

%cc%  ../../../code/game/ai_chat.c
%cc%  ../../../code/game/ai_cmd.c
%cc%  ../../../code/game/ai_dmnet.c
%cc%  ../../../code/game/ai_dmq3.c
%cc%  ../../../code/game/ai_main.c
%cc%  ../../../code/game/ai_team.c
%cc%  ../../../code/game/ai_vcmd.c
%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c
%cc%  ../../../code/game/bg_pmove.c
%cc%  ../../../code/game/bg_slidemove.c
%cc%  ../../../code/game/g_active.c
%cc%  ../../../code/game/g_admin.c
%cc%  ../../../code/game/g_arenas.c
%cc%  ../../../code/game/g_bot.c
%cc%  ../../../code/game/g_client.c
%cc%  ../../../code/game/g_cmds.c
%cc%  ../../../code/game/g_cmds_ext.c
%cc%  ../../../code/game/g_combat.c
%cc%  ../../../code/game/g_elimination.c
%cc%  ../../../code/game/g_items.c
%cc%  ../../../code/game/g_main.c
%cc%  ../../../code/game/bg_alloc.c
%cc%  ../../../code/game/g_fileops.c
%cc%  ../../../code/game/g_killspree.c
%cc%  ../../../code/game/g_misc.c
%cc%  ../../../code/game/g_missile.c
%cc%  ../../../code/game/g_mover.c
%cc%  ../../../code/game/g_playerstore.c
%cc%  ../../../code/game/g_possession.c
rem %cc%  ../../../code/game/g_rankings.c
%cc%  ../../../code/game/g_session.c
%cc%  ../../../code/game/g_spawn.c
%cc%  ../../../code/game/g_svcmds.c
%cc%  ../../../code/game/g_svcmds_ext.c
rem %cc%  ../../../code/game/g_syscalls.c
copy  ..\..\..\code\game\g_syscalls.asm ..
%cc%  ../../../code/game/g_target.c
%cc%  ../../../code/game/g_team.c
%cc%  ../../../code/game/g_trigger.c
%cc%  ../../../code/game/g_unlagged.c
%cc%  ../../../code/game/g_utils.c
%cc%  ../../../code/game/g_vote.c
%cc%  ../../../code/game/g_weapon.c

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../game

cd ..\..\..
rmdir .\windows\build /S /Q
cd /D "%~dp0"
cd "..\windows\%GAMEDIR%\vm"
IF NOT EXIST "%~dp0q3silent.txt" (
pause
"%WINDIR%\..\Program Files\Totalcmd_p\doublecmd.exe" -C "%CD%"
)
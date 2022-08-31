SET Q3ASM=windows\build\cgame.q3asm
SET GAMEDIR=base
cd /D "%~dp0"
cd ..
mkdir windows
mkdir windows\build
mkdir windows\%GAMEDIR%
mkdir windows\%GAMEDIR%\vm
mkdir windows\build\cgame
copy windows_scripts\lcc.exe  windows\build\cgame\
copy windows_scripts\q3cpp.exe  windows\build\cgame\
copy windows_scripts\q3rcc.exe  windows\build\cgame\
copy windows_scripts\q3asm.exe  windows\build\cgame\

echo -o "../../%GAMEDIR%/vm/cgame">%Q3ASM%
echo cg_main>>%Q3ASM%
echo ..\cg_syscalls>>%Q3ASM%
echo cg_challenges>>%Q3ASM%
echo cg_consolecmds>>%Q3ASM%
echo cg_draw>>%Q3ASM%
echo cg_drawtools>>%Q3ASM%
echo cg_effects>>%Q3ASM%
echo cg_ents>>%Q3ASM%
echo cg_event>>%Q3ASM%
echo cg_info>>%Q3ASM%
echo cg_localents>>%Q3ASM%
echo cg_marks>>%Q3ASM%
echo cg_players>>%Q3ASM%
echo cg_playerstate>>%Q3ASM%
echo cg_predict>>%Q3ASM%
echo cg_scoreboard>>%Q3ASM%
echo cg_servercmds>>%Q3ASM%
echo cg_snapshot>>%Q3ASM%
echo cg_unlagged>>%Q3ASM%
echo cg_view>>%Q3ASM%
echo cg_weapons>>%Q3ASM%
echo bg_slidemove>>%Q3ASM%
echo bg_pmove>>%Q3ASM%
echo bg_lib>>%Q3ASM%
echo bg_misc>>%Q3ASM%
echo q_math>>%Q3ASM%
echo q_shared>>%Q3ASM%

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\cgame -I..\..\..\code\qcommon %1
rem cc=lcc -DMISSIONPACK -DSCRIPTHUD -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\cgame -I..\..\..\code\ui -I..\..\..\code\qcommon %1

cd windows\build\cgame

%cc%  ../../../code/cgame/cg_challenges.c
%cc%  ../../../code/cgame/cg_consolecmds.c
%cc%  ../../../code/cgame/cg_draw.c
%cc%  ../../../code/cgame/cg_drawtools.c
%cc%  ../../../code/cgame/cg_effects.c
%cc%  ../../../code/cgame/cg_ents.c
%cc%  ../../../code/cgame/cg_event.c
%cc%  ../../../code/cgame/cg_info.c
%cc%  ../../../code/cgame/cg_localents.c
%cc%  ../../../code/cgame/cg_main.c
%cc%  ../../../code/cgame/cg_marks.c
rem %cc%  ../../../code/cgame/cg_newdraw.c
rem %cc%  ../../../code/cgame/cg_particles.c
%cc%  ../../../code/cgame/cg_players.c
%cc%  ../../../code/cgame/cg_playerstate.c
%cc%  ../../../code/cgame/cg_predict.c
%cc%  ../../../code/cgame/cg_scoreboard.c
%cc%  ../../../code/cgame/cg_servercmds.c
%cc%  ../../../code/cgame/cg_snapshot.c
%cc%  ../../../code/cgame/cg_unlagged.c
%cc%  ../../../code/cgame/cg_view.c
%cc%  ../../../code/cgame/cg_weapons.c

%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c
%cc%  ../../../code/game/bg_pmove.c
%cc%  ../../../code/game/bg_slidemove.c

copy  ..\..\..\code\cgame\cg_syscalls.asm ..

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../cgame
cd ..\..\..

rmdir .\windows\build /S /Q
cd /D "%~dp0"
cd "..\windows\%GAMEDIR%\vm"
IF NOT EXIST "%~dp0q3silent.txt" (
pause
"%WINDIR%\..\Program Files\Totalcmd_p\doublecmd.exe" -C "%CD%"
)
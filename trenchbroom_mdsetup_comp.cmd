IF "%~6"=="" goto bSETUP
IF %6 EQU bFAST goto bFAST
IF %6 EQU bFULL goto bFULL
IF %6 EQU bFULLNOSKY goto bFULLNOSKY
goto FINISH_HIM

rem setup trenchbroom if no 6 command line parameters present

:bSETUP
cd /D %~dp0
cd ..
SET GAMEPATH=%CD%
md "baseq3"
cd %APPDATA%
md "TrenchBroom"

SET EDITR1="%APPDATA%\TrenchBroom\Preferences.json"
echo ^{>%EDITR1%
echo ^"Games^/Quake 3^/Path^"^: ^"%GAMEPATH:\=\\%^"^,>>%EDITR1%
echo ^"Views^/Map view layout^"^: ^2^,>>%EDITR1%
echo ^"Renderer^/Texture mode mag filter^"^: 9729^,>>%EDITR1%
echo ^"Renderer^/Texture mode min filter^"^: 9987^,>>%EDITR1%
echo ^"Theme^"^: ^"Dark^">>%EDITR1%
echo ^}>>%EDITR1%

md "TrenchBroom\games"
md "TrenchBroom\games\Quake 3"

SET EDITR1="%APPDATA%\TrenchBroom\games\Quake 3\CompilationProfiles.cfg"
echo ^{>%EDITR1%
echo ^"profiles^"^: ^[>>%EDITR1%
echo ^{>>%EDITR1%
echo ^"name^"^: ^"fast compile^"^,>>%EDITR1%
echo ^"tasks^"^: [>>%EDITR1%
echo ^{>>%EDITR1%
echo ^"parameters^"^: ^"^\^"^$^{GAME_DIR_PATH^}^\^" ^\^"^$^{MODS^[^1^]^}^\^" ^\^"^$^{WORK_DIR_PATH^}^\^\^$^{MAP_FULL_NAME^}^\^" ^\^"^$^{MODS^[^0^]^}^\^" ^\^"^$^{MAP_BASE_NAME^}^\^" bFAST^"^,>>%EDITR1%
echo ^"tool^"^: ^"^$^{APP_DIR_PATH^}^\^\trenchbroom_mdsetup_comp.cmd^"^,>>%EDITR1%
echo ^"type^"^: ^"tool^">>%EDITR1%
echo ^}>>%EDITR1%
echo ^]^,>>%EDITR1%
echo ^"workdir^"^: ^"^$^{MAP_DIR_PATH^}^">>%EDITR1%
echo ^}^,>>%EDITR1%
echo ^{>>%EDITR1%
echo ^"name^"^: ^"full compile^"^,>>%EDITR1%
echo ^"tasks^"^: ^[>>%EDITR1%
echo ^{>>%EDITR1%
echo ^"parameters^"^: ^"^\^"^$^{GAME_DIR_PATH^}^\^" ^\^"^$^{MODS^[^1^]^}^\^" ^\^"^$^{WORK_DIR_PATH^}^\^\^$^{MAP_FULL_NAME^}^\^" ^\^"^$^{MODS^[^0^]^}^\^" ^\^"^$^{MAP_BASE_NAME^}^\^" bFULL^"^,>>%EDITR1%
echo ^"tool^"^: ^"^$^{APP_DIR_PATH^}^\^\trenchbroom_mdsetup_comp.cmd^"^,>>%EDITR1%
echo ^"type^"^: ^"tool^">>%EDITR1%
echo ^}>>%EDITR1%
echo ^]^,>>%EDITR1%
echo ^"workdir^"^: ^"^$^{MAP_DIR_PATH^}^">>%EDITR1%
echo ^}^,>>%EDITR1%
echo ^{>>%EDITR1%
echo ^"name^"^: ^"full compile nosky^"^,>>%EDITR1%
echo ^"tasks^"^: ^[>>%EDITR1%
echo ^{>>%EDITR1%
echo ^"parameters^"^: ^"^\^"^$^{GAME_DIR_PATH^}^\^" ^\^"^$^{MODS^[^1^]^}^\^" ^\^"^$^{WORK_DIR_PATH^}^\^\^$^{MAP_FULL_NAME^}^\^" ^\^"^$^{MODS^[^0^]^}^\^" ^\^"^$^{MAP_BASE_NAME^}^\^" bFULLNOSKY^"^,>>%EDITR1%
echo ^"tool^"^: ^"^$^{APP_DIR_PATH^}^\^\trenchbroom_mdsetup_comp.cmd^"^,>>%EDITR1%
echo ^"type^"^: ^"tool^">>%EDITR1%
echo ^}>>%EDITR1%
echo ^]^,>>%EDITR1%
echo ^"workdir^"^: ^"^$^{MAP_DIR_PATH^}^">>%EDITR1%
echo ^}>>%EDITR1%
echo ^]^,>>%EDITR1%
echo ^"version^"^: ^1>>%EDITR1%
echo ^}>>%EDITR1%

start "" "%~dp0trenchbroom.exe"
goto FINISH_HIM

rem compile map and start the map for test - fast mode for test, no lighmap at all!

:bFAST
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -meta -v %3
"%~dp0bspc" -forcesidesvisible -bsp2aas %3
"%~1\ioq3md.x64.exe" +set sv_pure 0 +set fs_game %~4 +devmap %5
goto FINISH_HIM

rem compile map and start the map for test - full mode for test, with lightmaps and skies enlight the map

:bFULL
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -meta -v %3
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -vis -v %3
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -light -fast -patchshadows -samples 3 -bounce 8 -gamma 2 -compensate 4 -dirty -v %3
"%~dp0bspc" -forcesidesvisible -bsp2aas %3
"%~1\ioq3md.x64.exe" +set sv_pure 0 +set fs_game %~4 +devmap %5
goto FINISH_HIM

rem compile map and start the map for test - full mode for test, with lightmaps, skies don't affect the lightmap

:bFULLNOSKY
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -meta -v %3
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -vis -v %3
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -light -fast -patchshadows -samples 3 -bounce 8 -sky 0.0 -gamma 2 -compensate 4 -dirty -v %3
"%~dp0bspc" -forcesidesvisible -bsp2aas %3
"%~1\ioq3md.x64.exe" +set sv_pure 0 +set fs_game %~4 +devmap %5
goto FINISH_HIM

:FINISH_HIM
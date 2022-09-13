IF NOT EXIST "%~dp0TrenchBroom.exe" (
    @setlocal enableextensions enabledelayedexpansion
    SET EDITR1="%~dp0Place TrenchBroom in this folder.htm"
    echo ^<HTML^>^<HEAD^>^<TITLE^>Get TrenchBroom^<^/TITLE^>^<^/HEAD^>>!EDITR1!
    echo ^<BODY BGCOLOR^=^#c0c0c0^ TEXT^=^#000000^>Place TrenchBroom in this folder. Download and extract Trenchbroom .7z file to^:>>!EDITR1!
    echo ^<p^>%~dp0>>!EDITR1!
    echo ^<p^>^<B^>^<a href=^"https^:^/^/github.com^/TrenchBroom^/TrenchBroom^/releases^" target=^"_blank^"^>LATEST TRENCHBROOM RELEASES^<^/a^>^<^/B^>^<^/BODY^>^<^/HTML^>>>!EDITR1!
    explorer.exe /select,!EDITR1!
    endlocal
    goto FINISH_HIM
)
IF "%~6"=="" goto bSETUP
goto bCOPYTHISMAP

rem setup trenchbroom if no 6 command line parameters present

:bSETUP
cd /D %~dp0
cd ..
SET GAMEPATH=%CD%
md "baseq3"
md "baseq3\maps"
cd /D %APPDATA%
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

rem copy source map text to exclude some redundant strings from worldspawn
rem but keep the strings in source map, because Trenchbroom uses 'em as map settings

:bCOPYTHISMAP
@setlocal enableextensions enabledelayedexpansion
SET BFIRST1=1
for /F "usebackq tokens=*" %%a in (%3) do (
    SET str0=%%a
    SET str1=x!str0:^"_tb_mod^" ^"=!
    SET str2=x!str0:^"_tb_textures^" ^"=!
    IF x!str0! == !str1! IF x!str0! == !str2! (
        if !BFIRST1! == 1 (
            echo !str0!>"%~dp3%5TEMP.map"
            SET BFIRST1=0
        ) else (
            echo !str0!>>"%~dp3%5TEMP.map"
        )
    )
)
endlocal

IF %6 EQU bFAST goto bFAST
IF %6 EQU bFULL goto bFULL
IF %6 EQU bFULLNOSKY goto bFULLNOSKY
del "%~dp3%5TEMP.map"
goto FINISH_HIM

rem compile map and start the map for test - fast mode for test, no lighmap at all!

:bFAST
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -meta -v "%~dp3%5TEMP.map"
move /Y "%~dp3%5TEMP.bsp" "%~dp3%5.bsp"
"%~dp0bspc" -forcesidesvisible -bsp2aas "%~dp3%5.bsp"
del "%~dp3%5TEMP.map"
"%~1\ioq3md.x64.exe" +set sv_pure 0 +set fs_game %~4 +devmap %5
goto FINISH_HIM

rem compile map and start the map for test - full mode for test, with lightmaps and skies enlight the map

:bFULL
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -meta -v "%~dp3%5TEMP.map"
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -vis -v "%~dp3%5TEMP.map"
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -light -fast -patchshadows -samples 3 -bounce 8 -gamma 2 -compensate 4 -dirty -v "%~dp3%5TEMP.map"
move /Y "%~dp3%5TEMP.bsp" "%~dp3%5.bsp"
"%~dp0bspc" -forcesidesvisible -bsp2aas "%~dp3%5.bsp"
del "%~dp3%5TEMP.map"
"%~1\ioq3md.x64.exe" +set sv_pure 0 +set fs_game %~4 +devmap %5
goto FINISH_HIM

rem compile map and start the map for test - full mode for test, with lightmaps, skies don't affect the lightmap

:bFULLNOSKY
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -meta -v "%~dp3%5TEMP.map"
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -vis -v "%~dp3%5TEMP.map"
"%~dp0q3map2" -fs_basepath %1 -fs_game %2 -light -fast -patchshadows -samples 3 -bounce 8 -sky 0.0 -gamma 2 -compensate 4 -dirty -v "%~dp3%5TEMP.map"
move /Y "%~dp3%5TEMP.bsp" "%~dp3%5.bsp"
"%~dp0bspc" -forcesidesvisible -bsp2aas "%~dp3%5.bsp"
del "%~dp3%5TEMP.map"
"%~1\ioq3md.x64.exe" +set sv_pure 0 +set fs_game %~4 +devmap %5

goto FINISH_HIM

:FINISH_HIM
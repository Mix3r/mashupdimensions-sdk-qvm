/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
/*
=======================================================================

USER INTERFACE MAIN

=======================================================================
*/


#include "ui_local.h"


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .qvm file
================
*/
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) {
	switch ( command ) {
	case UI_GETAPIVERSION:
		return UI_API_VERSION;

	case UI_INIT:
		UI_Init();
		return 0;

	case UI_SHUTDOWN:
		UI_Shutdown();
		return 0;

	case UI_KEY_EVENT:
		UI_KeyEvent( arg0, arg1 );
		return 0;

	case UI_MOUSE_EVENT:
		UI_MouseEvent( arg0, arg1 );
		return 0;

	case UI_REFRESH:
		UI_Refresh( arg0 );
		return 0;

	case UI_IS_FULLSCREEN:
		return UI_IsFullscreen();

	case UI_SET_ACTIVE_MENU:
		UI_SetActiveMenu( arg0 );
		return 0;

	case UI_CONSOLE_COMMAND:
		return UI_ConsoleCommand(arg0);

	case UI_DRAW_CONNECT_SCREEN:
		UI_DrawConnectScreen( arg0 );
		return 0;
	case UI_HASUNIQUECDKEY:				// mod authors need to observe this
		return qtrue;  // bk010117 - change this to qfalse for mods!
	}

	return -1;
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t *vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

vmCvar_t ui_ffa_fraglimit;
vmCvar_t ui_ffa_timelimit;
vmCvar_t ui_tourney_fraglimit;
vmCvar_t ui_tourney_timelimit;
vmCvar_t ui_team_fraglimit;
vmCvar_t ui_team_timelimit;
vmCvar_t ui_team_friendly;
vmCvar_t ui_ctf_capturelimit;
vmCvar_t ui_ctf_timelimit;
vmCvar_t ui_ctf_friendly;
vmCvar_t ui_1fctf_capturelimit;
vmCvar_t ui_1fctf_timelimit;
vmCvar_t ui_1fctf_friendly;
vmCvar_t ui_overload_capturelimit;
vmCvar_t ui_overload_timelimit;
vmCvar_t ui_overload_friendly;
vmCvar_t ui_harvester_capturelimit;
vmCvar_t ui_harvester_timelimit;
vmCvar_t ui_harvester_friendly;
vmCvar_t ui_elimination_capturelimit;
vmCvar_t ui_elimination_timelimit;
vmCvar_t ui_ctf_elimination_capturelimit;
vmCvar_t ui_ctf_elimination_timelimit;
vmCvar_t ui_lms_fraglimit;
vmCvar_t ui_lms_timelimit;
vmCvar_t ui_dd_capturelimit;
vmCvar_t ui_dd_timelimit;
vmCvar_t ui_dd_friendly;
vmCvar_t ui_dom_capturelimit;
vmCvar_t ui_dom_timelimit;
vmCvar_t ui_dom_friendly;
vmCvar_t ui_pos_scorelimit;
vmCvar_t ui_pos_timelimit;
vmCvar_t ui_arenasFile;
vmCvar_t ui_botsFile;
vmCvar_t ui_spScores1;
vmCvar_t ui_spScores2;
vmCvar_t ui_spScores3;
vmCvar_t ui_spScores4;
vmCvar_t ui_spScores5;
vmCvar_t ui_spAwards;
vmCvar_t ui_spVideos;
vmCvar_t ui_spSkill;
vmCvar_t ui_spSelection;
vmCvar_t ui_browserMaster;
vmCvar_t ui_browserGameType;
vmCvar_t ui_browserSortKey;
vmCvar_t ui_browserShowFull;
vmCvar_t ui_browserShowEmpty;
vmCvar_t ui_brassTime;
vmCvar_t ui_drawCrosshair;
vmCvar_t ui_drawCrosshairNames;
vmCvar_t ui_DrawGun; // Mix3r to save draw gun client game cvar
vmCvar_t ui_scoreplums; // Mix3r_Durachok: to save draw gun client game cvar
vmCvar_t ui_marks;
vmCvar_t ui_server1;
vmCvar_t ui_server2;
vmCvar_t ui_server3;
vmCvar_t ui_server4;
vmCvar_t ui_server5;
vmCvar_t ui_server6;
vmCvar_t ui_server7;
vmCvar_t ui_server8;
vmCvar_t ui_server9;
vmCvar_t ui_server10;
vmCvar_t ui_server11;
vmCvar_t ui_server12;
vmCvar_t ui_server13;
vmCvar_t ui_server14;
vmCvar_t ui_server15;
vmCvar_t ui_server16;
//vmCvar_t ui_cdkeychecked;
//new in beta 23:
vmCvar_t        ui_browserOnlyHumans;
//new in beta 37:
vmCvar_t ui_setupchecked;
vmCvar_t ui_developer;
vmCvar_t cl_language;
vmCvar_t pmove_fixed;
vmCvar_t pmove_msec;
vmCvar_t cg_delag;


void UI_DrawBlackHole( int *px, int *py ) {
        UI_DrawHandlePic( *px, *py, 128, 64, trap_R_RegisterShaderNoMip("menu/art_blueish/back_blackhole") );
}












char *strings_en[] = {
"", // Mix3r_Durachok 1 is postfix for path of lang dependent content
"", // 2 is fallback postfix of lang dependent content loop choice, leave empty
"CONTESTS & COOP",
"SETUP",
"DEMO",
"MODS",
"EXIT",
"^7Weapon position:",
"^7Third person view:",
"off-screen",
"right",
"left",
"centered",
"Mashup Dimensions. 2022, Lesnoy City.",
"Based upon OpenArena(c) 2005-2018, OpenArena team.",
"CHOOSE A LEVEL",
"RESET GAME?",
"Training tier",
"Tier",
"Frag limit:", //20
"Time limit:",
"Area closed",
"DIFFICULTY",
"Rookie",
"Pioneer",
"Trained",
"Master",
"Expert",
"GAME SERVER",
"Map autoswitch:",
"Game type:",
"Free for all",
"Team battle",
"Tournament",
"Capture the Flag",
"One Flag capture",
"Moonshiners",
"Mashup multiball",
"Clan arena",
"Clan arena CTF",
"Last Hero",
"Double Domination",
"Domination",
"Possession",
"Capture limit:",
"Friendly Fire:",
"Pure Server:",
"InstaGib:",
"All rockets:",
"Score type:",
"Physics:",
"Adjust for LAN play:",
"Server name:",
"Bots skill:",
"Human",
"0 = No limit.",
"Railguns only. One hit to frag.",
"Rocket launchers only. Infinite ammo.",
"Check client and server pk3 identity.",
"Only one team can capture in a round.",
"All players are equal.",
"Not all players are equal.",
"Physics depends on players framerates.",
"Physics are calculated at fixed intervals.",
"Physics are calculated exactly.",
"Framerate dependent or not.",
"Blue",     //4 char only!
"Red",
"Bot",
"Open",
"Framerate based",
"Fixed 125Hz",
"Fixed 91Hz",
"Precise",
"Name:",
"Player name",
"Your connection speed",
"Reliable hit detection of instant guns",
"Downloading missing mods and maps",
"FIRST CONNECTION",
"Data rate:",
"Lag compensation:",
"Automatic download:",
" Note: you can change these settings later using Setup.",
" Please, check these settings:",
"PLAYGROUNDS",
"Servers:",
"Sort by:",
"Show full:",
"Show empty:",
"Humans only:",
"Hide private:",
"Ping Time",
"Map name",
"Open Player Spots",
"Human Players",
"More info at www.moddb.com",
"Looking for servers.",
"Press SPACE to stop.",
"Master server not responding.",
"Servers not found.",
"All",
"FFA",
"Team DM",  // team deathmatch
"CTF",  // capture the flag
"1F CTF",  // one flag ctf
"Moonshnr",  // Overload
"Multball",  // Harvester
"Cl.Arena",
"Cl.CTFar",
"LastHero",
"D. Dom.",
"Dominat.",
"Possess.",
"JOIN BY IP",
"IP-address:",
"Port:",
"CAPTURE THE FLAG",
"ONE FLAG CAPTURE",
"MASHUP-MULTIBALL",
"MOONSHINERS",
"DUELS",
"BOXING CONTEST",
"COOP WALKTHROUGH",
"CLAN ARENA",
"LAST SURVIVOR",
"PLAYER",
"CONTROLS",
"SYSTEM",
"GAME",
"RESET SETTINGS",
"This will revert ALL",
"settings to defaults!",
"Handicap",
"Effects",
"Lowering handicap makes player weak,",
"giving more challenge in gameplay.",
"GRAPHICS",
"DISPLAY",
"SOUND",
"UPLINK",
"Sound volume:",
"Music volume:",
"Audio quality:",
"Low",
"Medium",
"High",
"Very high",
"Brightness:",
"Screen size:",
"LAN/Cable/xDSL",
"Show lagometer:",
"DRIVER INFO",
"Off",
"On",
"Bilinear",
"Trilinear",
"Lightmap (Normal)",
"Vertex (Low)",
"Default",
"16 bit",
"32 bit",
"Fast",
"Fastest",
"Custom",
"Graphics profile:",
"GL driver:",
"GL extensions:",
"Aspect ratio:",
"Resolution:",
"Fullscreen:",
"Lightning:",
"Flares:",
"Bloom:",
"Show frames per sec:",
"Geometry detail:",
"Texture detail:",
"Texture quality:",
"Texture filter:",
"Anisotropic:",
"Crosshair:",
"Crosshair indicates health:",
"Crosshair - red:",
"Crosshair - green:",
"Crosshair - blue:",
"Show simple items:",
"Keep weapon bar visible:",
"Marks on walls:",
"Eject brass:",
"Dynamic lights:",
"Show target names:",
"Quality sky:",
"Sync every frame:",
"Same models as player:",
"Show team overlay:",
"Chat beep:",
"Team chat beep:",
"YES ",
"NO",
"upper right",
"lower right",
"lower left",
"No demo records.",
"Mashup Dimensions",
"TEAM",
"ADD BOTS",
"REMOVE BOTS",
"ORDERS",
"VOTE",
"SERVER INFO",
"RESTART BATTLE",
"RESUME",
"LEAVE BATTLE",
"Show scores",
"Use item",
"Run / Walk",
"Forward",
"Back",
"Strafe left",
"Strafe right",
"Up / Jump",
"Down / Crouch",
"Turn left",
"Turn right",
"Turn / Strafe",
"Look up",
"Look down",
"Mouse look",
"Center view",
"Zoom reticle",
"Power fist",
"Machinegun",
"Shotgun",
"Grenade launcher",
"Rocket launcher",
"Electrodriver",
"RailGun",
"PlasmaGun",
"Annihilator Gun",
"Hook",
"NailGun",
"Mine thrower",
"Gatgun",
"Attack",
"Next weapon",
"Previous weapon",
"Gesture",
"Talk",
"Say to team",
"Say to target",
"Say to attacker",
"Voice chat",
"Show accuracy",
"Best weapon",
"NEVER",
"ALWAYS",
"NEW",
"BEST",
"NEW and BEST",
"LOOK",
"MOVE",
"ATTACK",
"MISC",
"Free look",
"Invert mouse",
"Smooth mouse",
"Always run",
"Weapon autoswitch",
"Mouse sensitivity",
"Team only voice chat",
"Joystick",
"Joystick threshold",
"Waiting for new key ... push ESC to cancel",
"Press ENTER or CLICK to change",
"Press BACKSPACE to clear",
"Use ARROW keys or CLICK to change",
"Join ^1RED",
"Join ^5BLUE",
"SPECTATOR",
"Team: ",
"I'm the leader",
"Defend the base",
"Follow me",
"Get the flag",
"Camp here",
"Report",
"I'm not a leader",
"Roam",
"Attack enemy base",
"Dominate A",
"Dominate B",
"Next level",
"Change level",
"Change contest",
"Kick player",
"Enable/disable warmup",
"Change frag limit",
"Change time limit",
"Custom vote",
"Vote YES",
"Vote NO",
"Enable warmup",
"Unlimited",
"SPECIFY PASSWORD",
"Password:",
"WARNING: This will wipe all",
"walkthrough progress of the game.",
"Use this only if you want to play",
"walkthrough again from start.",
"WALKTHROUGH",
"Free slots will be filled by bots in 10 seconds.",
"Accuracy",
"Impressive",
"Excellent",
"Melee",
"Frags",
"Elusive",
"Quaked Dimensions",
"Teamed Up Dimensions",
"OVERLOAD",
"HARVESTER",
"Scores:",
"Read COPYING_EN for details.",
"Gamma:",
"Open Dimensions",
"Mission",
"Mix3r_Durachok & e-sport community of N.Tura.",
"MULTIPLAYER",
"Desktop",
"Popup damage points",
NULL
};
char *strings_ru[] = {
"_ru", // Mix3r_Durachok 1 is postfix for path of lang dependent content
"", // 2 is fallback postfix of lang dependent content loop choice, leave empty
"СОСТЯЗАНИЯ И КООП",
"НАСТРОЙКИ",
"ДЕМО",
"МОДИФИКАЦИИ",
"ВЫХОД",
"^7Положение оружия:",
"^7Вид от третьего лица:",
"за экраном",
"справа",
"слева",
"по центру",
"Хитросплетённый мир. 2022, г. Лесной.",
"Основано на OpenArena(c) 2005-2018, Команда OpenArena.",
"ВЫБОР УРОВНЯ",
"СБРОС ИГРЫ?",
"Тренировочный тур",
"Тур",
"Лимит фрагов:", //20
"Лимит времени:",
"Район закрыт",
"СЛОЖНОСТЬ",
"Новичок",
"Пионер",
"Cпециалист",
"Мастер",
"Эксперт",
"СЕРВЕР ИГРЫ",
"Автосмена карты:",
"Тип игры:",
"Против всех",
"Командный бой",
"Турнир",
"Захват флага",
"Захват с одним флагом",
"Самогонщики",
"Мэшап-мультибол",
"Ликвидация",
"Ликвидация с флагом",
"Последний выживший",
"Двойное превосходство",
"Превосходство",
"Обладание", //44
"Лимит захватов:",
"Урон по своим:",
"Чистый сервер:",
"ИнстаГиб:",
"Всё-ракеты:",
"Метод счёта:",  //50
"Физика:",
"Оптимизация для лок.сети:",
"Имя сервера:",
"Навык ботов:",
"Человек",
"0 = Не ограничено.",
"Только рельсотрон. фраг с 1-го попадания.",
"Только ракетница с бесконечными ракетами.",
"Требовать одинаковые pk3 файлы.",
"Лишь одна команда может захватить в раунде.", //60
"Все игроки равны.",
"Не все игроки равны.",
"Физика зависит от частоты кадров игры.",
"Физика считается равными интервалами.",
"Физика точно рассчитывается.",
"Зависит или нет от частоты кадров.",
"Син.", //4 char only!
"Кр.",
"Бот",
"Откр", //70
"Зависит от кадров/cек",
"Фиксированнный 125Гц",
"Фиксированный 91Гц",
"Точный",
"Имя:",
"Название игрока",
"Ваша скорость связи",
"Надёжное попадание мгновенным оружием",
"Скачивание отсутствующих карт и модов",
"ПЕРВОЕ ПОДКЛЮЧЕНИЕ",
"Скорость данных:",
"Компенсация лагов:",
"Позволить скачивание:",
" Внимание: все параметры можно позже сменить в Настройках.",
" Пожалуйста, проверьте эти параметры:",
"ИГРОВЫЕ МЕСТА",
"Серверы:",
"Сортировка по:",
"Показ занятых:",
"Показ пустых:",
"Живые игроки:",
"Без частных:",
"Задержка",
"Имя карты",
"Свободные места",
"Живые игроки",
"Посетите www.moddb.com - Скоро",
"Поиск серверов.",
"Нажмите ПРОБЕЛ для остановки.",
"Нет ответа от Главного Сервера.",
"Серверы не найдены.",
"Все",
"Пр. всех",
"Ком. бой",  // team deathmatch
"Захв.Фл.",  // capture the flag
"Зах.1фл.",  // one flag ctf
"Самогонщ",  // Overload
"Мультбол",  // Harvester
"Ликвидац",
"Лик.зах.",
"Пос.гер.",
"Дв. дом.",
"Доминир.",
"Обладан.",
"ВХОД ПО IP",
"IP-адрес:",
"Порт:",
"ЗАХВАТ ФЛАГА",
"ЗАХВАТ ОДНОГО ФЛАГА",
"МЭШАП-МУЛЬТИБОЛ",
"САМОГОНЩИКИ",
"ДУЭЛИ",
"ТУРНИР ПО БОКСУ",
"КООП. ПРОХОЖДЕНИЕ",
"ЛИКВИДАЦИЯ",
"ПОСЛЕДНИЙ ВЫЖИВШИЙ",
"ИГРОК",
"УПРАВЛЕНИЕ",
"СИСТЕМА",
"ИГРА",
"СБРОС НАСТРОЕК",
"Это сбросит ВСЕ",
"настройки в стандарт!",
"Гандикап",
"Эффекты",
"Снижение гандикапа делает вас слабее,",
"давая больше сложности при игре.",
"ГРАФИКА",
"ЭКРАН",
"ЗВУК",
"СЕТЬ",
"Громкость звуков:",
"Громкость музыки:",
"Качество звука:",
"Низко",
"Средне",
"Высоко",
"Оч.высоко",
"Яркость:",
"Размер экрана:",
"Сеть/Кабель/DSL",
"Показывать лагомер:",
"О ДРАЙВЕРЕ",
"Выкл",
"Вкл",
"Билинейная",
"Трилинейная",
"Светокарта (Норма)",
"Вершинное (Низко)",
"Cтандартный",
"16 бит",
"32 бит",
"Быстро",
"Самое быстрое",
"Пользовательское",
"Настройки графики:",
"GL драйвер:",
"GL расширения:",
"Соотношение:",
"Разрешение:",
"Полный экран:",
"Освещение:",
"Блики:",
"Блум:",
"Показ FPS кадров в сек:",
"Детализация моделей:",
"Детализация текстур:",
"Качество текстур:",
"Фильтрация текстур:",
"Анизотропия:",
"Прицел:",
"Прицел выражает здоровье игрока:",
"Прицел - красный:",
"Прицел - зелёный:",
"Прицел - синий:",
"Значки вместо предметов:",
"Показывать оружие всегда:",
"Следы на стенах:",
"Выброс гильз:",
"Динамический свет:",
"Показать имя цели:",
"Качественное небо:",
"Синхронизация кадра:",
"Модель у всех, как у игрока:",
"Показывать колонки команд:",
"Сигнал при чате:",
"Сигнал при командном чате:",
"ДА",
"НЕТ",
"справа вверху",
"справа внизу",
"слева внизу",
"Нет демо-записей.",
"Хитросплетённый мир",
"УЧАСТИЕ",
"ДОБАВИТЬ БОТОВ",
"УДАЛИТЬ БОТОВ",
"ПРИКАЗЫ",
"ГОЛОСОВАНИЕ",
"О СЕРВЕРЕ",
"ПЕРЕЗАПУСК БОЯ",
"ПРОДОЛЖИТЬ",
"ПОКИНУТЬ БОЙ",
"Показать счёт",
"Применить предмет",
"Бежать / Идти",
"Идти вперёд",
"Пятиться",
"Шаги влево",
"Шаги вправо",
"Вверх / прыжок",
"Вниз / присесть",
"Поворот влево",
"Поворот вправо",
"Шаг / поворот",
"Смотреть вверх",
"Смотреть вниз",
"Обзор мышью",
"Смотреть в центр",
"Приблизить вид",
"Крагошокер",
"Пулемёт",
"Дробовик",
"Гранатомёт",
"Ракетница",
"Электрошафт",
"Рельсотрон",
"Плазмомёт",
"Аннигилятор",
"Кошка",
"Гвоздомёт",
"Миноукладчик",
"Картечница",
"Атаковать",
"Следующее оружие",
"Предыдущее оружие",
"Жест",
"Сказать",
"Сказать команде",
"Сказать цели",
"Сказать напавшему",
"Голосовая связь",
"Показать точность",
"Лучшее оружие",
"НИКОГДА",
"ВСЕГДА",
"НОВОЕ",
"ЛУЧШЕЕ",
"НОВОЕ и ЛУЧШЕЕ",
"ОБЗОР",
"МАНЕВР",
"АТАКА",
"РАЗНОЕ",
"Обзор мышью",
"Обратить мышь",
"Плавная мышь",
"Всегда бежать",
"Автосмена оружия",
"Скорость мыши",
"Голос слышит только команда",
"Джойстик",
"Порог джойстика",
"Ожидание новой клавиши ... ESCAPE для отмены",
"Нажмите ENTER или ЩЕЛКНИТЕ для замены",
"Нажмите BACKSPACE, чтобы убрать",
"Используйте клавиши со стрелками или ЩЕЛКНИТЕ для замены",
"Играть за ^1КРАСНЫХ",
"Играть за ^5СИНИХ",
"ЗРИТЕЛЬ",
"Команда: ",
"Я командир",
"Охраняй базу",
"Следуй за мной",
"Захвати флаг",
"Закрепись тут",
"Докладывай",
"Я не командир",
"Отбой",
"Атакуй базу врага",
"Охраняй A",
"Охраняй B",
"Следующая арена",
"Сменить арену",
"Сменить состязание",
"Выгнать игрока",
"Вкл/выкл разминку",
"Сменить лимит фрагов",
"Сменить лимит времени",
"Custom vote",
"Голосовать ДА",
"Голосовать НЕТ",
"Включить разминку",
"Неограничено",
"УКАЖИТЕ ПАРОЛЬ",
"Пароль:",
"ВНИМАНИЕ: Это сбросит весь",
"прогресс прохождения игры.",
"Делайте сброс только если",
"желаете проходить заново.",
"ПРОХОЖДЕНИЕ", // Mix3r_Durachok 310 is postfix for path of lang dependent content
"Свободные места будут заняты ботами за 10 секунд.",
"Точность",
"Впечатляет",
"Отлично",
"Рукопашный",
"Фраги",
"Несокрушимый",
"Дрожащий Мир",
"Сплочённый Мир",
"ВАНДАЛИЗМ",
"ЖАТВА",
"Счёт до:",
"Читайте COPYING для подробностей.",
"Гамма:",
"Открытый Мир",
"Задание",
"Mix3r_Durachok и Тайное общество киберспортсменов.",
"ИГРА С ДРУГИМИ",
"Рабочий стол",
"Выпадающие числа урона",
NULL
};

// bk001129 - made static to avoid aliasing.
static cvarTable_t cvarTable[] = {
	{ &ui_ffa_fraglimit, "ui_ffa_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_ffa_timelimit, "ui_ffa_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_tourney_fraglimit, "ui_tourney_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_tourney_timelimit, "ui_tourney_timelimit", "15", CVAR_ARCHIVE },

	{ &ui_team_fraglimit, "ui_team_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_team_timelimit, "ui_team_timelimit", "20", CVAR_ARCHIVE },
	{ &ui_team_friendly, "ui_team_friendly",  "1", CVAR_ARCHIVE },

	{ &ui_ctf_capturelimit, "ui_ctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_timelimit, "ui_ctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_ctf_friendly, "ui_ctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_1fctf_capturelimit, "ui_1fctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_1fctf_timelimit, "ui_1fctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_1fctf_friendly, "ui_1fctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_overload_capturelimit, "ui_overload_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_overload_timelimit, "ui_overload_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_overload_friendly, "ui_overload_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_harvester_capturelimit, "ui_harvester_capturelimit", "20", CVAR_ARCHIVE },
	{ &ui_harvester_timelimit, "ui_harvester_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_harvester_friendly, "ui_harvester_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_elimination_capturelimit, "ui_elimination_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_elimination_timelimit, "ui_elimination_timelimit", "20", CVAR_ARCHIVE },

	{ &ui_ctf_elimination_capturelimit, "ui_ctf_elimination_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_elimination_timelimit, "ui_ctf_elimination_timelimit", "30", CVAR_ARCHIVE },

	{ &ui_lms_fraglimit, "ui_lms_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_lms_timelimit, "ui_lms_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_dd_capturelimit, "ui_dd_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_dd_timelimit, "ui_dd_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_dd_friendly, "ui_dd_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_dom_capturelimit, "ui_dom_capturelimit", "500", CVAR_ARCHIVE },
	{ &ui_dom_timelimit, "ui_dom_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_dom_friendly, "ui_dom_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_pos_scorelimit, "ui_pos_scorelimit", "120", CVAR_ARCHIVE },
	{ &ui_pos_timelimit, "ui_pos_timelimit", "20", CVAR_ARCHIVE },

	{ &ui_arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_spScores1, "g_spScores1", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores2, "g_spScores2", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores3, "g_spScores3", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores4, "g_spScores4", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores5, "g_spScores5", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spAwards, "g_spAwards", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spVideos, "g_spVideos", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE | CVAR_LATCH },

	{ &ui_spSelection, "ui_spSelection", "", CVAR_ROM },

	{ &ui_browserMaster, "ui_browserMaster", "1", CVAR_ARCHIVE },
	{ &ui_browserGameType, "ui_browserGameType", "0", CVAR_ARCHIVE },
	{ &ui_browserSortKey, "ui_browserSortKey", "4", CVAR_ARCHIVE },
	{ &ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE },
	{ &ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE },

	{ &ui_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE },
	{ &ui_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE },
	{ &ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
        { &ui_DrawGun, "cg_DrawGun", "1", CVAR_ARCHIVE },
        { &ui_scoreplums, "cg_ScorePlums", "1", CVAR_ARCHIVE },
	{ &ui_marks, "cg_marks", "1", CVAR_ARCHIVE },

	{ &ui_server1, "server1", "", CVAR_ARCHIVE },
	{ &ui_server2, "server2", "", CVAR_ARCHIVE },
	{ &ui_server3, "server3", "", CVAR_ARCHIVE },
	{ &ui_server4, "server4", "", CVAR_ARCHIVE },
	{ &ui_server5, "server5", "", CVAR_ARCHIVE },
	{ &ui_server6, "server6", "", CVAR_ARCHIVE },
	{ &ui_server7, "server7", "", CVAR_ARCHIVE },
	{ &ui_server8, "server8", "", CVAR_ARCHIVE },
	{ &ui_server9, "server9", "", CVAR_ARCHIVE },
	{ &ui_server10, "server10", "", CVAR_ARCHIVE },
	{ &ui_server11, "server11", "", CVAR_ARCHIVE },
	{ &ui_server12, "server12", "", CVAR_ARCHIVE },
	{ &ui_server13, "server13", "", CVAR_ARCHIVE },
	{ &ui_server14, "server14", "", CVAR_ARCHIVE },
	{ &ui_server15, "server15", "", CVAR_ARCHIVE },
	{ &ui_server16, "server16", "", CVAR_ARCHIVE },
	{ &ui_browserOnlyHumans, "ui_browserOnlyHumans", "0", CVAR_ARCHIVE },
	{ &ui_setupchecked, "ui_setupchecked", "0", CVAR_ARCHIVE },
	{ &ui_developer, "ui_developer", "0", CVAR_CHEAT },
        { &cl_language, "cl_language", "ru", CVAR_ARCHIVE },
        { &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO},
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO},
        { &cg_delag, "cg_delag", "1", CVAR_ARCHIVE | CVAR_USERINFO},
	{ NULL, "g_localTeamPref", "", 0 }
};

// bk001129 - made static to avoid aliasing
static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

void UI_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}
}

void UI_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		if ( !cv->vmCvar ) {
			continue;
		}
		trap_Cvar_Update( cv->vmCvar );
	}
}

/*
==================
 * UI_SetDefaultCvar
 * If the cvar is blank it will be set to value
 * This is only good for cvars that cannot naturally be blank
================== 
 */
void UI_SetDefaultCvar(const char* cvar, const char* value) {
	if(strlen(UI_Cvar_VariableString(cvar)) == 0) {
		trap_Cvar_Set(cvar,value);
	}
}

/* Neon_Knight: Useful check in order to have code consistency. */
/*
===================
UI_IsATeamGametype

Checks if the gametype is a team-based game.
===================
 */
qboolean UI_IsATeamGametype(int check) {
	return GAMETYPE_IS_A_TEAM_GAME(check);
}
/*
===================
UI_UsesTeamFlags

Checks if the gametype makes use of the red and blue flags.
===================
 */
qboolean UI_UsesTeamFlags(int check) {
	return GAMETYPE_USES_RED_AND_BLUE_FLAG(check);
}
/*
===================
UI_UsesTheWhiteFlag

Checks if the gametype makes use of the neutral flag.
===================
 */
qboolean UI_UsesTheWhiteFlag(int check) {
	return GAMETYPE_USES_WHITE_FLAG(check);
}
/*
===================
UI_IsARoundBasedGametype

Checks if the gametype has a round-based system.
===================
 */
qboolean UI_IsARoundBasedGametype(int check) {
	return GAMETYPE_IS_ROUND_BASED(check);
}
/* /Neon_Knight */

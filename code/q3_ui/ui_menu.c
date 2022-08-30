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

MAIN MENU

=======================================================================
*/


#include "ui_local.h"
#include "../qcommon/oa_version.h"


#define ID_SINGLEPLAYER			10
#define ID_MULTIPLAYER			11
#define ID_SETUP				12
#define ID_DEMOS				13
//#define ID_CINEMATICS			14
#define ID_CHALLENGES                   14
#define ID_TEAMARENA		15
#define ID_MODS					16
#define ID_EXIT					17
#define ID_THIRDPRSON				700
#define ID_LANG 				777
#define MAIN_BANNER_MODEL				"models/mapobjects/banner/banner5.md3"
#define MAIN_MENU_VERTICAL_SPACING		34
#define ID_CUSTOMIZEPLAYER		9


typedef struct {
	menuframework_s	menu;

	menutext_s		singleplayer;
	menutext_s		multiplayer;
	menutext_s		setup;
	menutext_s		demos;
	//menutext_s		cinematics;
	menutext_s              challenges;
	menutext_s		teamArena;
	menutext_s		mods;
	menutext_s		exit;
	qhandle_t		bannerModel;
        // player model main menu
        playerInfo_t		playerinfo;
        char			playerModel[MAX_QPATH];
        menubitmap_s		player;
        menuradiobutton_s	thirdprson;
        menulist_s		weapplacement;
        menulist_s		uilanguage;
        //int                     player_z_look;

} mainmenu_t;


static mainmenu_t s_main;

const char *weapplacement_names[] = {
        "",
        "",
        "",
        "",
        NULL
};

const char *mlang_names[] = {
        "^7[^2 ENG ^7]",
        "^7[^2 RUS ^7]",
        NULL
};

typedef struct {
	menuframework_s menu;
	char errorMessage[4096];
} errorMessage_t;

static errorMessage_t s_errorMessage;

/*
=================
MainMenu_ExitAction
=================
*/
/*static void MainMenu_ExitAction( qboolean result ) {
	if( !result ) {
		return;
	}
	UI_PopMenu();
	//UI_CreditMenu();
        trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
}*/



/*
=================
Main_MenuEvent
=================
*/
void Main_MenuEvent (void* ptr, int event)
{
	if( event != QM_ACTIVATED ) {
		return;
	}



	switch( ((menucommon_s*)ptr)->id ) {
	case ID_SINGLEPLAYER:
                if (ui_is_missionpack) {
                        trap_Cvar_SetValue( "ui_spSelection", 0 );
                }
		UI_SPLevelMenu();
                //trap_Cvar_Set( "nextmap", "levelselect" );
		//trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect; cinematic intro_mma.RoQ\n" );
                //trap_Cmd_ExecuteText( EXEC_APPEND, "play video/intro_mma.wav\n" );
		break;

	case ID_MULTIPLAYER:
		if(ui_setupchecked.integer)
			UI_ArenaServersMenu();
		else
			UI_FirstConnectMenu();
		break;

	case ID_SETUP:
		UI_SetupMenu();
		break;

	case ID_DEMOS:
		UI_DemosMenu();
		break;

	case ID_LANG:
		//UI_CinematicsMenu();
                switch (s_main.uilanguage.curvalue) {
                    case 0:
                        trap_Cvar_Set( "cl_language", "en");
                    break;
                    case 1:
                        trap_Cvar_Set( "cl_language", "ru");
                    break;
                }
                s_main.uilanguage.generic.id = 0; // Mix3r_Durachok crazy hack to redraw menu on next tick (frame) & revert back to ID_LANG
		break;

	case ID_CHALLENGES:
		UI_ContestsMenu();
                //trap_Cmd_ExecuteText( EXEC_APPEND, "exec ctf.cfg" );
		break;

        case ID_CUSTOMIZEPLAYER:
		UI_PlayerSettingsMenu();
		break;

	case ID_MODS:
		UI_ModsMenu();
		break;

	case ID_TEAMARENA:
	//	trap_Cvar_Set( "fs_game", "missionpack");
	//	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
                trap_Cvar_SetValue( "cg_drawGun", s_main.weapplacement.curvalue );
		break;

	case ID_EXIT:
		//UI_ConfirmMenu( "EXIT GAME?", 0, MainMenu_ExitAction );
		UI_CreditMenu();
		break;
        case ID_THIRDPRSON:
                trap_Cvar_SetValue( "cg_ThirdPerson", s_main.thirdprson.curvalue );
                break;
        }
}


/*
===============
MainMenu_Cache
===============
*/
void MainMenu_Cache( void )
{
	//s_main.bannerModel = trap_R_RegisterModel( MAIN_BANNER_MODEL );
}

sfxHandle_t ErrorMessage_Key(int key)
{
	trap_Cvar_Set( "com_errorMessage", "" );
	UI_MainMenu();
	return (menu_null_sound);
}

/*
===============
Main_MenuDraw
TTimo: this function is common to the main menu and errorMessage menu
===============
*/

static void Main_DrawPlayer( void *self ) {
	//menubitmap_s	*b;
	float                   jiggle_speed_div;
	char			buf[MAX_QPATH];
        char			bufhead[MAX_QPATH];

	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
        trap_Cvar_VariableStringBuffer( "headmodel", bufhead, sizeof( bufhead ) );
	if ( !strequals( va("%s:%s", buf, bufhead), s_main.playerModel ) ) {
		UI_PlayerInfo_SetModel( &s_main.playerinfo, buf, bufhead );
		strcpy( s_main.playerModel, va("%s:%s", buf, bufhead) );
                s_main.playerinfo.viewAngles[PITCH] = 0;
		s_main.playerinfo.viewAngles[ROLL]  = 0;
                s_main.playerinfo.viewAngles[YAW]  = 150;
                UI_PlayerInfo_SetInfo( &s_main.playerinfo, LEGS_IDLE, TORSO_STAND, s_main.playerinfo.viewAngles, vec3_origin, WP_MACHINEGUN, qfalse );
	}

        if ( Menu_ItemAtCursor( &s_main.menu ) == &s_main.player ) {
                s_main.playerinfo.legs.yawAngle  = s_main.playerinfo.legs.yawAngle + uis.frametime * 0.5;
                s_main.playerinfo.viewAngles[YAW] = s_main.playerinfo.legs.yawAngle + uis.frametime * 0.7;
                if (s_main.playerinfo.legs.yawAngle >= 180) {
                        s_main.playerinfo.legs.yawAngle = 180;
                        s_main.playerinfo.viewAngles[YAW] = s_main.playerinfo.legs.yawAngle;
                        jiggle_speed_div = 1.75;
                } else {
                        jiggle_speed_div = 1;
                }
        } else {
                s_main.playerinfo.legs.yawAngle  = s_main.playerinfo.legs.yawAngle - uis.frametime * 0.5;
                s_main.playerinfo.viewAngles[YAW] = s_main.playerinfo.legs.yawAngle - uis.frametime * 0.7;
                if (s_main.playerinfo.legs.yawAngle < 150) {
                        s_main.playerinfo.legs.yawAngle = 150;
                        s_main.playerinfo.viewAngles[YAW] = s_main.playerinfo.legs.yawAngle;
                }
                jiggle_speed_div = 2;
        }
        s_main.playerinfo.torso.yawAngle = s_main.playerinfo.legs.yawAngle;

	//b = (menubitmap_s*) self;
        //debug_tmpy = (int)trap_Cvar_VariableValue( "debuggraph" );

	UI_DrawPlayer( 450, 77, 32*9, 56*9, &s_main.playerinfo, uis.realtime/jiggle_speed_div );
}

static void Main_MenuDraw( void )
{
	//refdef_t		refdef;
	//refEntity_t		ent;
	vec3_t			origin;
	vec3_t			angles;
	float			adjust;
	float			x, y, w, h;
	vec4_t			color = {1, 1, 1, 1.0};

	// setup the refdef
 /*
	memset( &refdef, 0, sizeof( refdef ) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	x = 0;
	y = 0;
	w = 640;
	h = 120;
	UI_AdjustFrom640( &x, &y, &w, &h );
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	adjust = 0; // JDC: Kenneth asked me to stop this 1.0 * sin( (float)uis.realtime / 1000 );
	refdef.fov_x = 60 + adjust;
	refdef.fov_y = 19.6875 + adjust;

	refdef.time = uis.realtime;

	origin[0] = 300;
	origin[1] = 0;
	origin[2] = -32;

	trap_R_ClearScene();

	// add the model

	//memset( &ent, 0, sizeof(ent) );

	//adjust = 5.0 * sin( (float)uis.realtime / 5000 );
	//VectorSet( angles, 0, 180 + adjust, 0 );
	//AnglesToAxis( angles, ent.axis );
	//ent.hModel = s_main.bannerModel;
	//VectorCopy( origin, ent.origin );
	//VectorCopy( origin, ent.lightingOrigin );
	//ent.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
	//VectorCopy( ent.origin, ent.oldorigin );

	// trap_R_AddRefEntityToScene( &ent );

	trap_R_RenderScene( &refdef );
 */


	if (strlen(s_errorMessage.errorMessage)) {
		UI_DrawProportionalString_AutoWrapped( 320, 192, 600, 20, s_errorMessage.errorMessage, UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, menu_text_color );
	} else {
		// standard menu drawing
                //if (s_main.uilanguage.curvalue == 1) {
                //        uis.menuBackShader = trap_R_RegisterShaderNoMip("textures/sfx/logo512_ru");
                //} else {
                //        uis.menuBackShader = trap_R_RegisterShaderNoMip("textures/sfx/logo512");
                //}
		Menu_Draw( &s_main.menu );
	}

        s_main.thirdprson.curvalue = trap_Cvar_VariableValue( "cg_ThirdPerson" ) != 0;
        s_main.weapplacement.curvalue = Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawGun" ) );

        // Mix3r_Durachok: get language
        if (Q_strequal(cl_language.string, "ru")) {
                s_main.uilanguage.curvalue = 1;
        } else {
                s_main.uilanguage.curvalue = 0;
        }
        if (s_main.uilanguage.generic.id != ID_LANG) {
                //s_main.uilanguage.generic.id = ID_LANG;
                UI_MainMenu();
                Menu_Cache();
        }

        UI_DrawHandlePic( 260, 50, 32, 32, trap_R_RegisterShaderNoMip("menu/art/lang_globe") );

	UI_DrawString( 320, 400, COM_Localize(14), UI_CENTER|UI_SMALLFONT, color );
	UI_DrawString( 320, 414, COM_Localize(327), UI_CENTER|UI_SMALLFONT, color );
	UI_DrawString( 320, 428, COM_Localize(15), UI_CENTER|UI_SMALLFONT, color );
	UI_DrawString( 320, 444, COM_Localize(323), UI_CENTER|UI_SMALLFONT, color );

	//Draw version.
	UI_DrawString( 640-2, 480-14, "^7 " OA_VERSION, UI_RIGHT|UI_SMALLFONT, color );
	//if ((int)trap_Cvar_VariableValue("protocol")!=OA_STD_PROTOCOL) {
	//	UI_DrawString( 0, 480-14, va("^7Протокол: %i",(int)trap_Cvar_VariableValue("protocol")), UI_SMALLFONT, color);
	//}
}


/*
===============
UI_TeamArenaExists
===============
*/
/*
static qboolean UI_TeamArenaExists( void )
{
	int		numdirs;
	char	dirlist[2048];
	char	*dirptr;
	char  *descptr;
	int		i;
	int		dirlen;

	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for( i = 0; i < numdirs; i++ ) {
		dirlen = strlen( dirptr ) + 1;
		descptr = dirptr + dirlen;
		if ( Q_strequal(dirptr, "missionpack") ) {
			return qtrue;
		}
		dirptr += dirlen + strlen(descptr) + 1;
	}
	return qfalse;
}
*/


/*
===============
UI_MainMenu

The main menu only comes up when not in a game,
so make sure that the attract loop server is down
and that local cinematics are killed
===============
*/
void UI_MainMenu( void )
{
	int		y;
	qboolean teamArena = qfalse;
	int		style = UI_CENTER | UI_DROPSHADOW;

	trap_Cvar_Set( "sv_killserver", "1" );
	trap_Cvar_SetValue( "handicap", 100 ); //Reset handicap during server change, it must be ser per game

	memset( &s_main, 0 ,sizeof(mainmenu_t) );
	memset( &s_errorMessage, 0 ,sizeof(errorMessage_t) );

	// com_errorMessage would need that too
	MainMenu_Cache();

        // s_firstconnect.delaghitscan.curvalue	= trap_Cvar_VariableValue( "cg_delag" ) != 0;

	trap_Cvar_VariableStringBuffer( "com_errorMessage", s_errorMessage.errorMessage, sizeof(s_errorMessage.errorMessage) );
	if (strlen(s_errorMessage.errorMessage)) {
		s_errorMessage.menu.draw = Main_MenuDraw;
		s_errorMessage.menu.key = ErrorMessage_Key;
		s_errorMessage.menu.fullscreen = qtrue;
		s_errorMessage.menu.wrapAround = qtrue;
		s_errorMessage.menu.showlogo = qtrue;

		trap_Key_SetCatcher( KEYCATCH_UI );
		uis.menusp = 0;
		UI_PushMenu ( &s_errorMessage.menu );

		return;
	}

	s_main.menu.draw = Main_MenuDraw;
	s_main.menu.fullscreen = qtrue;
	s_main.menu.wrapAround = qtrue;
	s_main.menu.showlogo = qtrue;

	y = 124;
	s_main.singleplayer.generic.type		= MTYPE_PTEXT;
	s_main.singleplayer.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.singleplayer.generic.x			= 320;
	s_main.singleplayer.generic.y			= y;
	s_main.singleplayer.generic.id			= ID_SINGLEPLAYER;
	s_main.singleplayer.generic.callback	= Main_MenuEvent;
        s_main.singleplayer.string                      = COM_Localize(310);
	s_main.singleplayer.color			= color_white;
	s_main.singleplayer.style			= style;

	y += MAIN_MENU_VERTICAL_SPACING;
	s_main.multiplayer.generic.type			= MTYPE_PTEXT;
	s_main.multiplayer.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.multiplayer.generic.x			= 320;
	s_main.multiplayer.generic.y			= y;
	s_main.multiplayer.generic.id			= ID_MULTIPLAYER;
	s_main.multiplayer.generic.callback		= Main_MenuEvent;
	s_main.multiplayer.string				= COM_Localize(328);
	s_main.multiplayer.color				= color_white;
	s_main.multiplayer.style				= style;

        y += MAIN_MENU_VERTICAL_SPACING;
	s_main.challenges.generic.type			= MTYPE_PTEXT;
	s_main.challenges.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.challenges.generic.x				= 320;
	s_main.challenges.generic.y				= y;
	s_main.challenges.generic.id			= ID_CHALLENGES;
	s_main.challenges.generic.callback		= Main_MenuEvent;
	s_main.challenges.string				= COM_Localize(3);
	s_main.challenges.color					= color_white;
	s_main.challenges.style					= style;

	y += MAIN_MENU_VERTICAL_SPACING;
	s_main.setup.generic.type				= MTYPE_PTEXT;
	s_main.setup.generic.flags				= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.setup.generic.x					= 320;
	s_main.setup.generic.y					= y;
	s_main.setup.generic.id					= ID_SETUP;
	s_main.setup.generic.callback			= Main_MenuEvent;
	s_main.setup.string						= COM_Localize(4);
	s_main.setup.color						= color_white;
	s_main.setup.style						= style;

	y += MAIN_MENU_VERTICAL_SPACING;
	s_main.demos.generic.type				= MTYPE_PTEXT;
	s_main.demos.generic.flags				= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.demos.generic.x					= 320;
	s_main.demos.generic.y					= y;
	s_main.demos.generic.id					= ID_DEMOS;
	s_main.demos.generic.callback			= Main_MenuEvent;
	s_main.demos.string						= COM_Localize(5);
	s_main.demos.color						= color_white;
	s_main.demos.style						= style;

	y += MAIN_MENU_VERTICAL_SPACING;
	s_main.mods.generic.type			= MTYPE_PTEXT;
	s_main.mods.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.mods.generic.x				= 320;
	s_main.mods.generic.y				= y;
	s_main.mods.generic.id				= ID_MODS;
	s_main.mods.generic.callback		= Main_MenuEvent;
	s_main.mods.string					= COM_Localize(6);
	s_main.mods.color					= color_white;
	s_main.mods.style					= style;

	y += MAIN_MENU_VERTICAL_SPACING;
	s_main.exit.generic.type				= MTYPE_PTEXT;
	s_main.exit.generic.flags				= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_main.exit.generic.x					= 320;
	s_main.exit.generic.y					= y;
	s_main.exit.generic.id					= ID_EXIT;
	s_main.exit.generic.callback			= Main_MenuEvent;
	s_main.exit.string						= COM_Localize(7);
	s_main.exit.color						= color_white;
	s_main.exit.style						= style;




        s_main.player.generic.type		= MTYPE_BITMAP;
	s_main.player.generic.flags		= QMF_CENTER_JUSTIFY;
        s_main.player.generic.callback = Main_MenuEvent;
        s_main.player.generic.id       = ID_CUSTOMIZEPLAYER;
	s_main.player.generic.ownerdraw	= Main_DrawPlayer;
	s_main.player.generic.x			= 528+50;
	s_main.player.generic.y			= 210;
	s_main.player.width			= 640-528;
	s_main.player.height			= 440-210;

        weapplacement_names[0] = COM_Localize(10);
        weapplacement_names[1] = COM_Localize(11);
        weapplacement_names[2] = COM_Localize(12);
        weapplacement_names[3] = COM_Localize(13);

        s_main.weapplacement.generic.type     = MTYPE_SPINCONTROL;
	s_main.weapplacement.generic.name     = COM_Localize(8);
	s_main.weapplacement.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_main.weapplacement.generic.callback = Main_MenuEvent;
	s_main.weapplacement.generic.id       = ID_TEAMARENA;
	s_main.weapplacement.generic.x	      = 190-17;
	s_main.weapplacement.generic.y	      = 58;
	s_main.weapplacement.itemnames	= weapplacement_names;

        s_main.thirdprson.generic.type     = MTYPE_RADIOBUTTON;
	s_main.thirdprson.generic.name	   = COM_Localize(9);
	s_main.thirdprson.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_main.thirdprson.generic.callback = Main_MenuEvent;
	s_main.thirdprson.generic.id       = ID_THIRDPRSON;
	s_main.thirdprson.generic.x	       = 320+176+80;
	s_main.thirdprson.generic.y	       = 58;

        s_main.uilanguage.generic.type     = MTYPE_SPINCONTROL;
	s_main.uilanguage.generic.name     = ":"; // globe pic stands here
	s_main.uilanguage.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_main.uilanguage.generic.callback = Main_MenuEvent;
	s_main.uilanguage.generic.id       = ID_LANG;
	s_main.uilanguage.generic.x	   = 284;
	s_main.uilanguage.generic.y	   = 58;
	s_main.uilanguage.itemnames	   = mlang_names;

	Menu_AddItem( &s_main.menu,	&s_main.singleplayer );
	Menu_AddItem( &s_main.menu,	&s_main.multiplayer );
        Menu_AddItem( &s_main.menu,	&s_main.challenges );
	Menu_AddItem( &s_main.menu,	&s_main.setup );
	Menu_AddItem( &s_main.menu,	&s_main.demos );
        Menu_AddItem( &s_main.menu,	&s_main.mods );
	Menu_AddItem( &s_main.menu,	&s_main.exit );
        Menu_AddItem( &s_main.menu,	&s_main.player );
        Menu_AddItem( &s_main.menu,	&s_main.weapplacement );
        Menu_AddItem( &s_main.menu,	&s_main.thirdprson );
        Menu_AddItem( &s_main.menu,	&s_main.uilanguage );
	//Menu_AddItem( &s_main.menu,	&s_main.cinematics );

	trap_Key_SetCatcher( KEYCATCH_UI );
	uis.menusp = 0;
	UI_PushMenu ( &s_main.menu );

}

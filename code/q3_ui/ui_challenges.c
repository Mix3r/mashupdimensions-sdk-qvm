/*
=======================================================================

CONTEST MENU

=======================================================================
*/


#include "ui_local.h"


#define SETUP_MENU_VERTICAL_SPACING		34

#define ART_BACK0		"menu/" MENU_ART_DIR "/back_0"
#define ART_BACK1		"menu/" MENU_ART_DIR "/back_1"

#define ID_CTF		730
#define ID_1CTF	        731
#define ID_BOXTOURNEY	732
#define ID_ELIMINATION	733
#define ID_COOP		734
#define ID_MULTIBALL	735
#define ID_OVERLOAD     736
#define ID_DUEL         737
#define ID_LMS         738
#define ID_BACK		18


typedef struct {
	menuframework_s	menu;

	menutext_s	banner;
	menutext_s	c_ctf;
	menutext_s	c_1ctf;
	menutext_s	c_mball;
	menutext_s	c_boxtourney;
	menutext_s	c_coop;
        menutext_s	c_duel;
	menutext_s	c_overload;
        menutext_s	c_lms;
	menutext_s	c_elimination;
	menubitmap_s	back;
} contests_t;

static contests_t	contests;

/*
===============
UI_SetupMenu_Event
===============
*/
static void UI_Contests_Event( void *ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_CTF:
                UI_SPSkillMenu( "exec ctf.cfg" );
		break;

	case ID_1CTF:
                UI_SPSkillMenu( "exec 1ctf.cfg" );
		break;

	case ID_BOXTOURNEY:
                UI_SPSkillMenu( "exec boxtourney.cfg" );
		break;
        	case ID_DUEL:
                if (ui_is_missionpack) {
                        UI_SPSkillMenu( "exec taduel.cfg" );
                } else {
                        UI_SPSkillMenu( "exec duel.cfg" );
                }
		break;

	case ID_ELIMINATION:
                if (trap_R_RegisterShaderNoMip("levelshots/Q3DM0.jpg")) {
                        UI_SPSkillMenu( "exec q3elmn.cfg" );
                } else if (ui_is_missionpack) {
                        UI_SPSkillMenu( "exec taelmn.cfg" );
                } else {
                        UI_SPSkillMenu( "exec elmn.cfg" );
                }
		break;

	case ID_MULTIBALL:
                UI_SPSkillMenu( "exec mball.cfg" );
		break;

	case ID_OVERLOAD:
                UI_SPSkillMenu( "exec overl.cfg" );
		break;
        case ID_LMS:
                UI_SPSkillMenu( "exec lms.cfg" );
		break;

	case ID_COOP:
                if (trap_R_RegisterShaderNoMip("levelshots/Q3DM0.jpg")) {
                        UI_SPSkillMenu( "exec coop_q3dm0.cfg" );
                } else if (ui_is_missionpack) {
                        UI_SPSkillMenu( "exec ta1_mpteam1.cfg" );
                } else {
                        UI_SPSkillMenu( "exec coop_intro.cfg" );
                }
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


static void contests_menu_draw( void ) {
	trap_R_DrawStretchPic( 0.0, 0.0, uis.glconfig.vidWidth, uis.glconfig.vidHeight, 0, 0, 1, 1, trap_R_RegisterShaderNoMip("textures/sfx/logo512_games") );
        UI_DrawString( 320, 58, COM_Localize(311), UI_CENTER|UI_SMALLFONT, colorWhite );
        UI_DrawBlackHole( &contests.back.generic.x, &contests.back.generic.y );
	// standard menu drawing
	Menu_Draw( &contests.menu );
}

static void UI_Contests_Init( void ) {
	int				y;

	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );

	memset( &contests, 0, sizeof(contests) );

	contests.menu.wrapAround = qtrue;
	contests.menu.fullscreen = qtrue;
        contests.menu.draw = contests_menu_draw;

	contests.banner.generic.type				= MTYPE_BTEXT;
	contests.banner.generic.x					= 320;
	contests.banner.generic.y					= 22;
	contests.banner.string						= COM_Localize(3);
	contests.banner.color						= color_white;
	contests.banner.style						= UI_CENTER;

	y = 100;
	contests.c_ctf.generic.type			= MTYPE_PTEXT;
	contests.c_ctf.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_ctf.generic.x				= 320;
	contests.c_ctf.generic.y				= y;
	contests.c_ctf.generic.id			= ID_CTF;
	contests.c_ctf.generic.callback		= UI_Contests_Event;
	contests.c_ctf.string				= COM_Localize(118);
	contests.c_ctf.color					= color_white;
	contests.c_ctf.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_coop.generic.type			= MTYPE_PTEXT;
	contests.c_coop.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_coop.generic.x				= 320;
	contests.c_coop.generic.y				= y;
	contests.c_coop.generic.id			= ID_COOP;
	contests.c_coop.generic.callback		= UI_Contests_Event;
	contests.c_coop.string				= COM_Localize(124);
	contests.c_coop.color					= color_white;
	contests.c_coop.style					= UI_CENTER;
	y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_1ctf.generic.type			= MTYPE_PTEXT;
	contests.c_1ctf.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_1ctf.generic.x				= 320;
	contests.c_1ctf.generic.y				= y;
	contests.c_1ctf.generic.id			= ID_1CTF;
	contests.c_1ctf.generic.callback		= UI_Contests_Event;
	contests.c_1ctf.string				= COM_Localize(119);
	contests.c_1ctf.color					= color_white;
	contests.c_1ctf.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_mball.generic.type			= MTYPE_PTEXT;
	contests.c_mball.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_mball.generic.x				= 320;
	contests.c_mball.generic.y				= y;
	contests.c_mball.generic.id			= ID_MULTIBALL;
	contests.c_mball.generic.callback		= UI_Contests_Event;
        if (ui_is_missionpack) {
                contests.c_mball.string				= COM_Localize(321);
        } else {
	        contests.c_mball.string				= COM_Localize(120);
        }
	contests.c_mball.color					= color_white;
	contests.c_mball.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_overload.generic.type			= MTYPE_PTEXT;
	contests.c_overload.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_overload.generic.x				= 320;
	contests.c_overload.generic.y				= y;
	contests.c_overload.generic.id			= ID_OVERLOAD;
	contests.c_overload.generic.callback		= UI_Contests_Event;
        if (ui_is_missionpack) {
                contests.c_overload.string				= COM_Localize(320);
        } else {
	        contests.c_overload.string				= COM_Localize(121);
        }
	contests.c_overload.color					= color_white;
	contests.c_overload.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_duel.generic.type			= MTYPE_PTEXT;
	contests.c_duel.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_duel.generic.x				= 320;
	contests.c_duel.generic.y				= y;
	contests.c_duel.generic.id			= ID_DUEL;
	contests.c_duel.generic.callback		= UI_Contests_Event;
	contests.c_duel.string				= COM_Localize(122);
	contests.c_duel.color					= color_white;
	contests.c_duel.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_boxtourney.generic.type			= MTYPE_PTEXT;
	contests.c_boxtourney.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_boxtourney.generic.x				= 320;
	contests.c_boxtourney.generic.y				= y;
	contests.c_boxtourney.generic.id			= ID_BOXTOURNEY;
	contests.c_boxtourney.generic.callback		= UI_Contests_Event;
	contests.c_boxtourney.string				= COM_Localize(123);
	contests.c_boxtourney.color					= color_white;
	contests.c_boxtourney.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_elimination.generic.type			= MTYPE_PTEXT;
	contests.c_elimination.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_elimination.generic.x				= 320;
	contests.c_elimination.generic.y				= y;
	contests.c_elimination.generic.id			= ID_ELIMINATION;
	contests.c_elimination.generic.callback		= UI_Contests_Event;
	contests.c_elimination.string			= COM_Localize(125);
	contests.c_elimination.color					= color_white;
	contests.c_elimination.style					= UI_CENTER;
        y += SETUP_MENU_VERTICAL_SPACING;
        contests.c_lms.generic.type			= MTYPE_PTEXT;
	contests.c_lms.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.c_lms.generic.x				= 320;
	contests.c_lms.generic.y				= y;
	contests.c_lms.generic.id			= ID_LMS;
	contests.c_lms.generic.callback		= UI_Contests_Event;
	contests.c_lms.string				= COM_Localize(126);
	contests.c_lms.color				= color_white;
	contests.c_lms.style				= UI_CENTER;
        ////////////////////////////////////////////////////////////////////////////////////////////

	contests.back.generic.type					= MTYPE_BITMAP;
	contests.back.generic.name					= ART_BACK0;
	contests.back.generic.flags				= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	contests.back.generic.id					= ID_BACK;
	contests.back.generic.callback				= UI_Contests_Event;
	contests.back.generic.x					= 0;
	contests.back.generic.y					= 480-64;
	contests.back.width						= 128;
	contests.back.height						= 64;
	contests.back.focuspic						= ART_BACK1;


        Menu_AddItem( &contests.menu, &contests.banner );
	Menu_AddItem( &contests.menu, &contests.c_ctf );
        Menu_AddItem( &contests.menu, &contests.c_coop );
        Menu_AddItem( &contests.menu, &contests.c_1ctf );
        Menu_AddItem( &contests.menu, &contests.c_mball );
        Menu_AddItem( &contests.menu, &contests.c_overload );
        Menu_AddItem( &contests.menu, &contests.c_duel );
        Menu_AddItem( &contests.menu, &contests.c_boxtourney );
        Menu_AddItem( &contests.menu, &contests.c_elimination );
        Menu_AddItem( &contests.menu, &contests.c_lms );
	Menu_AddItem( &contests.menu, &contests.back );
}

/*
===============
UI_SetupMenu
===============
*/
void UI_ContestsMenu( void ) {
	UI_Contests_Init();
	UI_PushMenu( &contests.menu );
}

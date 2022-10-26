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
// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

#include "cg_local.h"

#ifdef MISSIONPACK
#include "../ui/ui_shared.h"

// used for scoreboard
extern displayContextDef_t cgDC;
menuDef_t *menuScoreboard = NULL;
#else
int drawTeamOverlayModificationCount = -1;
#endif

int sortedTeamPlayers[TEAM_MAXOVERLAY];
int numSortedTeamPlayers;

char systemChat[256];
char teamChat1[256];
char teamChat2[256];

#ifdef MISSIONPACK

int CG_Text_Width(const char *text, float scale, int limit) {
	int count, len;
	float out;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale > cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	out = 0;
	if (text) {
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if (Q_IsColorString(s)) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[*s & 255];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}
	return out * useScale;
}

int CG_Text_Height(const char *text, float scale, int limit) {
	int len, count;
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale > cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	max = 0;
	if (text) {
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if (Q_IsColorString(s)) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[*s & 255];
				if (max < glyph->height) {
					max = glyph->height;
				}
				s++;
				count++;
			}
		}
	}
	return max * useScale;
}

void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader) {
	float w, h;
	w = width * scale;
	h = height * scale;
	CG_AdjustFrom640(&x, &y, &w, &h);
	trap_R_DrawStretchPic(x, y, w, h, s, t, s2, t2, hShader);
}

void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;
	float useScale;
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale > cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	if (text) {
		const char *s = text;
		trap_R_SetColor(color);
		memcpy(&newColor[0], &color[0], sizeof (vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[*s & 255];
			if (Q_IsColorString(s)) {
				memcpy(newColor, g_color_table[ColorIndex(*(s + 1))], sizeof ( newColor));
				newColor[3] = color[3];
				trap_R_SetColor(newColor);
				s += 2;
				continue;
			} else {
				float yadj = useScale * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor(colorBlack);
					CG_Text_PaintChar(x + ofs, y - yadj + ofs,
							glyph->imageWidth,
							glyph->imageHeight,
							useScale,
							glyph->s,
							glyph->t,
							glyph->s2,
							glyph->t2,
							glyph->glyph);
					colorBlack[3] = 1.0;
					trap_R_SetColor(newColor);
				}
				CG_Text_PaintChar(x, y - yadj,
						glyph->imageWidth,
						glyph->imageHeight,
						useScale,
						glyph->s,
						glyph->t,
						glyph->s2,
						glyph->t2,
						glyph->glyph);
				// CG_DrawPic(x, y - yadj, scale * cgDC.Assets.textFont.glyphs[text[i]].imageWidth, scale * cgDC.Assets.textFont.glyphs[text[i]].imageHeight, cgDC.Assets.textFont.glyphs[text[i]].glyph);
				x += (glyph->xSkip * useScale) + adjust;
				s++;
				count++;
			}
		}
		trap_R_SetColor(NULL);
	}
}


#endif

/*
==============
CG_DrawField

Draws large numbers for status bar and powerups
==============
 */
#ifndef MISSIONPACK

static void CG_DrawField(int x, int y, int width, int value) {
	char num[16], *ptr;
	int l;
	int frame;

	if (width < 1) {
		return;
	}

	// draw number string
	if (width > 5) {
		width = 5;
	}

	switch (width) {
		case 1:
			value = value > 9 ? 9 : value;
			value = value < 0 ? 0 : value;
			break;
		case 2:
			value = value > 99 ? 99 : value;
			value = value < -9 ? -9 : value;
			break;
		case 3:
			value = value > 999 ? 999 : value;
			value = value < -99 ? -99 : value;
			break;
		case 4:
			value = value > 9999 ? 9999 : value;
			value = value < -999 ? -999 : value;
			break;
	}

	Com_sprintf(num, sizeof (num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;
	x += 2 + CHAR_WIDTH * (width - l);

	ptr = num;
	while (*ptr && l) {
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr - '0';

		CG_DrawPic(x, y, CHAR_WIDTH, CHAR_HEIGHT, cgs.media.numberShaders[frame]);
		x += CHAR_WIDTH;
		ptr++;
		l--;
	}
}
#endif // MISSIONPACK

/*
================
CG_Draw3DModel

================
 */
void CG_Draw3DModel(float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles) {
	refdef_t refdef;
	refEntity_t ent;

	if (!cg_draw3dIcons.integer || !cg_drawIcons.integer) {
		return;
	}

	CG_AdjustFrom640(&x, &y, &w, &h);

	memset(&refdef, 0, sizeof ( refdef));

	memset(&ent, 0, sizeof ( ent));
	AnglesToAxis(angles, ent.axis);
	VectorCopy(origin, ent.origin);
	ent.hModel = model;
	ent.customSkin = skin;
	ent.renderfx = RF_NOSHADOW; // no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear(refdef.viewaxis);

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene(&ent);
	trap_R_RenderScene(&refdef);
}

/*
================
CG_DrawHead

Used for both the status bar and the scoreboard
================
 */
void CG_DrawHead(float x, float y, float w, float h, int clientNum) {
	clientInfo_t *ci;

	ci = &cgs.clientinfo[ clientNum ];

	if (cg_drawIcons.integer) {
		CG_DrawPic(x+w*0.25, y+w*0.25, w*0.5, h*0.5, ci->modelIcon);
	}

	// if they are deferred, draw a cross out
	if (ci->deferred) {
		CG_DrawPic(x+w*0.25, y+w*0.25, w*0.5, h*0.5, cgs.media.deferShader);
	}
}

/*
================
CG_DrawFlagModel

Used for both the status bar and the scoreboard
================
 */
void CG_DrawFlagModel(float x, float y, float w, float h, int team, qboolean force2D) {
	//qhandle_t cm;
	//float len;
	//vec3_t origin, angles;
	//vec3_t mins, maxs;
        gitem_t *item;
        //qhandle_t handle;


	//if (!force2D && cg_draw3dIcons.integer) {

	//} else if (cg_drawIcons.integer) {
                //if (force2D) {
                //        if (team == TEAM_RED) {
                //                handle = cgs.media.blueCubeIcon;
                //        } else {
                //                handle = cgs.media.redCubeIcon;
                //        }
                //        CG_DrawPic(x+w*0.25, y+w*0.25, w*0.5, h*0.5, handle);
                //        return;
                //}

		if (team == TEAM_RED) {
			item = BG_FindItemForPowerup(PW_REDFLAG);
		} else if (team == TEAM_BLUE) {
			item = BG_FindItemForPowerup(PW_BLUEFLAG);
		} else if (team == TEAM_FREE) {
			item = BG_FindItemForPowerup(PW_NEUTRALFLAG);
		} else {
			return;
		}
		if (item) {
			CG_DrawPic(x, y, w, h, cg_items[ ITEM_INDEX(item) ].icon);
		}
	//}
}

/*
================
CG_DrawStatusBarHead

================
 */
#ifndef MISSIONPACK

static void CG_DrawStatusBarHead(float x) {
	float size, stretch, frac;

	if (cg.damageTime && cg.time - cg.damageTime < DAMAGE_TIME) {
		frac = (float) (cg.time - cg.damageTime) / DAMAGE_TIME;
		size = ICON_SIZE * 1.25 * (1.5 - frac * 0.5);

		stretch = size - ICON_SIZE * 1.25;
		// kick in the direction of damage
		x -= stretch * 0.5 + cg.damageX * stretch * 0.5;

		//cg.headStartYaw = 180 + cg.damageX * 45;

		//cg.headEndYaw = 180 + 20 * cos(crandom() * M_PI);
		//cg.headEndPitch = 5 * cos(crandom() * M_PI);

		//cg.headStartTime = cg.time;
		//cg.headEndTime = cg.time + 100 + random() * 2000;
	} else {
		//if (cg.time >= cg.headEndTime) {
			// select a new head angle
			//cg.headStartYaw = cg.headEndYaw;
			//cg.headStartPitch = cg.headEndPitch;
			//cg.headStartTime = cg.headEndTime;
			//cg.headEndTime = cg.time + 100 + random() * 2000;

			//cg.headEndYaw = 180 + 20 * cos(crandom() * M_PI);
			//cg.headEndPitch = 5 * cos(crandom() * M_PI);
		//}

		size = ICON_SIZE * 1.25;
	}

	// if the server was frozen for a while we may have a bad head start time
	//if (cg.headStartTime > cg.time) {
	//	cg.headStartTime = cg.time;
	//}

	//frac = (cg.time - cg.headStartTime) / (float) (cg.headEndTime - cg.headStartTime);
	//frac = frac * frac * (3 - 2 * frac);
	//angles[YAW] = cg.headStartYaw + (cg.headEndYaw - cg.headStartYaw) * frac;
	//angles[PITCH] = cg.headStartPitch + (cg.headEndPitch - cg.headStartPitch) * frac;

	CG_DrawHead(x-size*0.5, 480 - size + 7, size, size, cg.snap->ps.clientNum);
}
#endif // MISSIONPACK

static void CG_DrawTokensCount(int toksnum) {
        char *tks;
        tks = va("%i",toksnum);
        if (toksnum > 9) {
                toksnum = 132;
        } else {
                toksnum = 139;
        }
        CG_DrawStringExt( toksnum, 437, tks, colorWhite, qfalse, qtrue, BIGCHAR_WIDTH+2, BIGCHAR_HEIGHT+2, 0 );
}


/*
================
CG_DrawStatusBarFlag

================
 */
#ifndef MISSIONPACK

static void CG_DrawStatusBarFlag(float x, int team) {
        float anm = (sin(cg.time * 0.0198)+1) * 0.125 + 0.25; // Mix3r_Durachok swing from 0.25 to 0.5 back and forth
        CG_DrawFlagModel(x+(ICON_SIZE-ICON_SIZE*anm)*0.5, 420+(ICON_SIZE-ICON_SIZE*anm)*0.5, ICON_SIZE*anm, ICON_SIZE*anm, team, qfalse);
}
#endif // MISSIONPACK

/*
================
CG_DrawTeamBackground

================
 */
void CG_DrawTeamBackground(int x, int y, int w, int h, float alpha, int team) {
	vec4_t hcolor;

	hcolor[3] = alpha;
	if (team == TEAM_RED) {
		hcolor[0] = 0.906;
		hcolor[1] = 0;
		hcolor[2] = 0.2;
	} else if (team == TEAM_BLUE) {
		hcolor[0] = 0;
		hcolor[1] = 0.1098;
		hcolor[2] = 0.6588;
	} else {
                hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 0;
	}
	trap_R_SetColor(hcolor);
	CG_DrawPic(x, y, w, h, cgs.media.teamStatusBar);
	trap_R_SetColor(NULL);
}

/*
================
CG_DrawStatusBar

================
 */
#ifndef MISSIONPACK

static void CG_DrawStatusBar(void) {
	int color;
	centity_t *cent;
	playerState_t *ps;
	int value;
	vec4_t hcolor;
	qhandle_t *handle;

	static float colors[4][4] = {
		//		{ 0.2, 1.0, 0.2, 1.0 } , { 1.0, 0.2, 0.2, 1.0 }, {0.5, 0.5, 0.5, 1} };
		{ 1.0f, 0.69f, 0.0f, 1.0f}, // normal
		{ 1.0f, 0.2f, 0.2f, 1.0f}, // low health
		{ 0.5f, 0.5f, 0.5f, 1.0f}, // weapon firing
		{ 1.0f, 1.0f, 1.0f, 1.0f}
	}; // health > 100

	if (cg_drawStatus.integer == 0) {
		return;
	}

	// draw the team background
	if (!(cg.snap->ps.pm_flags & PMF_FOLLOW)) //If not following anybody:
		CG_DrawTeamBackground(160, 442, 320, 30, 0.5f, cg.snap->ps.persistant[PERS_TEAM]);
	else //Sago: If we follow find the teamcolor of the guy we follow. It might not be our own team!
		CG_DrawTeamBackground(160, 442, 320, 30, 0.5f, cgs.clientinfo[ cg.snap->ps.clientNum ].team);

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	// draw any 3D icons first, so the changes back to 2D are minimized
	//if (cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel) {
	//	origin[0] = 70;
	//	origin[1] = 0;
	//	origin[2] = 0;
	//	angles[YAW] = 90 + 20 * sin(cg.time / 1000.0);
	//	CG_Draw3DModel(CHAR_WIDTH * 3 + TEXT_ICON_SPACE, 432, ICON_SIZE, ICON_SIZE,
	//			cg_weapons[ cent->currentState.weapon ].ammoModel, 0, origin, angles);
	//}

	CG_DrawStatusBarHead(320);

	if (cg.predictedPlayerState.powerups[PW_REDFLAG]) {
		//CG_DrawStatusBarFlag(17 + CHAR_WIDTH * 3 + TEXT_ICON_SPACE + ICON_SIZE, TEAM_RED);
                CG_DrawStatusBarFlag(154, TEAM_RED);
	} else if (cg.predictedPlayerState.powerups[PW_BLUEFLAG]) {
		CG_DrawStatusBarFlag(154, TEAM_BLUE);
	} else if (cg.predictedPlayerState.powerups[PW_NEUTRALFLAG]) {
		CG_DrawStatusBarFlag(154, TEAM_FREE);
	}

	//if (cgs.gametype == GT_HARVESTER) {
        //
	//	origin[0] = 90;
	//	origin[1] = 0;
	//	origin[2] = -10;
	//	angles[YAW] = (cg.time & 2047) * 360 / 2048.0;
	//	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
	//		handle = cgs.media.redCubeModel;
	//	} else {
	//		handle = cgs.media.blueCubeModel;
	//	}
	//	CG_Draw3DModel(470 + CHAR_WIDTH * 3 + TEXT_ICON_SPACE, 425, ICON_SIZE, ICON_SIZE, handle, 0, origin, angles);
	//}


	//
	// ammo
	//
	if (cent->currentState.weapon) {
		value = ps->ammo[cent->currentState.weapon];
		if (value > -1) {
			if (cg.predictedPlayerState.weaponstate == WEAPON_FIRING
					&& cg.predictedPlayerState.weaponTime > 100) {
				// draw as dark grey when reloading
				color = 2; // dark grey
			} else {
				if (value >= 0) {
					color = 0; // green
				} else {
					color = 1; // red
				}
			}
			trap_R_SetColor(colors[color]);

			CG_DrawField(0, 432, 3, value);
			trap_R_SetColor(NULL);

			// if we didn't draw a 3D icon, draw a 2D icon for ammo
			if (cg_drawIcons.integer) {
				qhandle_t icon;

				icon = cg_weapons[ cg.predictedPlayerState.weapon ].ammoIcon;
				if (icon) {
				  CG_DrawPic(CHAR_WIDTH * 3 + TEXT_ICON_SPACE + ICON_SIZE*0.25, 432+ICON_SIZE*0.25, ICON_SIZE*0.5, ICON_SIZE*0.5, icon);
				}
			}
		}
	}

	//
	// health
	//
	value = ps->stats[STAT_HEALTH];
	if (value > 100) {
		trap_R_SetColor(colors[3]); // white
	} else if (value > 25) {
		trap_R_SetColor(colors[0]); // green
	} else if (value > 0) {
		color = (cg.time >> 8) & 1; // flash
		trap_R_SetColor(colors[color]);
	} else {
		trap_R_SetColor(colors[1]); // red
	}

	// stretch the health up when taking damage
	CG_DrawField(320-CHAR_WIDTH*4, 432, 3, value);
	CG_ColorForHealth(hcolor);
	trap_R_SetColor(hcolor);

        //
	// armor
        //
	value = ps->stats[STAT_ARMOR];
	if (value > 0) {
		trap_R_SetColor(colors[0]);
                if (value >= 100) {
                   CG_DrawField(317+CHAR_WIDTH, 432, 3, value);
                   handle = &cgs.media.armorIconRed;
                } else if (value >= 10) {
                   CG_DrawField(317, 432, 3, value);
                   handle = &cgs.media.armorIcon;
                } else {
                   CG_DrawField(317-CHAR_WIDTH, 432, 3, value);
                   handle = &cgs.media.armorIconShard;
                }
                if (cg_drawIcons.integer) {
		      CG_DrawPic(440+ICON_SIZE*0.25, 432+ICON_SIZE*0.25, ICON_SIZE*0.5, ICON_SIZE*0.5, *handle);
		}
		trap_R_SetColor(NULL);
	}

	//Skulls!
	if (cgs.gametype == GT_HARVESTER) {
		value = ps->generic1;
		if (value > 0) {
                        CG_DrawTokensCount(value);
			//trap_R_SetColor(colors[0]);
			//CG_DrawField(154, 432 - ICON_SIZE, 3, value);
			//trap_R_SetColor(NULL);
			// if we didn't draw a 3D icon, draw a 2D icon for skull
			if (cg_drawIcons.integer) {
                                hcolor[2] = (sin(cg.time * 0.0198)+1) * 0.125 + 0.25; // Mix3r_Durachok swing from 0.25 to 0.5 back and forth
                                if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
                                        handle = &cgs.media.blueCubeIcon;
                                } else {
                                        handle = &cgs.media.redCubeIcon;
                                }
                                CG_DrawPic(154+(ICON_SIZE-ICON_SIZE*hcolor[2])*0.5, 420+(ICON_SIZE-ICON_SIZE*hcolor[2])*0.5, ICON_SIZE*hcolor[2], ICON_SIZE*hcolor[2], *handle);
			}

		}
	}
}
#endif

/*
===========================================================================================

 UPPER RIGHT CORNER

===========================================================================================
 */

/*
================
CG_DrawAttacker

================
 */
static float CG_DrawAttacker(float y) {
	int t;
	float size;
	const char *info;
	const char *name;
	int clientNum;

	if (cg.predictedPlayerState.stats[STAT_HEALTH] <= 0) {
		return y;
	}

	if (!cg.attackerTime) {
		return y;
	}

	clientNum = cg.predictedPlayerState.persistant[PERS_ATTACKER];
	if (clientNum < 0 || clientNum >= MAX_CLIENTS || clientNum == cg.snap->ps.clientNum) {
		return y;
	}

	if ( !cgs.clientinfo[clientNum].infoValid ) {
		cg.attackerTime = 0;
		return y;
	}

	t = cg.time - cg.attackerTime;
	if (t > ATTACKER_HEAD_TIME) {
		cg.attackerTime = 0;
		return y;
	}

	size = ICON_SIZE * 1.25;

	CG_DrawHead(640 - size, y, size, size, clientNum );

	info = CG_ConfigString(CS_PLAYERS + clientNum);
	name = Info_ValueForKey(info, "n");
	y += size;
	CG_DrawBigString(640 - (Q_PrintStrlen(name) * BIGCHAR_WIDTH), y, name, 0.5);

	return y + BIGCHAR_HEIGHT + 2;
}

/*
================
CG_DrawSpeedMeter

================
 */
static float CG_DrawSpeedMeter(float y) {
	char *s;
	int w;
	vec_t *vel;
	int speed;

	/* speed meter can get in the way of the scoreboard */
	if (cg.scoreBoardShowing) {
		return y;
	}

	vel = cg.snap->ps.velocity;
	/* ignore vertical component of velocity */
	speed = sqrt(vel[0] * vel[0] + vel[1] * vel[1]);

	s = va("%iu/s", speed);

	w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;

	if (cg_drawSpeed.integer == 1) {
		/* top left-hand corner of screen */
		CG_DrawBigString(635 - w, y + 2, s, 1.0F);
		return y + BIGCHAR_HEIGHT + 4;
	} else {
		/* center of screen */
		CG_DrawBigString(320 - w / 2, 300, s, 1.0F);
		return y;
	}
}

/*
==================
CG_DrawSnapshot
==================
 */
static float CG_DrawSnapshot(float y) {
	char *s;
	int w;

	s = va("time:%i snap:%i cmd:%i", cg.snap->serverTime,
			cg.latestSnapshotNum, cgs.serverCommandSequence);
	w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;

	CG_DrawBigString(635 - w, y + 2, s, 1.0F);

	return y + BIGCHAR_HEIGHT + 4;
}

/*
==================
CG_DrawFPS
==================
 */
#define FPS_FRAMES 4

static float CG_DrawFPS(float y) {
	char *s;
	int w;
	static int previousTimes[FPS_FRAMES];
	static int index;
	int i, total;
	int fps;
	static int previous;
	int t, frameTime;

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = trap_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if (index > FPS_FRAMES) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for (i = 0; i < FPS_FRAMES; i++) {
			total += previousTimes[i];
		}
		if (!total) {
			total = 1;
		}
		fps = 1000 * FPS_FRAMES / total;

		s = va("%ifps", fps);
		w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;

		CG_DrawBigString(635 - w, y + 2, s, 1.0F);
	}

	return y + BIGCHAR_HEIGHT + 4;
}

static float CG_DrawPossessionString(float y) {
	vec4_t *color;
	char *line;
	int timeUntilWon;
	int i;
	int w;
	float distance;

	if (cgs.gametype != GT_POSSESSION) {
		return y;
	}

	line = "Find the flag";
	timeUntilWon = -1;
	if (cgs.fraglimit > 0 && cgs.fraglimit < 1000) {
		timeUntilWon = cgs.fraglimit - cg.snap->ps.persistant[PERS_SCORE];
	}
	color = &colorYellow;

	if (cg.snap->ps.powerups[PW_NEUTRALFLAG]) {
		if (timeUntilWon > 0) {
			line = va("Survive for %i:%02i", timeUntilWon / 60, timeUntilWon % 60);
		} else if (timeUntilWon == 0) {
			line = va("You survived");
			color = &colorGreen;
		} else {
			line = va("Survive!");
		}
	} else {
		for (i = 0; i < MAX_GENTITIES; i++) {
			if (cg_entities[i].currentState.eType == ET_PLAYER && cg_entities[i].currentState.powerups & (1 << PW_NEUTRALFLAG)) {
				if (cg_entities[i].lerpOrigin[0] || cg_entities[i].lerpOrigin[1] || cg_entities[i].lerpOrigin[2]) {
					distance = Distance(cg_entities[i].lerpOrigin, cg.snap->ps.origin);
					line = va("%d0 units to flag carrier", ((int) distance) / 10);
					color = &colorYellow;
				}
			}
		}
	}

	w = CG_DrawStrlen(line) * SMALLCHAR_WIDTH;
	CG_DrawSmallStringColor(635 - w, y + 2, line, *color);

	return y + SMALLCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawTimer
=================
 */
static float CG_DrawTimer(float y) {
	char *s;
	int mins, seconds, tens;
	int msec;

	msec = cg.time - cgs.levelStartTime;

        if ( cgs.timelimit > 0 ) {
                msec = cgs.timelimit * 60 * 1000 - msec;
        }

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	s = va("%i:%i%i", mins, tens, seconds);

	CG_DrawSmallString(162, 457, s, 1.0F);

	return y + BIGCHAR_HEIGHT + 4;
}

/*
CG_DrawDoubleDominationThings
Might be relevant for debugging missionpack.
 */
/*
static float CG_DrawDoubleDominationThings( float y ) {
	char		*s;
	int			w;
	int 		statusA, statusB;
	statusA = cgs.redflag;
	statusB = cgs.blueflag;

	// This is only useful in Developer mode.
	if(!cg_developer.integer) {
		return;
	}
	
	if(statusA == TEAM_NONE) {
		s = va("Point A not spawned");
	} else
	if(statusA == TEAM_FREE) {
		s = va("Point A is not controlled");
	} else
	if(statusA == TEAM_RED) {
		s = va("Point A is controlled by RED");
	} else
	if(statusA == TEAM_BLUE) {
		s = va("Point A is controlled by BLUE");
	} else
		s = va("Point A has an error");
	w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
	CG_DrawSmallString( 635 - w, y + 2, s, 1.0F);
	y+=SMALLCHAR_HEIGHT+4;

	if(statusB == TEAM_NONE) {
		s = va("Point B not spawned");
	} else
	if(statusB == TEAM_FREE) {
		s = va("Point B is not controlled");
	} else
	if(statusB == TEAM_RED) {
		s = va("Point B is controlled by RED");
	} else
	if(statusB == TEAM_BLUE) {
		s = va("Point B is controlled by BLUE");
	} else
		s = va("Point B has an error");
	w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
	CG_DrawSmallString( 635 - w, y + 2, s, 1.0F);

	if( ( ( statusB == statusA ) && ( statusA == TEAM_RED ) ) ||
		( ( statusB == statusA ) && ( statusA == TEAM_BLUE ) ) ) {
		s = va("Capture in: %i",(cgs.timetaken+10*1000-cg.time)/1000+1);
		w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
		y+=SMALLCHAR_HEIGHT+4;
		CG_DrawSmallString( 635 - w, y + 2, s, 1.0F);
	}

	return y + SMALLCHAR_HEIGHT+4;
}
*/
/*
=================
CG_DrawLMSmode
=================
 */

static float CG_DrawLMSmode(float y) {
	char *s;
	int w;

	if (cgs.lms_mode == 0) {
		s = va("LMS: Point/round + OT");
	}
	else if (cgs.lms_mode == 1) {
		s = va("LMS: Point/round - OT");
	}
	else if (cgs.lms_mode == 2) {
		s = va("LMS: Point/kill + OT");
	}
	else if (cgs.lms_mode == 3) {
		s = va("LMS: Point/kill - OT");
	}
	else {
		s = va("LMS: Unknown mode");
	}

	w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
	CG_DrawSmallString(635 - w, y + 2, s, 1.0F);

	return y + SMALLCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawCTFoneway
=================
 */

static float CG_DrawCTFoneway(float y) {
	char *s;
	int w;
	vec4_t color;

	if (cgs.gametype != GT_CTF_ELIMINATION)
		return y;

	memcpy(color, g_color_table[ColorIndex(COLOR_WHITE)], sizeof (color));

	if ((cgs.elimflags & EF_ONEWAY) == 0) {
		return y; //nothing to draw
	}
	else if (cgs.attackingTeam == TEAM_BLUE) {
		memcpy(color, g_color_table[ColorIndex(COLOR_BLUE)], sizeof (color));
		s = va("Blue team on offence");
	}
	else if (cgs.attackingTeam == TEAM_RED) {
		memcpy(color, g_color_table[ColorIndex(COLOR_RED)], sizeof (color));
		s = va("Red team on offence");
	}
	else {
		s = va("Unknown team on offence");
	}


	w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
	CG_DrawSmallStringColor(635 - w, y + 2, s, color);

	return y + SMALLCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawDomStatus
=================
 */

static float CG_DrawDomStatus(float y) {
	int i, w;
	char *s;
	vec4_t color;

	for (i = 0; i < cgs.domination_points_count; i++) {
		switch (cgs.domination_points_status[i]) {
			case TEAM_RED:
				memcpy(color, g_color_table[ColorIndex(COLOR_RED)], sizeof (color));
				break;
			case TEAM_BLUE:
				memcpy(color, g_color_table[ColorIndex(COLOR_BLUE)], sizeof (color));
				break;
			default:
				memcpy(color, g_color_table[ColorIndex(COLOR_WHITE)], sizeof (color));
				break;
		}

		s = va("%s", cgs.domination_points_names[i]);
		w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
		CG_DrawSmallStringColor(635 - w, y + 2, s, color);
		y += SMALLCHAR_HEIGHT + 4;

	}

	return y;
}

/*
=================
CG_DrawEliminationTimer
=================
 */
static float CG_DrawEliminationTimer(float y) {
	char *s;
	int w;
	int mins, seconds, tens, sec;
	int msec;
	vec4_t color;
	const char *st __attribute__ ((unused));
	float scale __attribute__ ((unused));
	int cw __attribute__ ((unused));
	int rst;



	rst = cgs.roundStartTime;

	if (cg.time > rst && !cgs.roundtime) {
		return y;
	}

	//default color is white
	memcpy(color, g_color_table[ColorIndex(COLOR_WHITE)], sizeof (color));

	//msec = cg.time - cgs.levelStartTime;
	if (cg.time > rst) //We are started
	{
		msec = cgs.roundtime * 1000 - (cg.time - rst);
		if (msec <= 30 * 1000 - 1) //<= 30 seconds
			memcpy(color, g_color_table[ColorIndex(COLOR_YELLOW)], sizeof (color));
		if (msec <= 10 * 1000 - 1) //<= 10 seconds
			memcpy(color, g_color_table[ColorIndex(COLOR_RED)], sizeof (color));
		msec += 1000; //120-1 instead of 119-0
	} else {
		//Warmup
		msec = -cg.time + rst;
		memcpy(color, g_color_table[ColorIndex(COLOR_GREEN)], sizeof (color));
		sec = msec / 1000;
		msec += 1000; //5-1 instead of 4-0
		/***
		Lots of stuff
		 ****/
		if (cg.warmup == 0) {
			st = va("%i", sec + 1);
			if (sec != cg.warmupCount) {
				cg.warmupCount = sec;
				switch (sec) {
					case 0:
						trap_S_StartLocalSound(cgs.media.count1Sound, CHAN_ANNOUNCER);
						break;
					case 1:
						trap_S_StartLocalSound(cgs.media.count2Sound, CHAN_ANNOUNCER);
						break;
					case 2:
						trap_S_StartLocalSound(cgs.media.count3Sound, CHAN_ANNOUNCER);
						break;
					default:
						break;
				}
			}
			scale = 0.45f;
			switch (cg.warmupCount) {
				case 0:
					cw = 32;
					scale = 0.54f;
					break;
				case 1:
					cw = 28;
					scale = 0.51f;
					break;
				case 2:
					cw = 24;
					scale = 0.48f;
					break;
				default:
					cw = 20;
					scale = 0.45f;
					break;
			}

#ifdef MISSIONPACK
			//w = CG_Text_Width(s, scale, 0);
			//CG_Text_Paint(320 - w / 2, 125, scale, colorWhite, st, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
#else

			w = CG_DrawStrlen(st);
                        color[0] = 0.906;
		        color[1] = 0;
		        color[2] = 0.2;
                        color[3] = 0.91;
                        trap_R_SetColor(color);
                        CG_DrawPic(320 - w * cw * 1.2 / 2, 68+32, w * cw * 1.2, 32, cgs.media.teamStatusBar);
                        trap_R_SetColor(NULL);
			CG_DrawStringExt(320 - w * cw / 2, 70+32, st, colorWhite,
					qfalse, qtrue, cw, (int) (cw * 1.5), 0);
#endif
		}
		/*
		Lots of stuff
		 */
	}

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	if (msec >= 0)
		s = va(" %i:%i%i", mins, tens, seconds);
	else
		s = va(" Overtime");
	w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;

	//CG_DrawBigStringColor(635 - w, y + 2, s, color);
        CG_DrawSmallString(154, 443, s, 1.0F);

	return y + BIGCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawTeamOverlay
=================
 */

static float CG_DrawTeamOverlay(float y, qboolean right, qboolean upper) {
	int x, w, h, xx;
	int i, j, len;
	const char *p;
	vec4_t hcolor;
	int pwidth, lwidth;
	int plyrs;
	char st[16];
	clientInfo_t *ci;
	gitem_t *item;
	int ret_y, count;

	if (!cg_drawTeamOverlay.integer) {
		return y;
	}

	if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_RED && cg.snap->ps.persistant[PERS_TEAM] != TEAM_BLUE) {
		return y; // Not on any team
	}

	plyrs = 0;

	// max player name width
	pwidth = 0;
	count = (numSortedTeamPlayers > 8) ? 8 : numSortedTeamPlayers;
	for (i = 0; i < count; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if (ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {
			plyrs++;
			len = CG_DrawStrlen(ci->name);
			if (len > pwidth)
				pwidth = len;
		}
	}

	if (!plyrs)
		return y;

	if (pwidth > TEAM_OVERLAY_MAXNAME_WIDTH)
		pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;

	// max location name width
	lwidth = 0;
	//for (i = 1; i < MAX_LOCATIONS; i++) {
	//	p = CG_ConfigString(CS_LOCATIONS + i);
	//	if (p && *p) {
	//		len = CG_DrawStrlen(p);
	//		if (len > lwidth)
	//			lwidth = len;
	//	}
	//}

	//if (lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH)
	//	lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;

	w = (pwidth + lwidth + 4 + 7) * TINYCHAR_WIDTH;

	if (right)
		x = 640 - w;
	else
		x = 0;

	h = plyrs * TINYCHAR_HEIGHT;

	if (upper) {
		ret_y = y + h;
	} else {
		y -= h;
		ret_y = y;
	}

	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
		hcolor[0] = 1.0f;
		hcolor[1] = 0.0f;
		hcolor[2] = 0.0f;
		hcolor[3] = 0.33f;
	} else { // if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
		hcolor[0] = 0.0f;
		hcolor[1] = 0.0f;
		hcolor[2] = 1.0f;
		hcolor[3] = 0.33f;
	}
	trap_R_SetColor(hcolor);
	CG_DrawPic(x, y, w, h, cgs.media.teamStatusBar);
	trap_R_SetColor(NULL);

	for (i = 0; i < count; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if (ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {

			hcolor[0] = hcolor[1] = hcolor[2] = hcolor[3] = 1.0;

			xx = x + TINYCHAR_WIDTH;

			CG_DrawStringExt(xx, y,
					ci->name, hcolor, qfalse, qfalse,
					TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH);

			//if (lwidth) {
			//	p = CG_ConfigString(CS_LOCATIONS + ci->location);
			//	if (!p || !*p)
			//		p = "unknown";
			//	len = CG_DrawStrlen(p);
			//	if (len > lwidth)
			//		len = lwidth;
                        //
			//	//				xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth + 
			//	//					((lwidth/2 - len/2) * TINYCHAR_WIDTH);
			//	xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth;
			//	CG_DrawStringExt(xx, y,
			//			p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
			//			TEAM_OVERLAY_MAXLOCATION_WIDTH);
			//}

			CG_GetColorForHealth(ci->health, ci->armor, hcolor);

			Com_sprintf(st, sizeof (st), "%3i %3i", ci->health, ci->armor);

			xx = x + TINYCHAR_WIDTH * 3 +
					TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;

			CG_DrawStringExt(xx, y,
					st, hcolor, qfalse, qfalse,
					TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);

			// draw weapon icon
			xx += TINYCHAR_WIDTH * 3;

			if (cg_weapons[ci->curWeapon].weaponIcon) {
				CG_DrawPic(xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
						cg_weapons[ci->curWeapon].weaponIcon);
			} else {
				CG_DrawPic(xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
						cgs.media.deferShader);
			}

			// Draw powerup icons
			if (right) {
				xx = x;
			} else {
				xx = x + w - TINYCHAR_WIDTH;
			}
			for (j = 0; j <= PW_NUM_POWERUPS; j++) {
				if (ci->powerups & (1 << j)) {

					item = BG_FindItemForPowerup(j);

					if (item) {
						CG_DrawPic(xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
								trap_R_RegisterShader(item->icon));
						if (right) {
							xx -= TINYCHAR_WIDTH;
						} else {
							xx += TINYCHAR_WIDTH;
						}
					}
				}
			}

			y += TINYCHAR_HEIGHT;
		}
	}

	return ret_y;
	//#endif
}

static float CG_DrawFollowMessage(float y) {
	char *s;
	//int w;

	if (!(cg.snap->ps.pm_flags & PMF_FOLLOW) || ((cgs.elimflags & EF_NO_FREESPEC) && CG_IsARoundBasedGametype(cgs.gametype) && CG_IsATeamGametype(cgs.gametype))) {
		return y;
	}

	s = COM_Localize(71);
	//w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
	//CG_DrawSmallString(635 - w, y + 2, s, 1.0F);
        CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(s)*0.5*10, 25, s, colorWhite,0, 0, 10, 10, 0 );
	return y + SMALLCHAR_HEIGHT + 4;
}

static void CG_DrawFragMsgIcons(fragInfo_t fi, int i) {
	int x_offset;
	int spacing;
#ifdef MISSIONPACK
	float *color;
	int imageWidth = 10;
	int imageHeight = 10;
#else
	vec4_t hcolor;
#endif

#ifdef MISSIONPACK
	color = CG_FadeColor(fi.fragTime, FRAGMSG_FADETIME);
	if (!color) {
		trap_R_SetColor(NULL);
		return;
	}
	trap_R_SetColor(color);

	spacing = 3;

	if (fi.attackerName[0] != '\0') {
		CG_Text_Paint(FRAGMSG_X, FRAGMSG_Y + (i * 16), 0.20, color,fi.attackerName, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
		x_offset = CG_Text_Width(fi.attackerName, 0.20, 0) + spacing;
	} else {
                x_offset = 0;
        }

	CG_DrawPic(FRAGMSG_X + x_offset, FRAGMSG_Y + (i * 16) - imageHeight, imageWidth, imageHeight, fi.causeShader);

	x_offset += imageWidth + spacing;

	CG_Text_Paint(FRAGMSG_X + x_offset, FRAGMSG_Y + (i * 16), 0.20, color, fi.targetName, 0, 0, ITEM_TEXTSTYLE_SHADOWED);

	if (fi.teamFrag) {
		x_offset += CG_Text_Width(fi.targetName, 0.20, 0) + spacing;
		CG_Text_Paint(FRAGMSG_X + x_offset, FRAGMSG_Y + (i * 16), 0.20, color, "(^1TEAMMATE^7)", 0, 0, ITEM_TEXTSTYLE_SHADOWED);
	}
#else

	spacing = 3;
        hcolor[3] = FRAGMSG_X;

	if (fi.attackerName[0] != '\0') {
                x_offset = fi.widths_at[0] * TINYCHAR_WIDTH + spacing;
                if (fi.widths_at[1] > 0) {
                        hcolor[0] = 1.0 - ((cg.time - fi.fragTime) * 0.001);
                        if ( hcolor[0] < 0 ) {
                                hcolor[0] = 0.0;
                        }
                        hcolor[1] = hcolor[2] = 0.0;
                        hcolor[3] = 0.5;
                        trap_R_SetColor(hcolor);
                        hcolor[1] = TINYCHAR_WIDTH + spacing + fi.widths_at[1] * TINYCHAR_WIDTH + spacing;
                        hcolor[3] = 320 - (x_offset+hcolor[1]) * 0.5;
                        hcolor[2] = cg.time - fi.fragTime;
                        if ( hcolor[2] > 2300 ) {
                                hcolor[3] -= ( hcolor[2]-2300 ) * 0.53;
                                if (hcolor[3] < FRAGMSG_X) {
                                        hcolor[3] = FRAGMSG_X;
                                }
                        }
                        CG_DrawPic(hcolor[3]-2-hcolor[0]*15, FRAGMSG_Y + (i * TINYCHAR_HEIGHT)-hcolor[0]*8, (x_offset+hcolor[1])+hcolor[0]*30, TINYCHAR_HEIGHT+hcolor[0]*16, cgs.media.teamStatusBar);

                }
		CG_DrawStringExt(hcolor[3], FRAGMSG_Y + (i * TINYCHAR_HEIGHT), fi.attackerName, colorWhite, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);
	} else {
                x_offset = 0;
        }

        trap_R_SetColor(colorWhite);
	CG_DrawPic(hcolor[3] + x_offset, FRAGMSG_Y + (i * TINYCHAR_HEIGHT), TINYCHAR_WIDTH, TINYCHAR_HEIGHT, fi.causeShader);

	x_offset += TINYCHAR_WIDTH + spacing;

	CG_DrawStringExt(hcolor[3] + x_offset, FRAGMSG_Y + (i * TINYCHAR_HEIGHT), fi.targetName, colorWhite, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);

	if (fi.teamFrag) {
		x_offset += CG_DrawStrlen(fi.targetName) * TINYCHAR_WIDTH + spacing;
		CG_DrawStringExt(hcolor[3] + x_offset,
				FRAGMSG_Y + (i * TINYCHAR_HEIGHT), "(^1TEAMMATE^7)", colorWhite,
				qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);
	}

#endif
	trap_R_SetColor(NULL);
}

static void CG_DrawFragMsgText(char *msg, int i) {
#ifdef MISSIONPACK
	float *color;
#else
	vec4_t hcolor;
#endif

#ifdef MISSIONPACK
	color = CG_FadeColor(cgs.fragMsg[i].fragTime, FRAGMSG_FADETIME);
	if (!color) {
		trap_R_SetColor(NULL);
		return;
	}
	trap_R_SetColor(color);
	CG_Text_Paint(FRAGMSG_X, FRAGMSG_Y + (i * 13), 0.20, color, msg, 0, 0,
			ITEM_TEXTSTYLE_SHADOWED);
#else
	hcolor[0] = hcolor[1] = hcolor[2] = hcolor[3] = 1.0;
	trap_R_SetColor(hcolor);

	CG_DrawStringExt(FRAGMSG_X, FRAGMSG_Y + (i * TINYCHAR_HEIGHT),
			msg, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);
#endif
	trap_R_SetColor(NULL);
}

static void CG_DrawFragMessage(void) {
	int i;
	for (i = FRAGMSG_MAX - 1; i >= 0; i--) {
		if (cgs.fragMsg[i].targetName[0] != '\0') {
			// remove frag message after elapsed time
			if (cg.time > cgs.fragMsg[i].fragTime + FRAGMSG_FADETIME) {
				memset(&cgs.fragMsg[i], 0, sizeof (fragInfo_t));
			}
		}
		if (cgs.fragMsg[i].targetName[0] != '\0') {
			if (cg_obituaryOutput.integer == 2) {
				CG_DrawFragMsgText(cgs.fragMsg[i].message, i);
			} else if (cg_obituaryOutput.integer >= 3) {
				CG_DrawFragMsgIcons(cgs.fragMsg[i], i);
			}
		}
	}
}

/*
=====================
CG_DrawUpperRight

=====================
 */
static void CG_DrawUpperRight(stereoFrame_t stereoFrame) {
	float y;

	y = 0;

	if (CG_IsATeamGametype(cgs.gametype) && cg_drawTeamOverlay.integer == 1) {
		y = CG_DrawTeamOverlay(y, qtrue, qtrue);
	}
	/*if ( cgs.gametype == GT_DOUBLE_D ) {
		y = CG_DrawDoubleDominationThings(y);
	} 
	else*/
	if (cgs.gametype == GT_LMS && cg.showScores) {
		y = CG_DrawLMSmode(y);
	} else
		if (cgs.gametype == GT_CTF_ELIMINATION) {
		y = CG_DrawCTFoneway(y);
	} else
		if (cgs.gametype == GT_DOMINATION) {
		y = CG_DrawDomStatus(y);
	} else if (cgs.gametype == GT_POSSESSION) {
		y = CG_DrawPossessionString(y);
	}

	if (cg_drawSnapshot.integer) {
		y = CG_DrawSnapshot(y);
	}
	if (cg_drawFPS.integer && (stereoFrame == STEREO_CENTER || stereoFrame == STEREO_RIGHT)) {
		y = CG_DrawFPS(y);
	}
	if (CG_IsARoundBasedGametype(cgs.gametype)) {
		y = CG_DrawEliminationTimer(y);
		/*if (cgs.clientinfo[ cg.clientNum ].isDead)
			y = CG_DrawEliminationDeathMessage( y);*/
	}

	y = CG_DrawFollowMessage(y);

	if (cg_drawTimer.integer) {
		y = CG_DrawTimer(y);
	}
	if (cg_drawAttacker.integer) {
		y = CG_DrawAttacker(y);
	}
	if (cg_drawSpeed.integer) {
		y = CG_DrawSpeedMeter(y);
	}

}

/*
===========================================================================================

 LOWER RIGHT CORNER

===========================================================================================
 */

/*
=================
CG_DrawScores

Draw the small two score display
=================
 */
#ifndef MISSIONPACK
static float CG_DrawScores(float y) {
	const char *s;
	int s1, s2, score;
	int x, w;
	int v;
	vec4_t color;
	float y1, blowup;
	gitem_t *item;
	//int statusA, statusB;

	//statusA = cgs.redflag;
	//statusB = cgs.blueflag;

	s1 = cgs.scores1;
	s2 = cgs.scores2;

        v = BIGCHAR_HEIGHT + 8;
	y1 = y -= v;


	// draw from the right side to left
	if (CG_IsATeamGametype(cgs.gametype)) {
                if (cgs.fraglimit > 9999) {
                        CG_DrawMission(&y, &s2);
                        return y1 - 8;
                }

		x = 640;
		s = va("%2i", s2);
		w = CG_DrawDigilen(&s2) * BIGCHAR_WIDTH + 8;
		x -= w;

                blowup = (0.5 - (cg.time - cg.headEndTime) * 0.001);
                if (blowup > 0) {
                        blowup *= 2;
                        w += blowup*110;
                        x -= blowup*55;
                        v += blowup*76;
                        y -= blowup*38;
                }

                color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		color[3] = 0.5f;
                trap_R_SetColor(color);

                CG_DrawPic(x, y - 4, w, v, cgs.media.teamStatusBar);
		if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
                        trap_R_SetColor(colorWhite);
			CG_DrawPic(x, y - 4, w, BIGCHAR_HEIGHT + 8, cgs.media.selectShader);
		}
                CG_DrawStringExt( x + 4, y, s, colorWhite, qfalse, qtrue, v-8, v-8, 0 );

		if (CG_UsesTeamFlags(cgs.gametype) && !CG_UsesTheWhiteFlag(cgs.gametype)) {
		        y1 = y - BIGCHAR_HEIGHT - 8;
		        if (cgs.blueflag >= 0 && cgs.blueflag <= 2) {
		                CG_DrawPic(x, y1 - 4, w, BIGCHAR_HEIGHT + 8, cgs.media.blueFlagShader[cgs.blueflag]);
		        }
                        score = 222;
		}

		if (cgs.gametype == GT_DOUBLE_D) {
			// Display Domination point status

			y1 = y - 32; //BIGCHAR_HEIGHT - 8;
			if (cgs.redflag >= 0 && cgs.redflag <= 3) {
				CG_DrawPic(x, y1 - 4, w, 32, cgs.media.ddPointSkinB[cgs.blueflag]);
			}
		}

		s = va("%2i", s1);
		w = CG_DrawDigilen(&s1) * BIGCHAR_WIDTH + 8;
		x -= w;

                blowup = (0.5 - (cg.time - cg.headStartTime) * 0.001);
                if (blowup > 0) {
                        blowup *= 2;
                        w += blowup*110;
                        x -= blowup*55;
                        v += blowup*76;
                        y -= blowup*38;
                }

		color[0] = 1.0f;
		color[2] = 0.0f;
                trap_R_SetColor(color);

                CG_DrawPic(x, y - 4, w, v, cgs.media.teamStatusBar);
		if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
                        trap_R_SetColor(colorWhite);
			CG_DrawPic(x, y - 4, w, BIGCHAR_HEIGHT + 8, cgs.media.selectShader);
                }
                CG_DrawStringExt( x + 4, y, s, colorWhite, qfalse, qtrue, v-8, v-8, 0 );

		if (score == 222) {
		        y1 = y - BIGCHAR_HEIGHT - 8;
		        if (cgs.redflag >= 0 && cgs.redflag <= 2) {
		                CG_DrawPic(x, y1 - 4, w, BIGCHAR_HEIGHT + 8, cgs.media.redFlagShader[cgs.redflag]);
		        }
		}

                if (cgs.gametype == GT_OBELISK) {
                        y1 = y - BIGCHAR_HEIGHT - 8;
			s = va("%3i%%  %3i%%", cg.redObeliskHealth, cg.blueObeliskHealth);
                        CG_DrawPic(x, y1 - 4, w*2, BIGCHAR_HEIGHT + 8, cgs.media.blueCubeIcon);
			CG_DrawSmallString(x, y - 23, s, 1.0F);
		} else if (cgs.gametype == GT_DOUBLE_D) {
			// Display Domination point status

			y1 = y - 32; //BIGCHAR_HEIGHT - 8;
			if (cgs.redflag >= 0 && cgs.redflag <= 3) {
				CG_DrawPic(x, y1 - 4, w, 32, cgs.media.ddPointSkinA[cgs.redflag]);
			}



			//Time till capture:
			if (((cgs.blueflag == cgs.redflag) && (cgs.redflag == TEAM_RED)) ||
					((cgs.blueflag == cgs.redflag) && (cgs.redflag == TEAM_BLUE))) {
				s = va("%i", (cgs.takeAt - cg.time) / 1000 + 1);
				w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;
				CG_DrawBigString(x + 32 + 8 - w / 2, y - 28, s, 1.0F);
			}
		} else if (cgs.gametype == GT_1FCTF) {
                        switch (cgs.redflag) {
                                case 2:color[0] = 1.0f; color[2] = 0.0f; color[3] = 0.77f; break;
                                case 3:color[0] = 0.0f; color[2] = 1.0f; color[3] = 0.77f; break;
                                case 4:color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 0.77f; break;
                        }
                        if (color[3] == 0.77f) {
                                trap_R_SetColor(color);
                                CG_DrawPic(x+w*0.5+7, y1 - 4 - (BIGCHAR_HEIGHT + 8), w-14, BIGCHAR_HEIGHT + 8, cgs.media.flagShader[0]);
                        }
                }

		if (CG_IsATeamGametype(cgs.gametype) && cgs.gametype != GT_TEAM) {
			v = cgs.capturelimit;
		} else {
			v = cgs.fraglimit;
		}
		if (v) {
			s = va("%2i", v);
			w = CG_DrawDigilen(&v) * BIGCHAR_WIDTH + 8;
			x -= w;
			CG_DrawBigString(x + 4, y, s, 1.0F);
		}

	} else {
		qboolean spectator;

                if (cgs.fraglimit > 9999) {
                        CG_DrawMission(&y, &s2);
                        return y1 - 8;
                }

		x = 640;

		score = cg.snap->ps.persistant[PERS_SCORE];
		spectator = (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR);

                blowup = (0.5 - (cg.time - cg.headYaw) * 0.001)*2;
                if (blowup < 0) {
                        blowup = 0;
                }

                // always show your score in the second box if not in first place
		if (s1 != score) {
			s2 = score;
		}

		if (s2 != SCORE_NOT_PRESENT) {
			s = va("%2i", s2);
			w = CG_DrawDigilen(&s2) * BIGCHAR_WIDTH + 8;
			x -= w;
                        /////

                        v = BIGCHAR_HEIGHT + 8;
                        if (blowup > 0 && s2 == score) {
                                w += blowup*110;
                                x -= blowup*55;
                                v += blowup*76;
                                y -= blowup*38;
                        }

                        color[3] = 0.5f;
			if (!spectator && score == s2 && score != s1) {
				color[0] = 1.0f;
				color[1] = 0.0f;
				color[2] = 0.0f;
				//CG_FillRect(x, y - 4, w, v, color);
				//CG_DrawPic(x, y - 4, w, v, cgs.media.selectShader);
			} else {
				color[0] = 0.0f;
				color[1] = 0.0f;
				color[2] = 0.0f;
				//CG_FillRect(x, y - 4, w, v, color);
			}

                        if (blowup > 0.66) {
                                trap_R_SetColor(colorWhite);
                                CG_DrawPic(x, y - 4, w, v, cgs.media.skullShader );
                        } else {
                                trap_R_SetColor(color);
                                CG_DrawPic(x, y - 4, w, v, cgs.media.teamStatusBar);
                        }
                        trap_R_SetColor(NULL);
                        CG_DrawStringExt( x + 4, y, s, colorWhite, qfalse, qtrue, v-8, v-8, 0 );
		}

		// first place
		if (s1 != SCORE_NOT_PRESENT) {
			s = va("%2i", s1);
			w = CG_DrawDigilen(&s1) * BIGCHAR_WIDTH + 8;
			x -= w;
                        ////////////////////
                        v = BIGCHAR_HEIGHT + 8;
                        if (blowup > 0 && s1 == score) {
                                w += blowup*110;
                                x -= blowup*55;
                                v += blowup*76;
                                y -= blowup*38;
                        }
                        ////////////////////
                        color[3] = 0.5f;
			if (!spectator && score == s1) {
				color[0] = 0.0f;
				color[1] = 1.0f;
				color[2] = 0.0f;
			} else {
				color[0] = 0.0f;
				color[1] = 0.0f;
				color[2] = 0.0f;
			}
                        if (blowup > 0.66) {
                                trap_R_SetColor(colorWhite);
                                CG_DrawPic(x, y - 4, w, v, cgs.media.skullShader );
                        } else {
                                trap_R_SetColor(color);
                                CG_DrawPic(x, y - 4, w, v, cgs.media.teamStatusBar);
                        }
                        trap_R_SetColor(NULL);
			CG_DrawStringExt( x + 4, y, s, colorWhite, qfalse, qtrue, v-8, v-8, 0 );
		}
		if (cgs.fraglimit) {
			s = va("%2i", cgs.fraglimit);
			w = CG_DrawDigilen(&cgs.fraglimit) * BIGCHAR_WIDTH + 8;
			x -= w;
			CG_DrawBigString(x + 4, y, s, 1.0F);
		}
	}

	return y1 - 8;
}
#endif // MISSIONPACK

/*
================
CG_DrawPowerups
================
 */
#ifndef MISSIONPACK

static float CG_DrawPowerups(float y) {
	int sorted[MAX_POWERUPS];
	int sortedTime[MAX_POWERUPS];
	int i, j, k;
	int active;
	playerState_t *ps;
	int t;
	gitem_t *item;
	int x;
	int color;
	float size;
	float f;
	static float colors[2][4] = {
		{ 0.2f, 1.0f, 0.2f, 1.0f},
		{ 1.0f, 0.2f, 0.2f, 1.0f}
	};

	ps = &cg.snap->ps;

	if (ps->stats[STAT_HEALTH] <= 0) {
		return y;
	}

	// sort the list by time remaining
	active = 0;
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (!ps->powerups[ i ]) {
			continue;
		}

		// ZOID--don't draw if the power up has unlimited time
		// This is true of the CTF flags
		if ( ps->powerups[ i ] == INT_MAX ) {
			continue;
		}

		t = ps->powerups[ i ] - cg.time;
		if ( t <= 0 ) {
			continue;
		}

		item = BG_FindItemForPowerup(i);
		if (item && item->giType == IT_PERSISTANT_POWERUP)
			continue; //Don't draw persistant powerups here!

		// insert into the list
		for (j = 0; j < active; j++) {
			if (sortedTime[j] >= t) {
				for (k = active - 1; k >= j; k--) {
					sorted[k + 1] = sorted[k];
					sortedTime[k + 1] = sortedTime[k];
				}
				break;
			}
		}
		sorted[j] = i;
		sortedTime[j] = t;
		active++;
	}

	// draw the icons and timers
	x = 640 - ICON_SIZE - CHAR_WIDTH * 2;
	for (i = 0; i < active; i++) {
		item = BG_FindItemForPowerup(sorted[i]);

		if (item) {

			color = 1;

			y -= ICON_SIZE;

			trap_R_SetColor(colors[color]);
                        if (item->giTag == PW_FLIGHT) {
                                centity_t *cent;
                                // Mix3r_Durachok: airhog has bigger timer (battery)
                                if (!cg.renderingThirdPerson) {
                                    if (!cgs.media.airhog_window) {
                                            cgs.media.airhog_window = trap_R_RegisterShaderNoMip("gfx/2d/airhog_window");
                                    }
                                    t = ((cg.refdefViewAngles[PITCH]+90)/180)*(cgs.glconfig.vidHeight*1.38888*-0.28);
                                    cent = &cg_entities[cg.snap->ps.clientNum];
                                    if (cent->currentState.weapon && cg.predictedPlayerState.weaponstate == WEAPON_FIRING && ps->ammo[cent->currentState.weapon] > 0 && cg.predictedPlayerState.weaponTime < 201) {
                                            color = 10; // shake it when firing big gun
                                    } else {
                                        color = 3;
                                    }
                                    trap_R_DrawStretchPic(sin(cg.time * color) * color, t, cgs.glconfig.vidWidth*0.5, cgs.glconfig.vidHeight*1.38888, 0, 0, 1, 1, cgs.media.airhog_window);
                                    // flipped horisontally
                                    trap_R_DrawStretchPic(cgs.glconfig.vidWidth*0.5+sin(cg.time*color)*color-1, t, cgs.glconfig.vidWidth*0.5+1, cgs.glconfig.vidHeight*1.38888, 1, 0, 0, 1, cgs.media.airhog_window);
                                }
                                CG_DrawField(x, y, 2, sortedTime[ i ] / 10000);
                        } else {
			        CG_DrawField(x, y, 2, sortedTime[ i ] / 1000);
                        }

			t = ps->powerups[ sorted[i] ];
			if (t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME) {
				trap_R_SetColor(NULL);
			} else {
				vec4_t modulate;

				f = (float) (t - cg.time) / POWERUP_BLINK_TIME;
				f -= (int) f;
				modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
				trap_R_SetColor(modulate);
			}

			if (cg.powerupActive == sorted[i] &&
					cg.time - cg.powerupTime < PULSE_TIME) {
				f = 1.0 - (((float) cg.time - cg.powerupTime) / PULSE_TIME);
				size = ICON_SIZE * (1.0 + (PULSE_SCALE - 1.0) * f);
			} else {
				size = ICON_SIZE;
			}

			CG_DrawPic(640 - size, y + ICON_SIZE / 2 - size / 2, size, size, trap_R_RegisterShader(item->icon));
		}
	}
	trap_R_SetColor(NULL);

	return y;
}
#endif // MISSIONPACK

/*
=====================
CG_DrawLowerRight

=====================
 */
#ifndef MISSIONPACK

static void CG_DrawLowerRight(void) {
	float y = 472.0;

	if (CG_IsATeamGametype(cgs.gametype) && cg_drawTeamOverlay.integer == 2) {
		y = CG_DrawTeamOverlay(y, qtrue, qfalse);
	}

	y = CG_DrawScores(y);
	y = CG_DrawPowerups(y);
}
#endif // MISSIONPACK

/*
===================
CG_DrawPickupItem
===================
 */
#ifndef MISSIONPACK

static int CG_DrawPickupItem(int y) {
	int value;
	float *fadeColor;

	if (cg.snap->ps.stats[STAT_HEALTH] <= 0) {
		return y;
	}

	y -= ICON_SIZE;

	value = cg.itemPickup;
	if (value) {
		fadeColor = CG_FadeColor(cg.itemPickupTime, 3000);
		if (fadeColor) {
			CG_RegisterItemVisuals(value);
			trap_R_SetColor(fadeColor);
			CG_DrawPic(8, y, ICON_SIZE*0.9, ICON_SIZE*0.9, cg_items[ value ].icon);
                        CG_DrawStringExt(ICON_SIZE+16,   y + (ICON_SIZE / 2-BIGCHAR_HEIGHT/2) + 13, bg_itemlist[ value ].pickup_name, fadeColor,0, 0, 13, 13, 0 );
			trap_R_SetColor(NULL);
		}
	}

	return y;
}
#endif // MISSIONPACK

/*
=====================
CG_DrawLowerLeft

=====================
 */
#ifndef MISSIONPACK

static void CG_DrawLowerLeft(void) {
	float y;

	y = 480 - ICON_SIZE;

	if (CG_IsATeamGametype(cgs.gametype) && cg_drawTeamOverlay.integer == 3) {
		y = CG_DrawTeamOverlay(y, qfalse, qfalse);
	}


	y = CG_DrawPickupItem(y);
}
#endif // MISSIONPACK


//===========================================================================================

/*
=================
CG_DrawTeamInfo
=================
 */
#ifndef MISSIONPACK

static void CG_DrawTeamInfo(void) {
	int w, h;
	int i, len;
	vec4_t hcolor;
	int chatHeight;

#define CHATLOC_Y 420 // bottom end
#define CHATLOC_X 0

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT)
		chatHeight = cg_teamChatHeight.integer;
	else
		chatHeight = TEAMCHAT_HEIGHT;
	if (chatHeight <= 0)
		return; // disabled

	if (cgs.teamLastChatPos != cgs.teamChatPos) {
		if (cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer) {
			cgs.teamLastChatPos++;
		}

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * TINYCHAR_HEIGHT;

		w = 0;

		for (i = cgs.teamLastChatPos; i < cgs.teamChatPos; i++) {
			len = CG_DrawStrlen(cgs.teamChatMsgs[i % chatHeight]);
			if (len > w)
				w = len;
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

		if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED ) {
			hcolor[0] = 1.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 0.0f;
			hcolor[3] = 0.33f;
		} else if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE ) {
			hcolor[0] = 0.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 1.0f;
			hcolor[3] = 0.33f;
		} else {
			hcolor[0] = 0.0f;
			hcolor[1] = 1.0f;
			hcolor[2] = 0.0f;
			hcolor[3] = 0.33f;
		}

		trap_R_SetColor(hcolor);
		CG_DrawPic(CHATLOC_X, CHATLOC_Y - h, 640, h, cgs.media.teamStatusBar);
		trap_R_SetColor(NULL);

		hcolor[0] = hcolor[1] = hcolor[2] = 1.0f;
		hcolor[3] = 1.0f;

		for (i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i--) {
			CG_DrawStringExt(CHATLOC_X + TINYCHAR_WIDTH,
					CHATLOC_Y - (cgs.teamChatPos - i) * TINYCHAR_HEIGHT,
					cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse, qfalse,
					TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0);
		}
	}
}
#endif // MISSIONPACK

/*
===================
CG_DrawHoldableItem
===================
 */
#ifndef MISSIONPACK

static void CG_DrawHoldableItem(void) {
	int value;
        float blowup;
	value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];
	if (value) {
		CG_RegisterItemVisuals(value);
                blowup = (sin(cg.time * 0.0150)+1)*0.125 + 0.25; // Mix3r_Durachok from 0.25 to 0.5
		CG_DrawPic(318+(ICON_SIZE-ICON_SIZE*blowup)*0.5, 433+(ICON_SIZE-ICON_SIZE*blowup)*0.5, ICON_SIZE*blowup, ICON_SIZE*blowup, cg_items[ value ].icon);
	}

}
#endif // MISSIONPACK

#ifndef MISSIONPACK
/*
===================
CG_DrawPersistantPowerup
===================
 */
#if 1 // sos001208 - DEAD // sago - ALIVE

static void CG_DrawPersistantPowerup(void) {
	int value;
        float blowup;

	value = cg.snap->ps.stats[STAT_PERSISTANT_POWERUP];
	if (value) {
		CG_RegisterItemVisuals(value);
                if (bg_itemlist[ value ].giTag == PW_SCOUT) {
                        blowup = (sin(cg.time * 0.0198)+1)*0.0625 + 0.5; // Mix3r_Durachok from 0.5 to 0.625
                        CG_DrawPic(440+(ICON_SIZE-ICON_SIZE*blowup)*0.5, 432+(ICON_SIZE-ICON_SIZE*blowup)*0.5, ICON_SIZE*blowup, ICON_SIZE*blowup, cg_items[ value ].icon);
                } else {
                        blowup = (sin(cg.time * 0.0198)+1)*0.125 + 0.25; // Mix3r_Durachok from 0.25 to 0.5
                        CG_DrawPic(274+(ICON_SIZE-ICON_SIZE*blowup)*0.5, 433+(ICON_SIZE-ICON_SIZE*blowup)*0.5, ICON_SIZE*blowup, ICON_SIZE*blowup, cg_items[ value ].icon);
                }
	}
}
#endif
#endif // MISSIONPACK

/*
===================
CG_DrawReward
===================
 */
static void CG_DrawReward(void) {
	float *color;
	int i, count;
	float x, y;
	char buf[32];

	if (!cg_drawRewards.integer) {
		return;
	}

	color = CG_FadeColor(cg.rewardTime, REWARD_TIME);
	if (!color) {
		if (cg.rewardStack > 0) {
			for (i = 0; i < cg.rewardStack; i++) {
				cg.rewardSound[i] = cg.rewardSound[i + 1];

				cg.rewardShader[i] = cg.rewardShader[i + 1];
				cg.rewardCount[i] = cg.rewardCount[i + 1];
			}
			cg.rewardTime = cg.time;
			cg.rewardStack--;
			color = CG_FadeColor(cg.rewardTime, REWARD_TIME);

			trap_S_StartLocalSound(cg.rewardSound[0], CHAN_ANNOUNCER);

		} else {
			return;
		}
	}

	trap_R_SetColor(color);

	/*
	count = cg.rewardCount[0]/10;				// number of big rewards to draw

	if (count) {
		y = 4;
		x = 320 - count * ICON_SIZE;
		for ( i = 0 ; i < count ; i++ ) {
			CG_DrawPic( x, y, (ICON_SIZE*2)-4, (ICON_SIZE*2)-4, cg.rewardShader[0] );
			x += (ICON_SIZE*2);
		}
	}

	count = cg.rewardCount[0] - count*10;		// number of small rewards to draw
	 */

	if (cg.rewardCount[0] > 2) {
		y = 56;
		x = 320 - ICON_SIZE / 2;
		CG_DrawPic(x+2, y, ICON_SIZE - 4, ICON_SIZE - 4, cg.rewardShader[0]);
		Com_sprintf(buf, sizeof (buf), "%d", cg.rewardCount[0]);
		x = (SCREEN_WIDTH - SMALLCHAR_WIDTH * CG_DrawStrlen(buf)) / 2;
		CG_DrawStringExt(x, y + ICON_SIZE, buf, color, qfalse, qtrue,
				SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0);
	} else {

		count = cg.rewardCount[0];

		y = 56;
		x = 320 - count * ICON_SIZE / 2;
		for (i = 0; i < count; i++) {
			CG_DrawPic(x+2, y, ICON_SIZE - 4, ICON_SIZE - 4, cg.rewardShader[0]);
			x += ICON_SIZE;
		}
	}
	trap_R_SetColor(NULL);
}


/*
===============================================================================

LAGOMETER

===============================================================================
 */

#define LAG_SAMPLES  128

typedef struct {
	int frameSamples[LAG_SAMPLES];
	int frameCount;
	int snapshotFlags[LAG_SAMPLES];
	int snapshotSamples[LAG_SAMPLES];
	int snapshotCount;
} lagometer_t;

lagometer_t lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
 */
void CG_AddLagometerFrameInfo(void) {
	int offset;

	offset = cg.time - cg.latestSnapshotTime;
	lagometer.frameSamples[ lagometer.frameCount & (LAG_SAMPLES - 1) ] = offset;
	lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
 */
void CG_AddLagometerSnapshotInfo(snapshot_t *snap) {
	// dropped packet
	if (!snap) {
		lagometer.snapshotSamples[ lagometer.snapshotCount & (LAG_SAMPLES - 1) ] = -1;
		lagometer.snapshotCount++;
		return;
	}

	// add this snapshot's info
	lagometer.snapshotSamples[ lagometer.snapshotCount & (LAG_SAMPLES - 1) ] = snap->ping;
	lagometer.snapshotFlags[ lagometer.snapshotCount & (LAG_SAMPLES - 1) ] = snap->snapFlags;
	lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
 */
static void CG_DrawDisconnect(void) {
	float x, y;
	int cmdNum;
	usercmd_t cmd;
	const char *s;
	int w; // bk010215 - FIXME char message[1024];

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd(cmdNum, &cmd);
	if (cmd.serverTime <= cg.snap->ps.commandTime
			|| cmd.serverTime > cg.time) { // special check for map_restart // bk 0102165 - FIXME
		return;
	}

	// also add text in center of screen
	s = "Connection Interrupted"; // bk 010215 - FIXME
	w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;
	CG_DrawBigString(320 - w / 2, 100, s, 1.0F);

	// blink the icon
	if ((cg.time >> 9) & 1) {
		return;
	}

	x = 640 - 48;
	y = 480 - 48;

	CG_DrawPic(x, y, 48, 48, trap_R_RegisterShader("gfx/2d/net.tga"));
}


#define MAX_LAGOMETER_PING 900
#define MAX_LAGOMETER_RANGE 300

/*
==============
CG_DrawLagometer
==============
 */
static void CG_DrawLagometer(void) {
	int a, x, y, i;
	float v;
	float ax, ay, aw, ah, mid, range;
	int color;
	float vscale;

	if (!cg_lagometer.integer || cgs.localServer) {
		CG_DrawDisconnect();
		return;
	}

	//
	// draw the graph
	//
#ifdef MISSIONPACK
	x = 640 - 48;
	y = 480 - 144;
#else
	x = 640 - 48;
	y = 480 - 48;
#endif

	trap_R_SetColor(NULL);
	CG_DrawPic(x, y, 48, 48, cgs.media.lagometerShader);

	ax = x;
	ay = y;
	aw = 48;
	ah = 48;
	CG_AdjustFrom640(&ax, &ay, &aw, &ah);

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for (a = 0; a < aw; a++) {
		i = (lagometer.frameCount - 1 - a) & (LAG_SAMPLES - 1);
		v = lagometer.frameSamples[i];
		v *= vscale;
		if (v > 0) {
			if (color != 1) {
				color = 1;
				trap_R_SetColor(g_color_table[ColorIndex(COLOR_YELLOW)]);
			}
			if (v > range) {
				v = range;
			}
			trap_R_DrawStretchPic(ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader);
		} else if (v < 0) {
			if (color != 2) {
				color = 2;
				trap_R_SetColor(g_color_table[ColorIndex(COLOR_BLUE)]);
			}
			v = -v;
			if (v > range) {
				v = range;
			}
			trap_R_DrawStretchPic(ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader);
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for (a = 0; a < aw; a++) {
		i = (lagometer.snapshotCount - 1 - a) & (LAG_SAMPLES - 1);
		v = lagometer.snapshotSamples[i];
		if (v > 0) {
			if (lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED) {
				if (color != 5) {
					color = 5; // YELLOW for rate delay
					trap_R_SetColor(g_color_table[ColorIndex(COLOR_YELLOW)]);
				}
			} else {
				if (color != 3) {
					color = 3;
					trap_R_SetColor(g_color_table[ColorIndex(COLOR_GREEN)]);
				}
			}
			v = v * vscale;
			if (v > range) {
				v = range;
			}
			trap_R_DrawStretchPic(ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader);
		} else if (v < 0) {
			if (color != 4) {
				color = 4; // RED for dropped snapshots
				trap_R_SetColor(g_color_table[ColorIndex(COLOR_RED)]);
			}
			trap_R_DrawStretchPic(ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader);
		}
	}

	trap_R_SetColor(NULL);

	if (cg_nopredict.integer || cg_synchronousClients.integer) {
		CG_DrawBigString(ax, ay, "snc", 1.0);
	}

	CG_DrawDisconnect();
}



/*
===============================================================================

CENTER PRINTING

===============================================================================
 */

/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
 */
void CG_CenterPrint(const char *str, int y, int charWidth) {
	char *s;

	Q_strncpyz(cg.centerPrint, str, sizeof (cg.centerPrint));

        // Mix3r_Durachok: switch or continue camera view
        //(let's avoid viewpos blinking by keeping 999/1000 on camera switch, seamless switching)
        if (cg.centerPrintLines >= 999) {
                int nStrlen = strlen(str);
                if (nStrlen > 3 && cg.centerPrint[0] == '#' && cg.centerPrint[1] == '_' && (cg.centerPrint[2] == 'c' || cg.centerPrint[2] == 'C')) {
                        cg.centerPrintTime = cg.time + ((cg.centerPrint[3] - '0') * 1000);
                        // reset image sequence counter by using different [message] string length for this camera
                        // hint: add/remove spaces to the message tail to achieve seamless switching between sequences
                        if (cg.centerPrintCharWidth != -nStrlen) {
                                cg.centerPrintCharWidth = -nStrlen;
                                cg.centerPrintY = cg.time;
                        }
                        return;
                }
        }

        cg.centerPrintTime = cg.time;
	cg.centerPrintCharWidth = charWidth;
        cg.centerPrintY = y;

        // count lines for multi-line text draw
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while (*s) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}

/*
===================
CG_DrawCenterString
===================
 */
static void CG_DrawCenterString(void) {
	char *start;
	int l;
	int x, y, w;
#ifdef MISSIONPACK // bk010221 - unused else
	int h;
#endif
	float *color;

	if (!cg.centerPrintTime) {
		return;
	}

	color = CG_FadeColor(cg.centerPrintTime, 1000 * cg_centertime.value);
	if (!color) {
                cg.centerPrintTime = 0;
                cg.centerPrintLines = 0;
                cg.centerPrintCharWidth = 0;
		return;
	}

	trap_R_SetColor(color);

        // Mix3r_Durachok: ingame still pic/sequence/camera feature via target_print entity message
        // Message formats are:
        // #_cN x y z pitch yaw - static camera hanging at x y z, looking at pitch yaw during N+3 seconds (0-9 range allowed only, but see note below!)
        // hud and crosshair are present, pov player forced to 3rd person, MUST be located within player vicinity, otherwise HOM effects happen(!)
        // example: #_c9-33 112 40 0 45  means camera hanging at -33 112 40, vertically centered (pitch 0), 45 angled (yaw 45),
        // the camera view duration is 12 seconds (9 sec + 3 default seconds).
        // NOTE 1: To make longer duration, just send the same message via target_print, which resets the camera duration seamlessly to N+3 sec again

        // to disable hud & crosshair within camera view and allow image sequence playback, use C instead of c:
        // example: #_C9-33 112 40 0 45 is the same as above, but both hud & crosshair aren't on the screen.

        // jpeg image sequence playback description:
        // use the following format:
        // #_CN x y z pitch yaw #clipname - sets view as described above and shows video/clipname_F.jpg, where F is frame counting from 1 since
        // message sent, then increasing every frame by 30 frames per second. Gaps (clipname_32, clipname_33 ..empty range.. clipname_91) are allowed,
        // camera view used within gap :) rules are above
        // example: #_C9-33 112 40 0 45 #soccerball starts video/soccerball_1.jpg and continues up to soccerball_2.jpg _3.jpg etc within camera duration.
        // starts video/soccerball_L.wav file on img sequence start where L is selected game language
        // gender dependent: pov female model makes the code look for soccerballf_L.wav sound and soccerballf_F.jpg sequence. If soccerballf frame missing
        // (clipnamef), then fallback to soccerball (clipname) follows for this particular frame, so you can use this for mixing generic and gender-dependent
        // frames during clip. Sound code can perform soccerballf_L.wav fallback too, so if no soccerballf_L.wav, soccerball_L.wav will be played at clip start
        // language fallback is present too, so even further fallback to soccerball.wav possible, if language-dependent wav is missing.
        // language-dependent image sequence isn't implemented because of game small size conservation (avoiding language signs in sequence images), just
        // gender-dependence allowed at now :)

        // NOTE 2: to stop camera/sequence at any moment, send target_print message containing anything but #_ , space character for example

        // still image show format:
        // #_Nstillimagename shows video/stillimagename.jpg for N+2 seconds, then fades away for 1 second. You can prolong the duration by sending the same
        // message until fade away occur, or switch image by sending #_Nanotherstillimage message
        // example: #_9evildogbarking shows video/evildogbarking.jpg for 11 seconds then fades away for 1 second.

        // Mix3r_Durachok: editor-friendly improvement added now (makes camera entity copying and moving easier, no need to re-adjust x y z values in
        // message field of camera entity after every moving of camera in editor window)

        // Camera x y z may be entity origin-dependent now,
        // so anyone can use xyz instead of manually writing x y z values to message field
        // format: #_C9xyz pitch yaw instead of #_C9xvalue yvalue zvalue pitch yaw
        // for example #_C9xyz -45 90 instead of #_C123 456 789 -45 90

        if (cg.centerPrint[0] == '#' && cg.centerPrint[1] == '_') {
                start = cg.centerPrint+3;
                if (cg.centerPrintLines > 0) {
                        if (cg.centerPrint[2] == 'c') {
                                if (cg.centerPrintLines != 999) {
                                        cg.centerPrintLines = 999;
                                        l = (cg.centerPrint[3] - '0') * 1000;
                                        cg.centerPrintTime = cg.time + l;
                                }
                                return;
                        } else if (cg.centerPrint[2] == 'C') {
                                if (cg.centerPrintLines != 1000) {
                                        cg.centerPrintLines = 1000;
                                        l = (cg.centerPrint[3] - '0') * 1000;
                                        cg.centerPrintCharWidth = -strlen(cg.centerPrint);
                                        cg.centerPrintTime = cg.time + l;
                                        cg.centerPrintY = cg.time;
                                        l = -999; // flag to choose and start clipname sound
                                }
                                start = strchr(start,'#');
                                if (start) {
                                        qhandle_t imgseq_frame = 0;
                                        char media_path[MAX_QPATH];
                                        start++;
                                        if (l == -999) {
                                                sfxHandle_t imgseq_snd = 0;
                                                if (cgs.clientinfo[ cg.predictedPlayerState.clientNum ].gender == GENDER_FEMALE) {
                                                        // pov player has female model, let's find clipnamef_lang, clipnamef
                                                        for( l = 1; l < 3; l++ ) {
                                                                // l is small L, not ONE
                                                                Com_sprintf(media_path, sizeof(media_path), "video/%sf%s.wav", start, COM_Localize(l));
                                                                imgseq_snd = trap_S_RegisterSound(media_path, qfalse);
                                                                if (imgseq_snd) {
                                                                        break;
                                                                }
                                                        }
                                                }
                                                if (!imgseq_snd) {
                                                        // pov player hasn't female model, or it is nevermind for this particular clip, so
                                                        // clipnamef isn't present
                                                        // look for generic sound for the clip, clipname_lang, clipname
                                                        for( l = 1; l < 3; l++ ) {
                                                                // l is small L, not ONE
                                                                Com_sprintf(media_path, sizeof(media_path), "video/%s%s.wav", start, COM_Localize(l));
                                                                imgseq_snd = trap_S_RegisterSound(media_path, qfalse);
                                                                if (imgseq_snd) {
                                                                        break;
                                                                }
                                                        }
                                                }
                                                trap_SendConsoleCommand(va("music %s video/v\n",media_path));
                                        }
                                        l = cg.time - cg.centerPrintY;
                                        l = (int)((l/33.33333)+1.0f);
                                        if (cgs.clientinfo[ cg.predictedPlayerState.clientNum ].gender == GENDER_FEMALE) {
                                                Com_sprintf(media_path, sizeof(media_path), "video/%sf_%i.jpg", start,l);
                                        }
                                        if (!CG_TouchPic(media_path)) {
                                                Com_sprintf(media_path, sizeof(media_path), "video/%s_%i.jpg", start,l);
                                                if (!CG_TouchPic(media_path)) {
                                                        l = 0;
                                                }
                                        }
                                        if (l) {
                                                imgseq_frame = trap_R_RegisterShaderNoMip(media_path);
                                                trap_R_DrawStretchPic(0,0,cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 1, 1, imgseq_frame);
                                        }
                                }
                                return;
                        }
                        l = (cg.centerPrint[2] - '0') * 1000;
                        cg.centerPrintLines = 0;
                        // CG_Printf("choffset: %i \n", l );
                        cg.centerPrintTime = cg.time + l;
                }
                for( l = 1; l < 3; l++ ) {
                        cgs.media.scoreboardScore = trap_R_RegisterShaderNoMip(va("video/%s%s",start,COM_Localize(l)));
                        if (cgs.media.scoreboardScore) {
                                break;
                        }
                }
                trap_R_DrawStretchPic(0,0,cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 1, 1, cgs.media.scoreboardScore);
                if (start[0] == '_') {
                        trap_R_DrawStretchPic( 0,0,cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 1, 1, trap_R_RegisterShaderNoMip( "gfx/misc/mma_storyteller_scratches" ) );
                }
                return;
	}

	start = cg.centerPrint;

	y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

	while (1) {
		char linebuffer[1024];

		for (l = 0; l < 50; l++) {
			if (!start[l] || start[l] == '\n') {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

#ifdef MISSIONPACK
		w = CG_Text_Width(linebuffer, 0.5, 0);
		h = CG_Text_Height(linebuffer, 0.5, 0);
		x = (SCREEN_WIDTH - w) / 2;
		CG_Text_Paint(x, y + h, 0.5, color, linebuffer, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
		y += h + 6;
#else
		w = cg.centerPrintCharWidth * CG_DrawStrlen(linebuffer);

		x = (SCREEN_WIDTH - w) / 2;

		CG_DrawStringExt(x, y, linebuffer, color, qfalse, qtrue,
				cg.centerPrintCharWidth, (int) (cg.centerPrintCharWidth * 1.5), 0);

		y += cg.centerPrintCharWidth * 1.5;
#endif
		while (*start && (*start != '\n')) {
			start++;
		}
		if (!*start) {
			break;
		}
		start++;
	}

	trap_R_SetColor(NULL);
}

/*
=====================
CG_DrawCenterDDString
=====================
 */
static void CG_DrawCenterDDString(void) {
#ifndef MISSIONPACK
	int x, y, w;
	float *color;
	char *line;
	int statusA, statusB;
	int sec;
	static int lastDDSec = -100;


	if (cgs.gametype != GT_DOUBLE_D)
		return;


	statusA = cgs.redflag;
	statusB = cgs.blueflag;

	if (((statusB == statusA) && (statusA == TEAM_RED)) ||
			((statusB == statusA) && (statusA == TEAM_BLUE))) {
	} else {
		lastDDSec = -100;
		return; //No team is dominating
	}

	if (statusA == TEAM_BLUE) {
		line = va("Blue scores in %i", (cgs.takeAt - cg.time) / 1000 + 1);
		color = colorBlue;
	} else if (statusA == TEAM_RED) {
		line = va("Red scores in %i", (cgs.takeAt - cg.time) / 1000 + 1);
		color = colorRed;
	} else {
		lastDDSec = -100;
		return;
	}

	if (cgs.takeAt == 0) {
		return;
	}

	sec = (cgs.takeAt - cg.time) / 1000 + 1;

	if (sec < 0) {
		return;
	}

	if (sec != lastDDSec) {
		//A new number is being displayed... play the sound!
		switch (sec) {
			case 1:
				trap_S_StartLocalSound(cgs.media.count1Sound, CHAN_ANNOUNCER);
				break;
			case 2:
				trap_S_StartLocalSound(cgs.media.count2Sound, CHAN_ANNOUNCER);
				break;
			case 3:
				trap_S_StartLocalSound(cgs.media.count3Sound, CHAN_ANNOUNCER);
				break;
			default:
				if (lastDDSec == -100) {
					trap_S_StartLocalSound(cgs.media.doublerSound, CHAN_ANNOUNCER);
				}
				break;
		}
	}
	lastDDSec = sec;

	y = 100;


	w = cg.centerPrintCharWidth * CG_DrawStrlen(line);

	x = (SCREEN_WIDTH - w) / 2;

	CG_DrawStringExt(x, y, line, color, qfalse, qtrue,
			cg.centerPrintCharWidth, (int) (cg.centerPrintCharWidth * 1.5), 0);

#endif
}


/*
================================================================================

CROSSHAIR

================================================================================
 */

/*




=================
CG_DrawCrosshair
=================
 */

static void CG_DrawZoomReticle(float f) {
           vec4_t color;
           color[3] = 0.75f;

           switch (cg.predictedPlayerState.weapon) {
			case 1:
	                        //color[0] = 0.2;
	                        //color[1] = 0.2;
                         	//color[2] = 0.2;
                                CG_ZoomUp_f();
                                return;
				break;
			case 2:
	                        color[0] = 1;
	                        color[1] = 1;
                         	color[2] = 0;
				break;
			case 3:
	                        color[0] = 1;
	                        color[1] = 0.5;
                         	color[2] = 0;
				break;
			case 4:
	                        color[0] = 0;
	                        color[1] = 0.5;
                         	color[2] = 0;
				break;
			case 5:
	                        color[0] = 1;
	                        color[1] = 0;
                         	color[2] = 0;
				break;
			case 6:
	                        color[0] = 1;
	                        color[1] = 1;
                         	color[2] = 0.8;
				break;
			case 7:
	                        color[0] = 0;
	                        color[1] = 1;
                         	color[2] = 0;
				break;
			case 8:
	                        color[0] = 1;
	                        color[1] = 0;
                         	color[2] = 1;
				break;
			case 9:
	                        color[0] = 0;
	                        color[1] = 0;
                         	color[2] = 1;
				break;
			case 10:
	                        color[0] = 0.2;
	                        color[1] = 0.2;
                         	color[2] = 0.2;
				break;
			default:
	                        color[0] = cg_crosshairColorRed.value;
	                        color[1] = cg_crosshairColorGreen.value;
                         	color[2] = cg_crosshairColorBlue.value;
				break;
		}
	   trap_R_SetColor(color);
           color[2] = cgs.glconfig.vidWidth*(1+f);
           color[3] = cgs.glconfig.vidHeight*(1+f);
           trap_R_DrawStretchPic(0-(color[2]-cgs.glconfig.vidWidth)*0.5,0-(color[3]-cgs.glconfig.vidHeight)*0.5,color[2], color[3], 0, 0, 1, 1, cgs.media.ZoomReticle);
           color[2] = 190*(1+f);
           color[3] = 190*(1+f);
           color[0] = SCREEN_WIDTH*0.5-color[2]*0.5;
	   color[1] = SCREEN_HEIGHT*0.5-color[3]*0.5;
	   CG_AdjustFrom640(&color[0], &color[1], &color[2], &color[3]);
           trap_R_DrawStretchPic(color[0], color[1], color[2], color[3], 0, 0, 1, 1, cgs.media.ZoomReticleCenter);
        trap_R_SetColor( NULL );
}

static void CG_DrawCrosshair(void) {
	float w, h;
	qhandle_t hShader;
	float f = 0;
	float x, y;
	int ca = 0; //only to get rid of the warning(not useful)

	if (!cg_drawCrosshair.integer) {
		return;
	}

	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		return;
	}

	if (cg.renderingThirdPerson) {
		return;
	}

	// set color based on health
	if (cg_crosshairHealth.integer) {
		vec4_t hcolor;

		CG_ColorForHealth(hcolor);
		trap_R_SetColor(hcolor);
	} else {
		vec4_t color;
		color[0] = cg_crosshairColorRed.value;
		color[1] = cg_crosshairColorGreen.value;
		color[2] = cg_crosshairColorBlue.value;
		color[3] = 1.0f;
		trap_R_SetColor(color);
	}

	if (cg_differentCrosshairs.integer == 1) {
		switch (cg.predictedPlayerState.weapon) {
			case 1:
				w = h = cg_ch1size.value;
				ca = cg_ch1.integer;
				break;
			case 2:
				w = h = cg_ch2size.value;
				ca = cg_ch2.integer;
				break;
			case 3:
				w = h = cg_ch3size.value;
				ca = cg_ch3.integer;
				break;
			case 4:
				w = h = cg_ch4size.value;
				ca = cg_ch4.integer;
				break;
			case 5:
				w = h = cg_ch5size.value;
				ca = cg_ch5.integer;
				break;
			case 6:
				w = h = cg_ch6size.value;
				ca = cg_ch6.integer;
				break;
			case 7:
				w = h = cg_ch7size.value;
				ca = cg_ch7.integer;
				break;
			case 8:
				w = h = cg_ch8size.value;
				ca = cg_ch8.integer;
				break;
			case 9:
				w = h = cg_ch9size.value;
				ca = cg_ch9.integer;
				break;
			case 10:
				w = h = cg_ch10size.value;
				ca = cg_ch10.integer;
				break;
			case 11:
				w = h = cg_ch11size.value;
				ca = cg_ch11.integer;
				break;
			case 12:
				w = h = cg_ch12size.value;
				ca = cg_ch12.integer;
				break;
			case 13:
				w = h = cg_ch13size.value;
				ca = cg_ch13.integer;
				break;
			default:
				w = h = cg_crosshairSize.value;
				ca = cg_drawCrosshair.integer;
				break;
		}
	} else {
		w = h = cg_crosshairSize.value;
		ca = cg_drawCrosshair.integer;
	}

	if (cg_crosshairPulse.integer || cg.zoomed) {
		// pulse the size of the crosshair when picking up items
		f = cg.time - cg.itemPickupBlendTime;
		if (f > 0 && f < ITEM_BLOB_TIME) {
			f /= ITEM_BLOB_TIME;
			w *= (1 + f);
			h *= (1 + f);
		} else {
                        f = 0;
                }
	}

	x = cg_crosshairX.integer - wideAdjustX; // leilei - widescreen adjust
        //CG_Printf("choffset: %i \n", wideAdjustX );
	y = cg_crosshairY.integer;
	CG_AdjustFrom640(&x, &y, &w, &h);

	if (ca < 0) {
		ca = 0;
	}
	hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

	if (!hShader)
		hShader = cgs.media.crosshairShader[ ca % 10 ];

	trap_R_DrawStretchPic(x + cg.refdef.x + 0.5 * (cg.refdef.width - w),
			y + cg.refdef.y + 0.5 * (cg.refdef.height - h),
			w, h, 0, 0, 1, 1, hShader);

	trap_R_SetColor( NULL );
        if (cg.zoomed) {
                CG_DrawZoomReticle(f);
        }
}

/*
=================
CG_DrawCrosshair3D
=================
 */
static void CG_DrawCrosshair3D(float *f) {
	float w, h;
	qhandle_t hShader;
	int ca;
	trace_t trace;
	vec3_t endpos;
	float zProj, maxdist;
	char rendererinfos[128];
	refEntity_t ent;

	if (!cg_drawCrosshair.integer) {
		return;
	}

	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		return;
	}

        if (cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
		return;
        }

	w = h = cg_crosshairSize.value;

	// pulse the size of the crosshair when picking up items
	*f = cg.time - cg.itemPickupBlendTime;
	if (*f > 0 && *f < ITEM_BLOB_TIME) {
		*f /= ITEM_BLOB_TIME;
		w *= (1 + *f);
		h *= (1 + *f);
	} else {
                *f = 0;
        }

	ca = cg_drawCrosshair.integer;
	if (ca < 0) {
		ca = 0;
	}
	hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

	if (!hShader)
		hShader = cgs.media.crosshairShader[ ca % 10 ];

	// Use a different method rendering the crosshair so players don't see two of them when
	// focusing their eyes at distant objects with high stereo separation
	// We are going to trace to the next shootable object and place the crosshair in front of it.

	// first get all the important renderer information
	trap_Cvar_VariableStringBuffer("r_zProj", rendererinfos, sizeof (rendererinfos));
	zProj = atof(rendererinfos);

        maxdist = 8192;

	//VectorMA(cg.refdef.vieworg, maxdist, cg.refdef.viewaxis[0], endpos);

	memset(&ent, 0, sizeof (ent));
	ent.reType = RT_SPRITE;
	ent.renderfx = RF_DEPTHHACK | RF_CROSSHAIR;

        VectorCopy(cg.predictedPlayerState.origin, ent.origin );
        ent.origin[2] = ent.origin[2]+cg.predictedPlayerState.viewheight;
        AnglesToAxis(cg.predictedPlayerState.viewangles, ent.axis);
        VectorMA(ent.origin, maxdist, ent.axis[0], endpos);

	CG_Trace(&trace, ent.origin, NULL, NULL, endpos, 0, MASK_SHOT);

	VectorCopy(trace.endpos, ent.origin);
        //VectorCopy(cgs.clientinfo[ cg.predictedPlayerState.clientNum ].headOffset, ent.origin);

	// scale the crosshair so it appears the same size for all distances
	ent.radius = w / 800 * zProj * tan(cg.refdef.fov_x * M_PI / 360.0f) * trace.fraction * maxdist / zProj;
	ent.customShader = hShader;

	trap_R_AddRefEntityToScene(&ent);
}

/*
=================
CG_ScanForCrosshairEntity
=================
 */
static void CG_ScanForCrosshairEntity(void) {
	trace_t trace;
	vec3_t start, end;
	int content;

	VectorCopy(cg.refdef.vieworg, start);
	VectorMA(start, 131072, cg.refdef.viewaxis[0], end);

	CG_Trace(&trace, start, vec3_origin, vec3_origin, end,
			cg.snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_BODY);
	if (trace.entityNum >= MAX_CLIENTS) {
		return;
	}

	// if the player is in fog, don't show it
	content = CG_PointContents(trace.endpos, 0);
	if (content & CONTENTS_FOG) {
		return;
	}

	// if the player is invisible, don't show it
	if (cg_entities[ trace.entityNum ].currentState.powerups & (1 << PW_INVIS)) {
		return;
	}

	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;
}

/*
=====================
CG_DrawCrosshairNames
=====================
 */
static void CG_DrawCrosshairNames(void) {
	float *color;
	char *name;
	float w;

	if (!cg_drawCrosshair.integer) {
		return;
	}
	if (!cg_drawCrosshairNames.integer) {
		return;
	}
	//if (cg.renderingThirdPerson) {
	//	return;
        //	}

	// scan the known entities to see if the crosshair is sighted on one
	CG_ScanForCrosshairEntity();

	// draw the name of the player being looked at
	color = CG_FadeColor(cg.crosshairClientTime, 1000);
	if (!color) {
		trap_R_SetColor(NULL);
		return;
	}
        //if (cgs.clientinfo[ cg.crosshairClientNum ].botSkill > 0 && cgs.clientinfo[ cg.crosshairClientNum ].handicap > 0) {
        //        name = COM_Localize(cgs.clientinfo[ cg.crosshairClientNum ].handicap);
        //} else {
	        name = cgs.clientinfo[ cg.crosshairClientNum ].name;
        //}
#ifdef MISSIONPACK
	color[3] *= 0.5f;
	w = CG_Text_Width(name, 0.3f, 0);
	CG_Text_Paint(320 - w / 2, 190, 0.3f, color, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
#else
	//w = CG_DrawStrlen(name) * BIGCHAR_WIDTH;
	//CG_DrawBigString(320 - w / 2, 170, name, color[3] * 0.5f);
        color[3] *= 0.5f;
        CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(name)*0.5*10, 170, name, color,0, 0, 10, 10, 0 );
#endif
	trap_R_SetColor(NULL);
}


//==============================================================================

/*
=================
CG_DrawSpectator
=================
 */
static void CG_DrawSpectator(void) {
        char *sinfo;
        sinfo = COM_Localize(68);
        //CG_Printf("drawspec: %s \n", sinfo );
        CG_DrawTeamBackground(160, 457, 320, 15, 0.5f, TEAM_FREE);
        CG_DrawStringExt(35+SCREEN_WIDTH*0.5-CG_DrawStrlen(sinfo)*0.5*10, 459, sinfo, colorWhite,0, 0, 10, 10, 0 );
	if (cgs.gametype == GT_TOURNAMENT) {
                sinfo = COM_Localize(69);
                CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(sinfo)*0.5*10, 445, sinfo, colorWhite,0, 0, 10, 10, 0 );
	} else if (CG_IsATeamGametype(cgs.gametype)) {
                sinfo = COM_Localize(70);
                CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(sinfo)*0.5*10, 445, sinfo, colorWhite,0, 0, 10, 10, 0 );
	}
}

/*
=================
CG_DrawVote
=================
 */
static void CG_DrawVote(void) {
        char *s;
	int sec;
        vec4_t VoteColor = {0.0, 0.0, 0.0, 0.5};

	if (!cgs.voteTime) {
		return;
	}

	// play a talk beep whenever it is modified
	if (cgs.voteModified) {
		cgs.voteModified = qfalse;
		trap_SendConsoleCommand("play sound/teamplay/flagreturn_yourteam.wav");
	}

	sec = (VOTE_TIME - (cg.time - cgs.voteTime)) / 1000;
	if (sec < 0) {
		sec = 0;
	}
#ifdef MISSIONPACK
	s = va("VOTE(%i):%s yes:%i no:%i", sec, cgs.voteString, cgs.voteYes, cgs.voteNo);
	CG_DrawSmallString(0, 58, s, 1.0F);
	s = "or press ESC then click Vote";
	CG_DrawSmallString(0, 58 + SMALLCHAR_HEIGHT + 2, s, 1.0F);
#else
        s = COM_Localize(73);
	s = va(s, sec, cgs.voteString, cgs.voteYes, cgs.voteNo);
        trap_R_SetColor(VoteColor);
        CG_DrawPic(0, 57, 640, SMALLCHAR_HEIGHT + 2, cgs.media.teamStatusBar);
        trap_R_SetColor(NULL);
	CG_DrawSmallString(1, 58, s, 1.0F);
#endif
}

/*
=================
CG_DrawTeamVote
=================
 */
static void CG_DrawTeamVote(void) {
	char *s;
	int sec, cs_offset;
        vec4_t VoteColor = {0.0, 0.0, 0.0, 0.5};

	if (cgs.clientinfo[cg.clientNum].team == TEAM_RED)
		cs_offset = 0;
	else if (cgs.clientinfo[cg.clientNum].team == TEAM_BLUE)
		cs_offset = 1;
	else
		return;

	if (!cgs.teamVoteTime[cs_offset]) {
		return;
	}

	// play a talk beep whenever it is modified
	if (cgs.teamVoteModified[cs_offset]) {
		cgs.teamVoteModified[cs_offset] = qfalse;
		trap_SendConsoleCommand("play sound/teamplay/flagreturn_yourteam.wav");
	}

	sec = (VOTE_TIME - (cg.time - cgs.teamVoteTime[cs_offset])) / 1000;
	if (sec < 0) {
		sec = 0;
	}
        s = COM_Localize(74);
	s = va(s, sec, cgs.teamVoteString[cs_offset],
			cgs.teamVoteYes[cs_offset], cgs.teamVoteNo[cs_offset]);
        trap_R_SetColor(VoteColor);
        CG_DrawPic(0, 89, 640, SMALLCHAR_HEIGHT + 2, cgs.media.teamStatusBar);
        trap_R_SetColor(NULL);
	CG_DrawSmallString(1, 90, s, 1.0F);
}

static qboolean CG_DrawScoreboard(void) {
#ifdef MISSIONPACK
	static qboolean firstTime = qtrue;
	float *fadeColor;

	if (menuScoreboard) {
		menuScoreboard->window.flags &= ~WINDOW_FORCED;
	}
	if (cg_paused.integer) {
		cg.deferredPlayerLoading = 0;
		firstTime = qtrue;
		return qfalse;
	}

	// should never happen in Team Arena
	if (cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
		cg.deferredPlayerLoading = 0;
		firstTime = qtrue;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if (cg.warmup && !cg.showScores) {
		return qfalse;
	}

	if (cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor(cg.scoreFadeTime, FADE_TIME);
		if (!fadeColor) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			firstTime = qtrue;
			return qfalse;
		}
	}


	if (menuScoreboard == NULL) {
		if (CG_IsATeamGametype(cgs.gametype)) {
			menuScoreboard = Menus_FindByName("teamscore_menu");
		} else {
			menuScoreboard = Menus_FindByName("score_menu");
		}
	}

	if (menuScoreboard) {
		if (firstTime) {
			CG_SetScoreSelection(menuScoreboard);
			firstTime = qfalse;
		}
		Menu_Paint(menuScoreboard, qtrue);
	}

	// load any models that have been deferred
	if (++cg.deferredPlayerLoading > 10) {
		CG_LoadDeferredPlayers();
	}

	return qtrue;
#else
	char *s;
	int w;
	if (cg.respawnTime && cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR && (!CG_IsARoundBasedGametype(cgs.gametype))) {
		if (cg.respawnTime > cg.time) {
                        s = COM_Localize(79);
			s = va(s, ((double) cg.respawnTime - (double) cg.time) / 1000.0);
			w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(320 - w / 2, 400, s, colorYellow);
		} else {
			s = COM_Localize(78);
			w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(320 - w / 2, 400, s, colorGreen);
		}
	}
	return CG_DrawOldScoreboard();
#endif
}

#define ACCBOARD_XPOS 500
#define ACCBOARD_YPOS 150
#define ACCBOARD_HEIGHT 20
#define ACCBOARD_WIDTH 75
#define ACCITEM_SIZE 16

qboolean CG_DrawAccboard(void) {
	int counter, i;

	i = 0;

	if (!cg.showAcc) {
		return qfalse;
	}
	trap_R_SetColor(colorWhite);

	for (counter = 0; counter < WP_NUM_WEAPONS; counter++) {
		if (cg_weapons[counter + 2].weaponIcon && counter != WP_PROX_LAUNCHER && counter != WP_GRAPPLING_HOOK)
			i++;
	}

	CG_DrawTeamBackground(ACCBOARD_XPOS, ACCBOARD_YPOS, ACCBOARD_WIDTH, ACCBOARD_HEIGHT * (i + 1), 0.33f, TEAM_BLUE);

	i = 0;

	for (counter = 0; counter < WP_NUM_WEAPONS; counter++) {
		if (cg_weapons[counter + 2].weaponIcon && counter != WP_PROX_LAUNCHER && counter != WP_GRAPPLING_HOOK) {
			CG_DrawPic(ACCBOARD_XPOS + 10, ACCBOARD_YPOS + 10 + i*ACCBOARD_HEIGHT, ACCITEM_SIZE, ACCITEM_SIZE, cg_weapons[counter + 2].weaponIcon);
			if (cg.accuracys[counter][0] > 0)
				CG_DrawSmallStringColor(ACCBOARD_XPOS + 10 + ACCITEM_SIZE + 10, ACCBOARD_YPOS + 10 + i * ACCBOARD_HEIGHT + ACCITEM_SIZE / 2 - SMALLCHAR_HEIGHT / 2,
					va("%i%s", (int) (((float) cg.accuracys[counter][1]*100) / ((float) (cg.accuracys[counter][0]))), "%"), colorWhite);
			else
				CG_DrawSmallStringColor(ACCBOARD_XPOS + 10 + ACCITEM_SIZE + 10, ACCBOARD_YPOS + 10 + i * ACCBOARD_HEIGHT + ACCITEM_SIZE / 2 - SMALLCHAR_HEIGHT / 2, "-%", colorWhite);
			i++;
		}
	}

	trap_R_SetColor(NULL);
	return qtrue;
}



/*
=================
CG_DrawIntermission
=================
 */

void trap_Cmd_ExecuteText(int exec_when, const char *text); // leilei - for unlocks only...

static void CG_DrawIntermission(void) {
	//	int key;
        char *s;
        int mins, seconds, tens;
        vec4_t hcolor;

	seconds = cg.intermissionStarted / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	hcolor[0] = 0;
	hcolor[1] = 0.1098;
	hcolor[2] = 0.6588;
        hcolor[3] = 0.86;

        trap_R_SetColor(hcolor);
        CG_DrawPic(45, 434, 585, 30, cgs.media.teamStatusBar);
        trap_R_SetColor(NULL);

        s = va(COM_Localize(96), mins, tens, seconds);
	CG_DrawSmallString(150, 441, s, 1.0F);
#ifndef MISSIONPACK
	if (cgs.gametype == GT_SINGLE_PLAYER) {
		CG_DrawCenterString();
		return;
	}
#endif
	cg.scoreFadeTime = cg.time;
	cg.scoreBoardShowing = CG_DrawScoreboard();
}

/*
=================
CG_DrawFollow
=================
 */
static qboolean CG_DrawFollow(void) {
	float x;
	vec4_t color;
	char *name;

	if (!(cg.snap->ps.pm_flags & PMF_FOLLOW)) {
		return qfalse;
	}
        name = COM_Localize(72);
        name = va(name, cgs.clientinfo[ cg.snap->ps.clientNum ].name);
        CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(name)*0.5*10, 15, name, colorWhite,0, 0, 10, 10, 0 );

	return qtrue;
}

/*
=================
CG_DrawAmmoWarning
=================
 */
static void CG_DrawAmmoWarning(void) {
	const char *s;
	int w;

	//Don't report in instant gib same with RA
	if (cgs.nopickup)
		return;

	if (cg_drawAmmoWarning.integer == 0) {
		return;
	}

	if (!cg.lowAmmoWarning) {
		return;
	}

	if (cg.lowAmmoWarning == 2) {
		// s = "OUT OF AMMO";
	} else {
		s = "LOW AMMO WARNING";
                w = CG_DrawStrlen(s) * BIGCHAR_WIDTH;
	        CG_DrawBigString(320 - w / 2, 64, s, 1.0F);
	}
}


/*
=================
CG_DrawProxWarning
=================
*/
static void CG_DrawProxWarning( void ) {
	char s [32];
	int w;
	static int proxTime;
	int proxTick;

	if( !(cg.snap->ps.eFlags & EF_TICKING ) ) {
		proxTime = 0;
		return;
	}

	if (proxTime == 0) {
		proxTime = cg.time;
	}

	proxTick = 10 - ((cg.time - proxTime) / 1000);

	if (proxTick > 0 && proxTick <= 5) {
		Com_sprintf(s, sizeof(s), "INTERNAL COMBUSTION IN: %i", proxTick);
	} else {
		Com_sprintf(s, sizeof(s), "YOU HAVE BEEN MINED");
	}

	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigStringColor( 320 - w / 2, 64 + BIGCHAR_HEIGHT, s, g_color_table[ColorIndex(COLOR_RED)] );
}

/*
=================
CG_DrawWarmup
=================
 */
static void CG_DrawWarmup(void) {
	int w;
	int sec;
	int i;
	float scale __attribute__ ((unused));
	clientInfo_t *ci1, *ci2;
	int cw __attribute__ ((unused));
	char *s;

	sec = cg.warmup;
	if (!sec) {
		return;
	}

	if (sec < 0) {
		s = COM_Localize(75);
		CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(s)*0.5*10, 45, s, colorWhite,0, 0, 10, 10, 0 );
		cg.warmupCount = 0;
		return;
	}

	if (cgs.gametype == GT_TOURNAMENT) {
		// find the two active players
		ci1 = NULL;
		ci2 = NULL;
		for (i = 0; i < cgs.maxclients; i++) {
			if (cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_FREE) {
				if (!ci1) {
					ci1 = &cgs.clientinfo[i];
				} else {
					ci2 = &cgs.clientinfo[i];
				}
			}
		}

		if (ci1 && ci2) {
                        s = COM_Localize(76);
			s = va(s, ci1->name, ci2->name);
#ifdef MISSIONPACK
			w = CG_Text_Width(s, 0.6f, 0);
			CG_Text_Paint(320 - w / 2, 60, 0.6f, colorWhite, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
#else
                        CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(s)*0.5*10, 45, s, colorWhite,0, 0, 10, 10, 0 );
#endif
		}
	} else {
		if (cgs.gametype == GT_FFA) {
			s = "Против всех";
		} else if (cgs.gametype == GT_TEAM) {
			s = "Командный бой";
		} else if (cgs.gametype == GT_CTF) {
			s = "Захват флага";
		} else if (cgs.gametype == GT_ELIMINATION) {
			s = "Устранение";
		} else if (cgs.gametype == GT_CTF_ELIMINATION) {
			s = "Устранение с флагами";
		} else if (cgs.gametype == GT_LMS) {
			s = "Последний выживший";
		} else if (cgs.gametype == GT_DOUBLE_D) {
			s = "Двойное превосходство";
		} else if (cgs.gametype == GT_1FCTF) {
			s = "Захват с 1-м флагом";
		} else if (cgs.gametype == GT_OBELISK) {
			s = "Самогонщики";
		} else if (cgs.gametype == GT_HARVESTER) {
			s = "Мэшап-мультибол";
		} else if (cgs.gametype == GT_DOMINATION) {
			s = "Превосходство";
		} else if (cgs.gametype == GT_POSSESSION) {
			s = "Обладание";
		} else {
			s = "";
		}
#ifdef MISSIONPACK
		w = CG_Text_Width(s, 0.6f, 0);
		CG_Text_Paint(320 - w / 2, 90, 0.6f, colorWhite, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
#else
		CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(s)*0.5*10, 45, s, colorWhite,0, 0, 10, 10, 0 );
#endif
	}

	sec = (sec - cg.time) / 1000;
	if (sec < 0) {
		cg.warmup = 0;
		sec = 0;
	}
        s = COM_Localize(77);
	s = va(s, sec + 1);
	if (sec != cg.warmupCount) {
		cg.warmupCount = sec;
		switch (sec) {
			case 0:
				trap_S_StartLocalSound(cgs.media.count1Sound, CHAN_ANNOUNCER);
				break;
			case 1:
				trap_S_StartLocalSound(cgs.media.count2Sound, CHAN_ANNOUNCER);
				break;
			case 2:
				trap_S_StartLocalSound(cgs.media.count3Sound, CHAN_ANNOUNCER);
				break;
			default:
				break;
		}
	}
	scale = 0.45f;
	switch (cg.warmupCount) {
		case 0:
			cw = 28;
			scale = 0.54f;
			break;
		case 1:
			cw = 24;
			scale = 0.51f;
			break;
		case 2:
			cw = 20;
			scale = 0.48f;
			break;
		default:
			cw = 16;
			scale = 0.45f;
			break;
	}

#ifdef MISSIONPACK
	w = CG_Text_Width(s, scale, 0);
	CG_Text_Paint(320 - w / 2, 125, scale, colorWhite, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
#else
        CG_DrawStringExt(SCREEN_WIDTH*0.5-CG_DrawStrlen(s)*0.5*10, 55, s, colorWhite,0, 0, 10, 10, 0 );
#endif
}

//==================================================================================
#ifdef MISSIONPACK

/* 
=================
CG_DrawTimedMenus
=================
 */
void CG_DrawTimedMenus(void) {
	if (cg.voiceTime) {
		int t = cg.time - cg.voiceTime;
		if (t > 2500) {
			Menus_CloseByName("voiceMenu");
			trap_Cvar_Set("cl_conXOffset", "0");
			cg.voiceTime = 0;
		}
	}
}
#endif

/*
=================
CG_Draw2D
=================
 */
static void CG_Draw2D(stereoFrame_t stereoFrame) {
#ifdef MISSIONPACK
	if (cgs.orderPending && cg.time > cgs.orderTime) {
		CG_CheckOrderPending();
	}
#endif

	if (cg_draw2D.integer == 0) {
		return;
	}

	if (cg.snap->ps.pm_type == PM_INTERMISSION) {
		CG_DrawIntermission();
		return;
	}

        // Mix3r_Durachok: camera mode special case
        if (cg.centerPrintLines == 1000) {
                CG_DrawCenterString();
                if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
                        CG_DrawSpectator();
                }
                CG_DrawVote();
	        CG_DrawTeamVote();
                return;
        }

#ifndef MISSIONPACK
	CG_DrawLowerRight();
	CG_DrawLowerLeft();
#endif

	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| cg.snap->ps.pm_type == PM_SPECTATOR*/) {
		CG_DrawSpectator();

		if (stereoFrame == STEREO_CENTER)
			CG_DrawCrosshair();

		CG_DrawCrosshairNames();
	} else {
		// don't draw any status if dead or the scoreboard is being explicitly shown
		if (!cg.showScores && cg.snap->ps.stats[STAT_HEALTH] > 0) {

#ifdef MISSIONPACK
			if (cg_drawStatus.integer) {
				Menu_PaintAll();
				CG_DrawTimedMenus();
			}
#else
			CG_DrawStatusBar();
#endif

			CG_DrawAmmoWarning();

			CG_DrawProxWarning();
			if (stereoFrame == STEREO_CENTER)
				CG_DrawCrosshair();
			CG_DrawCrosshairNames();
			CG_DrawWeaponSelect();

#ifndef MISSIONPACK
			CG_DrawHoldableItem();
			CG_DrawPersistantPowerup();
#endif

			CG_DrawReward();
		}
	}
#ifndef MISSIONPACK
	if (CG_IsATeamGametype(cgs.gametype)) {
		CG_DrawTeamInfo();
	}
#endif

	CG_DrawVote();
	CG_DrawTeamVote();

	CG_DrawLagometer();

#ifdef MISSIONPACK
	if (!cg_paused.integer) {
		CG_DrawUpperRight(stereoFrame);
	}
#else
	CG_DrawUpperRight(stereoFrame);
#endif

	CG_DrawFragMessage();

	if (!CG_DrawFollow()) {
		CG_DrawWarmup();
	}

	// don't draw center string if scoreboard is up
	cg.scoreBoardShowing = CG_DrawScoreboard();
	if (!cg.scoreBoardShowing) {
		CG_DrawCenterDDString();
		CG_DrawCenterString();
	}

	cg.accBoardShowing = CG_DrawAccboard();
}

/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
 */
void CG_DrawActive(stereoFrame_t stereoView) {
        float blowback;
	// optionally draw the info screen instead
	if (!cg.snap) {
		// loadingscreen
#ifdef SCRIPTHUD
		menuDef_t *loading = Menus_FindByName("Loading");
		if (loading == NULL)
			//	if(!loading)

#endif
			// end loadingscreen
			CG_DrawInformation();
		return;
	}

	// optionally draw the tournement scoreboard instead
	if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR &&
			(cg.snap->ps.pm_flags & PMF_SCOREBOARD)) {
		CG_DrawTourneyScoreboard();
		return;
	}

	// clear around the rendered view if sized down
	CG_TileClear();

	if (stereoView != STEREO_CENTER || cg.renderingThirdPerson) {
		CG_DrawCrosshair3D(&blowback);
                trap_R_RenderScene(&cg.refdef);
                if (cg.zoomed) {
                   CG_DrawZoomReticle(blowback);
                   //CG_Printf("zoomret: %.2f \n", blowback );
                }
        } else {
                trap_R_RenderScene(&cg.refdef);
        }
	// draw 3D view

	// draw status bar and other floating elements
	CG_Draw2D(stereoView);
}



// loadingscreen
#ifdef SCRIPTHUD

/*
======================
CG_UpdateMediaFraction
======================
 */
void CG_UpdateMediaFraction(float newFract) {
	cg.mediaFraction = newFract;
	trap_UpdateScreen();
}

/*
======================
CG_UpdateSoundFraction
======================
 */
void CG_UpdateSoundFraction(float newFract) {
	cg.soundFraction = newFract;
	trap_UpdateScreen();
}

/*
======================
CG_UpdateGraphicFraction
======================
 */
void CG_UpdateGraphicFraction(float newFract) {
	cg.graphicFraction = newFract;
	trap_UpdateScreen();
}

/*
====================
CG_DrawLoadingScreen
====================
 */
void CG_DrawLoadingScreen(void) {
	menuDef_t *loading = Menus_FindByName("Loading");
	if (loading != NULL) {
		//	if(loading) {
		Menu_Paint(loading, qtrue);
	} else {
		CG_DrawInformation();
	}
}
#endif



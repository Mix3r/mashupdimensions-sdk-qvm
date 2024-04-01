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
// cg_players.c -- handle the media and animation for player entities
#include "cg_local.h"
#define SHADOW_DISTANCE  128

static char *cg_customSoundNames[MAX_CUSTOM_SOUNDS] = {
	"*death1.wav",
	"*death2.wav",
	"*death3.wav",
	"*jump1.wav",
	"*pain25_1.wav",
	"*pain50_1.wav",
	"*pain75_1.wav",
	"*pain100_1.wav",
	"*falling1.wav",
	"*gasp.wav",
	"*drown.wav",
	"*fall1.wav",
	"*taunt.wav"
};

// leilei - eyes hack

vec3_t headpos;
vec3_t headang;

int enableQ;

/*
================
CG_CustomSound

================
 */
sfxHandle_t CG_CustomSound(int clientNum, const char *soundName) {
	clientInfo_t *ci;
	int i;

	if (soundName[0] != '*') {
		return trap_S_RegisterSound(soundName, qfalse);
	}

	if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	for (i = 0; i < MAX_CUSTOM_SOUNDS && cg_customSoundNames[i]; i++) {
		if (strequals(soundName, cg_customSoundNames[i])) {
			return ci->sounds[i];
		}
	}

	CG_Error("Unknown custom sound: %s", soundName);
	return 0;
}



/*
=============================================================================

CLIENT INFO

=============================================================================
 */

/*
======================
CG_ParseAnimationFile

Read a configuration file containing animation counts and rates
models/players/visor/animation.cfg, etc
Mix3r: also reads head gender to override head voice (if genderonly qtrue just reads sex from anim file)
======================
 */
static qboolean CG_ParseAnimationFile(const char *filename, clientInfo_t *ci, qboolean genderonly) {
	char *text_p, *prev;
	int len;
	int i;
	char *token;
	float fps;
	int skip;
	char text[20000];
	fileHandle_t f;
	animation_t *animations;

	animations = ci->animations;

	// load the file
	len = trap_FS_FOpenFile(filename, &f, FS_READ);
	if (len <= 0) {
		return qfalse;
	}
	if (len >= sizeof ( text) - 1) {
		CG_Printf("File %s too long\n", filename);
		trap_FS_FCloseFile(f);
		return qfalse;
	}
	trap_FS_Read(text, len, f);
	text[len] = 0;
	trap_FS_FCloseFile(f);

	// parse the text
	text_p = text;
	skip = 0; // quite the compiler warning

        // Mix3r_Durachok: skip this if just loading gender for head voice
        if (!genderonly) {
	        ci->footsteps = FOOTSTEP_NORMAL;
	        ci->gender = GENDER_MALE;
	        ci->fixedlegs = qfalse;
	        ci->fixedtorso = qfalse;
                // Mix3r_Durachok: set default camera offset for third person
                ci->eyepos[0] = 0.0;
                ci->eyepos[1] = 0.33;
                ci->eyepos[2] = 15.0;
        }

	// read optional parameters
	while (1) {
		prev = text_p; // so we can unget
		token = COM_Parse(&text_p);
		if ( !token[0] ) {
			break;
		}
                if (Q_strequal(token, "sex")) {
			token = COM_Parse(&text_p);
			if ( !token[0] ) {
				break;
			}
			if (token[0] == 'f' || token[0] == 'F') {
				ci->gender = GENDER_FEMALE;
			} else if (token[0] == 'n' || token[0] == 'N') {
				ci->gender = GENDER_NEUTER;
			} else {
				ci->gender = GENDER_MALE;
			}
			continue;
                }
                if (genderonly) {
                        continue;
                } else if (Q_strequal(token, "footsteps")) {
			token = COM_Parse(&text_p);
			if ( !token[0] ) {
				break;
			}
			if (Q_strequal(token, "default") || Q_strequal(token, "normal")) {
				ci->footsteps = FOOTSTEP_NORMAL;
			} else if (Q_strequal(token, "boot")) {
				ci->footsteps = FOOTSTEP_BOOT;
			} else if (Q_strequal(token, "flesh")) {
				ci->footsteps = FOOTSTEP_FLESH;
			} else if (Q_strequal(token, "mech")) {
				ci->footsteps = FOOTSTEP_MECH;
			} else if (Q_strequal(token, "energy")) {
				ci->footsteps = FOOTSTEP_ENERGY;
			} else {
				CG_Printf("Bad footsteps parm in %s: %s\n", filename, token);
			}
			continue;
                } else if ( Q_strequal( token, "headoffset" ) ) {
			for ( i = 0 ; i < 3; i++ ) {
				token = COM_Parse( &text_p );
				if ( !token[0] ) {
					break;
				}
			}
			continue;
		} else if (Q_strequal(token, "eyes")) { // leilei - EYES
			for (i = 0; i < 3; i++) {
				token = COM_Parse(&text_p);
				if ( !token[0] ) {
					break;
				}
				ci->eyepos[i] = atof(token);
			}
			continue;
		} else if (Q_strequal(token, "fixedlegs")) {
			ci->fixedlegs = qtrue;
			continue;
		} else if (Q_strequal(token, "fixedtorso")) {
			ci->fixedtorso = qtrue;
			continue;
		}

		// if it is a number, start parsing animations
		if (token[0] >= '0' && token[0] <= '9') {
			text_p = prev; // unget the token
			break;
		}
		Com_Printf("unknown token '%s' in %s\n", token, filename);
	}
        if (genderonly) {
                return qtrue;
        }

	// read information for each frame
	for (i = 0; i < MAX_ANIMATIONS; i++) {

		token = COM_Parse(&text_p);
		if ( !token[0] ) {
			if (i >= TORSO_GETFLAG && i <= TORSO_NEGATIVE) {
				animations[i] = animations[TORSO_GESTURE];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			// LEILEI - run animation fallback.
			if (i == TORSO_RUN) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_RUN2) {
				animations[i] = animations[TORSO_STAND2];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_RUN3) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_STRAFE) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_STAND3) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_JUMP) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_JUMP2) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_JUMP3) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_FALL) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_FALL2) {
				animations[i] = animations[TORSO_STAND2];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_FALL3) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == TORSO_TALK) {
				animations[i] = animations[TORSO_STAND];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == LEGS_STRAFE_LEFT) {
				animations[i] = animations[LEGS_RUN];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			if (i == LEGS_STRAFE_RIGHT) {
				animations[i] = animations[LEGS_RUN];
				animations[i].reversed = qfalse;
				animations[i].flipflop = qfalse;
				continue;
			}
			break;
		}
		animations[i].firstFrame = atoi(token);
		// leg only frames are adjusted to not count the upper body only frames
		if (i == LEGS_WALKCR) {
			skip = animations[LEGS_WALKCR].firstFrame - animations[TORSO_GESTURE].firstFrame;
		}
		if (i >= LEGS_WALKCR && i < TORSO_GETFLAG) {
			animations[i].firstFrame -= skip;
		}

		token = COM_Parse(&text_p);
		if ( !token[0] ) {
			break;
		}
		animations[i].numFrames = atoi(token);

		animations[i].reversed = qfalse;
		animations[i].flipflop = qfalse;
		// if numFrames is negative the animation is reversed
		if (animations[i].numFrames < 0) {
			animations[i].numFrames = -animations[i].numFrames;
			animations[i].reversed = qtrue;
		}

		token = COM_Parse(&text_p);
		if ( !token[0] ) {
			break;
		}
		animations[i].loopFrames = atoi(token);

		token = COM_Parse(&text_p);
		if ( !token[0] ) {
			break;
		}
		fps = atof(token);
		if (fps == 0) {
			fps = 1;
		}
		animations[i].frameLerp = 1000 / fps;
		animations[i].initialLerp = 1000 / fps;
	}

	if (i != MAX_ANIMATIONS) {
		CG_Printf("Error parsing animation file: %s\n", filename);
		return qfalse;
	}

	// crouch backward animation
	memcpy(&animations[LEGS_BACKCR], &animations[LEGS_WALKCR], sizeof (animation_t));
	animations[LEGS_BACKCR].reversed = qtrue;
	// walk backward animation
	memcpy(&animations[LEGS_BACKWALK], &animations[LEGS_WALK], sizeof (animation_t));
	animations[LEGS_BACKWALK].reversed = qtrue;
	// flag moving fast
	animations[FLAG_RUN].firstFrame = 0;
	animations[FLAG_RUN].numFrames = 16;
	animations[FLAG_RUN].loopFrames = 16;
	animations[FLAG_RUN].frameLerp = 1000 / 15;
	animations[FLAG_RUN].initialLerp = 1000 / 15;
	animations[FLAG_RUN].reversed = qfalse;
	// flag not moving or moving slowly
	animations[FLAG_STAND].firstFrame = 16;
	animations[FLAG_STAND].numFrames = 5;
	animations[FLAG_STAND].loopFrames = 0;
	animations[FLAG_STAND].frameLerp = 1000 / 20;
	animations[FLAG_STAND].initialLerp = 1000 / 20;
	animations[FLAG_STAND].reversed = qfalse;
	// flag speeding up
	animations[FLAG_STAND2RUN].firstFrame = 16;
	animations[FLAG_STAND2RUN].numFrames = 5;
	animations[FLAG_STAND2RUN].loopFrames = 1;
	animations[FLAG_STAND2RUN].frameLerp = 1000 / 15;
	animations[FLAG_STAND2RUN].initialLerp = 1000 / 15;
	animations[FLAG_STAND2RUN].reversed = qtrue;
	//
	// new anims changes
	//
	animations[TORSO_GETFLAG].flipflop = qtrue;
	animations[TORSO_GUARDBASE].flipflop = qtrue;
	animations[TORSO_PATROL].flipflop = qtrue;
	animations[TORSO_AFFIRMATIVE].flipflop = qtrue;
	animations[TORSO_NEGATIVE].flipflop = qtrue;
	//
	return qtrue;
}

/*
======================
CG_ParseEyesFile

Read eyes definitions.  Maybe this should be done engine-side for mod compatiblity? :S
======================
 */
static qboolean CG_ParseEyesFile(const char *filename, clientInfo_t *ci) {
	char *text_p;
	int len;
	int i;
	char *token;
	char text[20000];
	fileHandle_t f;
	// load the file
	len = trap_FS_FOpenFile(filename, &f, FS_READ);
	if (len <= 0) {
		return qfalse;
	}
	if (len >= sizeof ( text) - 1) {
		CG_Printf("File %s too long\n", filename);
		trap_FS_FCloseFile(f);
		return qfalse;
	}
	trap_FS_Read(text, len, f);
	text[len] = 0;
	trap_FS_FCloseFile(f);

	// parse the text
	text_p = text;


	// read optional parameters
	while (1) {
		token = COM_Parse(&text_p);
		if ( !token[0] ) {
			break;
		}


		if (!Q_stricmp(token, "eyes")) { // leilei - EYES
			for (i = 0; i < 3; i++) {
				token = COM_Parse(&text_p);
				if ( !token[0] ) {
					break;
				}
				ci->eyepos[i] = atof(token);
			}
			continue;
		}
		break;
	}

	return qtrue;
}

/*
==========================
CG_FileExists
==========================
 */
static qboolean CG_FileExists(const char *filename) {
	int len;

	len = trap_FS_FOpenFile(filename, NULL, FS_READ);
	if (len > 0) {
		return qtrue;
	}
	return qfalse;
}

/*
==========================
CG_FindClientModelFile
==========================
 */
static qboolean CG_FindClientModelFile(char *filename, int length, clientInfo_t *ci, const char *teamName, const char *modelName, const char *skinName, const char *base, const char *ext) {
	char *team, *charactersFolder;
	int i;

	if (CG_IsATeamGametype(cgs.gametype) && ci->teamTask != 9) {
		switch (ci->team) {
			case TEAM_BLUE:
			{
				team = "blue";
				break;
			}
			default:
			{
				team = "red";
				break;
			}
		}
	} else {
		team = "default";
	}
	charactersFolder = "";
	while (1) {
		for (i = 0; i < 2; i++) {
			if (i == 0 && teamName && *teamName) {
				//								"models/players/characters/sergei/stroggs/lower_lily_red.skin"
				Com_sprintf(filename, length, "models/players/%s%s/%s%s_%s_%s.%s", charactersFolder, modelName, teamName, base, skinName, team, ext);
			} else {
				//								"models/players/characters/sergei/lower_lily_red.skin"
				Com_sprintf(filename, length, "models/players/%s%s/%s_%s_%s.%s", charactersFolder, modelName, base, skinName, team, ext);
			}
			if (CG_FileExists(filename)) {
				return qtrue;
			}
			if (CG_IsATeamGametype(cgs.gametype) && ci->teamTask != 9) {
				if (i == 0 && teamName && *teamName) {
					//								"models/players/characters/sergei/stroggs/lower_red.skin"
					Com_sprintf(filename, length, "models/players/%s%s/%s%s_%s.%s", charactersFolder, modelName, teamName, base, team, ext);
				} else {
					//								"models/players/characters/sergei/lower_red.skin"
					Com_sprintf(filename, length, "models/players/%s%s/%s_%s.%s", charactersFolder, modelName, base, team, ext);
				}
			} else {
				if (i == 0 && teamName && *teamName) {
					//								"models/players/characters/sergei/stroggs/lower_lily.skin"
					Com_sprintf(filename, length, "models/players/%s%s/%s%s_%s.%s", charactersFolder, modelName, teamName, base, skinName, ext);
				} else {
					//								"models/players/characters/sergei/lower_lily.skin"
					Com_sprintf(filename, length, "models/players/%s%s/%s_%s.%s", charactersFolder, modelName, base, skinName, ext);
				}
			}
			if (CG_FileExists(filename)) {
				return qtrue;
			}
			if (!teamName || !*teamName) {
				break;
			}
		}
		// if tried the heads folder first
		if (charactersFolder[0]) {
			break;
		}
		charactersFolder = "characters/";
	}

	return qfalse;
}

/*
==========================
CG_FindClientHeadFile
==========================
 */
static qboolean CG_FindClientHeadFile(char *filename, int length, clientInfo_t *ci, const char *teamName, const char *headModelName, const char *headSkinName, const char *base, const char *ext) {
	char *team, *headsFolder;
	int i;

	if (CG_IsATeamGametype(cgs.gametype) && ci->teamTask != 9) {
		switch (ci->team) {
			case TEAM_BLUE:
			{
				team = "blue";
				break;
			}
			default:
			{
				team = "red";
				break;
			}
		}
	} else {
		team = "default";
	}

	if (headModelName[0] == '*') {
		headsFolder = "heads/";
		headModelName++;
	} else {
		headsFolder = "";
	}
	while (1) {
		for (i = 0; i < 2; i++) {
			if (i == 0 && teamName && *teamName) {
				Com_sprintf(filename, length, "models/players/%s%s/%s/%s%s_%s.%s", headsFolder, headModelName, headSkinName, teamName, base, team, ext);
			} else {
				Com_sprintf(filename, length, "models/players/%s%s/%s/%s_%s.%s", headsFolder, headModelName, headSkinName, base, team, ext);
			}
			if (CG_FileExists(filename)) {
				return qtrue;
			}
			if (CG_IsATeamGametype(cgs.gametype) && ci->teamTask != 9) {
				if (i == 0 && teamName && *teamName) {
					Com_sprintf(filename, length, "models/players/%s%s/%s%s_%s.%s", headsFolder, headModelName, teamName, base, team, ext);
				} else {
					Com_sprintf(filename, length, "models/players/%s%s/%s_%s.%s", headsFolder, headModelName, base, team, ext);
				}
			} else {
				if (i == 0 && teamName && *teamName) {
					Com_sprintf(filename, length, "models/players/%s%s/%s%s_%s.%s", headsFolder, headModelName, teamName, base, headSkinName, ext);
				} else {
					Com_sprintf(filename, length, "models/players/%s%s/%s_%s.%s", headsFolder, headModelName, base, headSkinName, ext);
				}
			}
			if (CG_FileExists(filename)) {
				return qtrue;
			}
			if (!teamName || !*teamName) {
				break;
			}
		}
		// if tried the heads folder first
		if (headsFolder[0]) {
			break;
		}
		headsFolder = "heads/";
	}

	return qfalse;
}

/*
==========================
CG_RegisterClientSkin
==========================
 */
static qboolean CG_RegisterClientSkin(clientInfo_t *ci, const char *teamName, const char *modelName, const char *skinName, const char *headModelName, const char *headSkinName) {
	char filename[MAX_QPATH];

	if (ci->onepiece) {
		Com_sprintf(filename, sizeof ( filename), "models/playerfs/%s/%stris_%s.skin", modelName, teamName, skinName);
		ci->legsSkin = trap_R_RegisterSkin(filename);
		if (!ci->legsSkin) {
			Com_Printf("Onepieced model skin load failure: %s\n", filename);
		}
	} else {
		if (CG_FindClientModelFile(filename, sizeof (filename), ci, teamName, modelName, skinName, "lower", "skin")) {
			ci->legsSkin = trap_R_RegisterSkin(filename);
		}
		if (!ci->legsSkin) {
			Com_Printf("Leg skin load failure: %s\n", filename);
		}
	}

	if (CG_FindClientModelFile(filename, sizeof (filename), ci, teamName, modelName, skinName, "upper", "skin")) {
		ci->torsoSkin = trap_R_RegisterSkin(filename);
	}
	if (!ci->torsoSkin) {
		Com_Printf("Torso skin load failure: %s\n", filename);
	}

	if (CG_FindClientHeadFile(filename, sizeof (filename), ci, teamName, headModelName, headSkinName, "head", "skin")) {
		ci->headSkin = trap_R_RegisterSkin(filename);
	}
	if (!ci->headSkin) {
		Com_Printf("Head skin load failure: %s\n", filename);
	}

	// if any skins failed to load
	if (!ci->legsSkin || !ci->torsoSkin || !ci->headSkin) {
		return qfalse;
	}
	return qtrue;
}

/*
==========================
CG_RegisterClientModelname
==========================
 */
static qboolean CG_RegisterClientModelname(clientInfo_t *ci, const char *modelName, const char *skinName, const char *headModelName, const char *headSkinName, const char *teamName) {
	char filename[MAX_QPATH * 2];
	const char *headName;
	char newTeamName[MAX_QPATH * 2];
	int FSloaded = 0;

	if (headModelName[0] == '\0') {
		headName = modelName;
	} else {
		headName = headModelName;
	}

	// leilei - onepiece model loading for certain maps or WIP animation debug
	ci->onepiece = 0;
	if (cg_enableFS.integer) {
		Com_sprintf(filename, sizeof ( filename), "models/playerfs/%s/tris.mdr", modelName);
		ci->legsModel = trap_R_RegisterModel(filename);
		if (ci->legsModel) {
			FSloaded = 1;
			ci->onepiece = 1;
		}
		if (!ci->legsModel)
			FSloaded = 0; // just end it

		if (FSloaded) {
			Com_Printf("%s is fanservice\n", filename);
		} else {
			Com_Printf("%s can't provide fanservice\n", filename);
		}
	}

	// load the usual model
	if (FSloaded == 0) {
		Com_sprintf(filename, sizeof ( filename), "models/players/%s/lower.mdr", modelName);
		ci->legsModel = trap_R_RegisterModel(filename);
		if (!ci->legsModel) {
			Com_sprintf(filename, sizeof ( filename), "models/players/characters/%s/lower.mdr", modelName);
			ci->legsModel = trap_R_RegisterModel(filename);
			if (!ci->legsModel) {
				Com_sprintf(filename, sizeof ( filename), "models/players/%s/lower.md3", modelName);
				ci->legsModel = trap_R_RegisterModel(filename);
				if (!ci->legsModel) {
					Com_sprintf(filename, sizeof ( filename), "models/players/characters/%s/lower.md3", modelName);
					ci->legsModel = trap_R_RegisterModel(filename);
					if (!ci->legsModel) {
						Com_Printf("Failed to load model file %s\n", filename);
						return qfalse;
					}
				}
			}
		}

		Com_sprintf(filename, sizeof ( filename), "models/players/%s/upper.mdr", modelName);
		ci->torsoModel = trap_R_RegisterModel(filename);
		if (!ci->torsoModel) {
			Com_sprintf(filename, sizeof ( filename), "models/players/characters/%s/upper.mdr", modelName);
			ci->torsoModel = trap_R_RegisterModel(filename);
			if (!ci->torsoModel) {
				Com_sprintf(filename, sizeof ( filename), "models/players/%s/upper.md3", modelName);
				ci->torsoModel = trap_R_RegisterModel(filename);
				if (!ci->torsoModel) {
					Com_sprintf(filename, sizeof ( filename), "models/players/characters/%s/upper.md3", modelName);
					ci->torsoModel = trap_R_RegisterModel(filename);
					if (!ci->torsoModel) {
						Com_Printf("Failed to load model file %s\n", filename);
						return qfalse;
					}
				}
			}
		}

	} // enableFS

	if (headName[0] == '*') {
		Com_sprintf(filename, sizeof ( filename), "models/players/heads/%s/%s.mdr", &headModelName[1], &headModelName[1]);
	} else {
		Com_sprintf(filename, sizeof ( filename), "models/players/%s/head.mdr", headName);
	}
	ci->headModel = trap_R_RegisterModel(filename);
	// if the head model could not be found and we didn't load from the heads folder try to load from there
	if (!ci->headModel && headName[0] != '*') {
		Com_sprintf(filename, sizeof ( filename), "models/players/heads/%s/%s.mdr", headModelName, headModelName);
		ci->headModel = trap_R_RegisterModel(filename);
	}
	if (!ci->headModel) {
		if (headName[0] == '*') {
			Com_sprintf(filename, sizeof ( filename), "models/players/heads/%s/%s.md3", &headModelName[1], &headModelName[1]);
		} else {
			Com_sprintf(filename, sizeof ( filename), "models/players/%s/head.md3", headName);
		}
		ci->headModel = trap_R_RegisterModel(filename);
		// if the head model could not be found and we didn't load from the heads folder try to load from there
		if (!ci->headModel && headName[0] != '*') {
			Com_sprintf(filename, sizeof ( filename), "models/players/heads/%s/%s.md3", headModelName, headModelName);
			ci->headModel = trap_R_RegisterModel(filename);
		}
		if (!ci->headModel) {
			Com_Printf("Failed to load model file %s\n", filename);
			return qfalse;
		}
	}

	// if any skins failed to load, return failure
	if (!CG_RegisterClientSkin(ci, teamName, modelName, skinName, headName, headSkinName)) {
		if (teamName && *teamName) {
			Com_Printf("Failed to load skin file: %s : %s : %s, %s : %s\n", teamName, modelName, skinName, headName, headSkinName);
			if (ci->team == TEAM_BLUE) {
				Com_sprintf(newTeamName, sizeof (newTeamName), "%s/", DEFAULT_BLUETEAM_NAME);
			} else {
				Com_sprintf(newTeamName, sizeof (newTeamName), "%s/", DEFAULT_REDTEAM_NAME);
			}
			if (!CG_RegisterClientSkin(ci, newTeamName, modelName, skinName, headName, headSkinName)) {
				Com_Printf("Failed to load skin file: %s : %s : %s, %s : %s\n", newTeamName, modelName, skinName, headName, headSkinName);
				return qfalse;
			}
		} else {
			Com_Printf("Failed to load skin file: %s : %s, %s : %s\n", modelName, skinName, headName, headSkinName);
			return qfalse;
		}
	}

	// load the animations
	// leilei - load one piece animation
	if (ci->onepiece) {
		Com_sprintf(filename, sizeof ( filename), "models/playerfs/%s/animation.cfg", modelName);
		if (!CG_ParseAnimationFile(filename, ci, qfalse)) {
			Com_Printf("Failed to load animation file %s\n", filename);
		}
	} else {
                // Mix3r_Durachok: some hack to load alternative animation .ini for some mods and Quake 3 native models,
                // to load eye pos for 3rd person, ini file tries to be loaded first, then normal procedure used
		Com_sprintf(filename, sizeof ( filename), "models/players/%s/animation.ini", modelName);
		if (!CG_ParseAnimationFile(filename, ci, qfalse)) {
			Com_sprintf(filename, sizeof ( filename), "models/players/characters/%s/animation.cfg", modelName);
			if (!CG_ParseAnimationFile(filename, ci, qfalse)) {
                                Com_sprintf(filename, sizeof ( filename), "models/players/%s/animation.cfg", modelName);
                                if (!CG_ParseAnimationFile(filename, ci, qfalse)) {
				        Com_Printf("Failed to load animation file %s\n", filename);
				        return qfalse;
                                }
			}
		}
	}




	// Mix3r_Durachok: override gender sound by reading head model gender from animation cfg
	Com_sprintf(filename, sizeof ( filename), "models/players/%s/animation.cfg", headName);
	if (!CG_ParseAnimationFile(filename, ci, qtrue)) {
		//	Com_Printf( "No eyes for %s\n", filename );
	}



	if (CG_FindClientHeadFile(filename, sizeof (filename), ci, teamName, headName, headSkinName, "icon", "skin")) {
		ci->modelIcon = trap_R_RegisterShaderNoMip(filename);
	} else if (CG_FindClientHeadFile(filename, sizeof (filename), ci, teamName, headName, headSkinName, "icon", "tga")) {
		ci->modelIcon = trap_R_RegisterShaderNoMip(filename);
	}

	if (!ci->modelIcon) {
		return qfalse;
	}

	return qtrue;
}

/*
====================
CG_ColorFromString
====================
 */
static void CG_ColorFromString(const char *v, vec3_t color) {
	int val;

	VectorClear(color);

	val = atoi(v);

	if (val < 1 || val > 7) {
		VectorSet(color, 1, 1, 1);
		return;
	}

	if (val & 1) {
		color[2] = 1.0f;
	}
	if (val & 2) {
		color[1] = 1.0f;
	}
	if (val & 4) {
		color[0] = 1.0f;
	}
}

/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
 */
static void CG_LoadClientInfo(int clientNum, clientInfo_t *ci) {
	const char *dir, *fallback;
	int i, modelloaded;
	const char *s;
	char teamname[MAX_QPATH];

	teamname[0] = 0;
#ifdef MISSIONPACK
	if (CG_IsATeamGametype(cgs.gametype)) {
		if (ci->team == TEAM_BLUE) {
			Q_strncpyz(teamname, cg_blueTeamName.string, sizeof (teamname));
		} else {
			Q_strncpyz(teamname, cg_redTeamName.string, sizeof (teamname));
		}
	}
	if (teamname[0]) {
		strcat(teamname, "/");
	}
#endif
	modelloaded = qtrue;
	if (!CG_RegisterClientModelname(ci, ci->modelName, ci->skinName, ci->headModelName, ci->headSkinName, teamname)) {
		//if (cg_buildScript.integer) {
		//	CG_Error("CG_RegisterClientModelname( %s, %s, %s, %s %s ) failed", ci->modelName, ci->skinName, ci->headModelName, ci->headSkinName, teamname);
		//}

		// fall back to default team name
		if (CG_IsATeamGametype(cgs.gametype)) {
			// keep skin name
			if (ci->team == TEAM_BLUE) {
				Q_strncpyz(teamname, DEFAULT_BLUETEAM_NAME, sizeof (teamname));
			} else {
				Q_strncpyz(teamname, DEFAULT_REDTEAM_NAME, sizeof (teamname));
			}
			/* Neon_Knight: Missionpack checks, if != 0, enables this. */
			if (cg_missionpackChecks.integer != 0) {
				if (!CG_RegisterClientModelname(ci, DEFAULT_TEAM_MODEL, ci->skinName, DEFAULT_TEAM_HEAD, ci->skinName, teamname)) {
					CG_Error("DEFAULT_TEAM_MODEL / skin (%s/%s) failed to register", DEFAULT_TEAM_MODEL, ci->skinName);
				}
			}
			/* /Neon_Knight */
		} else {
			if (!CG_RegisterClientModelname(ci, DEFAULT_MODEL, "default", DEFAULT_MODEL, "default", teamname)) {
				//CG_Error("DEFAULT_MODEL (%s) failed to register", DEFAULT_MODEL);
			}
		}
		modelloaded = qfalse;
	}

	ci->newAnims = qfalse;
	if (ci->torsoModel) {
		orientation_t tag;
		// if the torso model has the "tag_flag"
		if (trap_R_LerpTag(&tag, ci->torsoModel, 0, 0, 1, "tag_flag")) {
			ci->newAnims = qtrue;
		}
	}

	// sounds
	dir = ci->headModelName;
        if (ci->gender == GENDER_FEMALE) {
                fallback = "female";
        } else if (ci->gender == GENDER_MALE) {
        	fallback = "male";
        } else {
                fallback = "skelebot";
        }

	for (i = 0; i < MAX_CUSTOM_SOUNDS; i++) {
		s = cg_customSoundNames[i];
		if (!s) {
			break;
		}
		ci->sounds[i] = 0;
		// if the model didn't load use the sounds of the default model
		if (modelloaded) {
			ci->sounds[i] = trap_S_RegisterSound(va("sound/player/%s/%s", dir, s + 1), qfalse);
		}
		/* Neon_Knight: Missionpack checks, if != 0, enables this. */
		if (cg_missionpackChecks.integer) {
			if (!ci->sounds[i]) {
				ci->sounds[i] = trap_S_RegisterSound(va("sound/player/%s/%s", fallback, s + 1), qfalse);
			}
		}
		/* /Neon_Knight */
	}

	ci->deferred = qfalse;

	// reset any existing players and bodies, because they might be in bad
	// frames for this new model
	for (i = 0; i < MAX_GENTITIES; i++) {
		if (cg_entities[i].currentState.clientNum == clientNum
				&& cg_entities[i].currentState.eType == ET_PLAYER) {
			CG_ResetPlayerEntity(&cg_entities[i]);
		}
	}
}

/*
======================
CG_CopyClientInfoModel
======================
 */
static void CG_CopyClientInfoModel(clientInfo_t *from, clientInfo_t *to) {
	VectorCopy(from->eyepos, to->eyepos);
	to->footsteps = from->footsteps;
	to->gender = from->gender;

	to->legsModel = from->legsModel;
	to->legsSkin = from->legsSkin;
	to->torsoModel = from->torsoModel;
	to->torsoSkin = from->torsoSkin;
	to->headModel = from->headModel;
	to->headSkin = from->headSkin;
	to->modelIcon = from->modelIcon;

	to->newAnims = from->newAnims;

	memcpy(to->animations, from->animations, sizeof ( to->animations));
	memcpy(to->sounds, from->sounds, sizeof ( to->sounds));
}

/*
======================
CG_ScanForExistingClientInfo
======================
 */
static qboolean CG_ScanForExistingClientInfo(clientInfo_t *ci) {
	int i;
	clientInfo_t *match;

	for (i = 0; i < cgs.maxclients; i++) {
		match = &cgs.clientinfo[ i ];
		if (!match->infoValid) {
			continue;
		}
		if (match->deferred) {
			continue;
		}
		if (Q_strequal(ci->modelName, match->modelName)
				&& Q_strequal(ci->skinName, match->skinName)
				&& Q_strequal(ci->headModelName, match->headModelName)
				&& Q_strequal(ci->headSkinName, match->headSkinName)
				&& Q_strequal(ci->blueTeam, match->blueTeam)
				&& Q_strequal(ci->redTeam, match->redTeam)
				&& (!CG_IsATeamGametype(cgs.gametype) || ci->team == match->team)) {
			// this clientinfo is identical, so use it's handles

			ci->deferred = qfalse;

			CG_CopyClientInfoModel(match, ci);

			return qtrue;
		}
	}

	// nothing matches, so defer the load
	return qfalse;
}

/*
======================
CG_SetDeferredClientInfo

We aren't going to load it now, so grab some other
client's info to use until we have some spare time.
======================
 */
static void CG_SetDeferredClientInfo(int clientNum, clientInfo_t *ci) {
	int i;
	clientInfo_t *match;

	// if someone else is already the same models and skins we
	// can just load the client info
	for (i = 0; i < cgs.maxclients; i++) {
		match = &cgs.clientinfo[ i ];
		if (!match->infoValid || match->deferred) {
			continue;
		}
		if (!Q_strequal(ci->skinName, match->skinName) ||
				!Q_strequal(ci->modelName, match->modelName) ||
				(CG_IsATeamGametype(cgs.gametype) && ci->team != match->team)) {
			continue;
		}
		// just load the real info cause it uses the same models and skins
		CG_LoadClientInfo(clientNum, ci);
		return;
	}

	// if we are in teamplay, only grab a model if the skin is correct
	if (CG_IsATeamGametype(cgs.gametype)) {
		for (i = 0; i < cgs.maxclients; i++) {
			match = &cgs.clientinfo[ i ];
			if (!match->infoValid || match->deferred) {
				continue;
			}
			if (!Q_strequal(ci->skinName, match->skinName) ||
					(CG_IsATeamGametype(cgs.gametype) && ci->team != match->team)) {
				continue;
			}
			ci->deferred = qtrue;
			CG_CopyClientInfoModel(match, ci);
			return;
		}
		// load the full model, because we don't ever want to show
		// an improper team skin.  This will cause a hitch for the first
		// player, when the second enters.  Combat shouldn't be going on
		// yet, so it shouldn't matter
		CG_LoadClientInfo(clientNum, ci);
		return;
	}

	// find the first valid clientinfo and grab its stuff
	for (i = 0; i < cgs.maxclients; i++) {
		match = &cgs.clientinfo[ i ];
		if (!match->infoValid) {
			continue;
		}

		ci->deferred = qtrue;
		CG_CopyClientInfoModel(match, ci);
		return;
	}

	// we should never get here...
	CG_Printf("CG_SetDeferredClientInfo: no valid clients!\n");

	CG_LoadClientInfo(clientNum, ci);
}

/*
======================
CG_NewClientInfo
======================
 */
void CG_NewClientInfo(int clientNum) {
	clientInfo_t *ci;
	clientInfo_t newInfo;
	const char *configstring;
	const char *v;
	char *slash;

	ci = &cgs.clientinfo[clientNum];

	configstring = CG_ConfigString(clientNum + CS_PLAYERS);
	if (!configstring[0]) {
		memset(ci, 0, sizeof ( *ci));
		return; // player just left
	}

	// build into a temp buffer so the defer checks can use
	// the old value
	memset(&newInfo, 0, sizeof ( newInfo));

	// colors
	v = Info_ValueForKey(configstring, "c1");
	CG_ColorFromString(v, newInfo.color1);

	// bot skill
	v = Info_ValueForKey(configstring, "skill");
	newInfo.botSkill = atoi(v);

        v = Info_ValueForKey(configstring, "c2");
        // Mix3r_Durachok: read localized bot name multiplied by 10 and store it instead of handicap info
        if (newInfo.botSkill > 0) {
                newInfo.handicap = atoi(v);
                if (newInfo.handicap > 9) {
                        v = va("%i",newInfo.handicap%10);
                        newInfo.handicap = newInfo.handicap / 10;
                } else {
                        newInfo.handicap = 0;
                }
        }
	CG_ColorFromString(v, newInfo.color2);

        // Mix3r_Durachok: apply stored localized bot name or actual name
        if (newInfo.botSkill > 0 && newInfo.handicap > 0) {
                v = COM_Localize(newInfo.handicap);
        } else {
                // isolate the player's name
	        v = Info_ValueForKey(configstring, "n");
        }
        Q_strncpyz(newInfo.name, v, sizeof ( newInfo.name));

	// handicap Mix3r_Durachok: bots are excluded
        if (newInfo.botSkill > 0) {
        } else {
	        v = Info_ValueForKey(configstring, "hc");
	        newInfo.handicap = atoi(v);
        }

	// wins
	v = Info_ValueForKey(configstring, "w");
	newInfo.wins = atoi(v);

	// losses
	v = Info_ValueForKey(configstring, "l");
	newInfo.losses = atoi(v);

	// team
	v = Info_ValueForKey(configstring, "t");
	newInfo.team = atoi(v);

	// team task
	v = Info_ValueForKey(configstring, "tt");
	newInfo.teamTask = atoi(v);

	// team leader
	v = Info_ValueForKey(configstring, "tl");
	newInfo.teamLeader = atoi(v);

	v = Info_ValueForKey(configstring, "g_redteam");
	Q_strncpyz(newInfo.redTeam, v, MAX_TEAMNAME);

	v = Info_ValueForKey(configstring, "g_blueteam");
	Q_strncpyz(newInfo.blueTeam, v, MAX_TEAMNAME);

	// model
	v = Info_ValueForKey(configstring, "model");
	if (cg_forceModel.integer) {
		// forcemodel makes everyone use a single model
		// to prevent load hitches
		char modelStr[MAX_CVAR_VALUE_STRING];
		char *skin;

		/* Neon_Knight: Missionpack checks, if != 0, enables this. */
		//if (cg_missionpackChecks.integer) {
		//	if (CG_IsATeamGametype(cgs.gametype)) {
		//		Q_strncpyz(newInfo.modelName, DEFAULT_TEAM_MODEL, sizeof ( newInfo.modelName));
		//		Q_strncpyz(newInfo.skinName, "default", sizeof ( newInfo.skinName));
		//	}
		//} else {
			trap_Cvar_VariableStringBuffer("model", modelStr, sizeof (modelStr));
			if ((skin = strchr(modelStr, '/')) == NULL) {
				skin = "default";
			} else {
				*skin++ = 0;
			}

			Q_strncpyz(newInfo.skinName, skin, sizeof ( newInfo.skinName));
			Q_strncpyz(newInfo.modelName, modelStr, sizeof ( newInfo.modelName));
		//}
		/* /Neon_Knight */
		if (CG_IsATeamGametype(cgs.gametype)) {
			// keep skin name
			slash = strchr(v, '/');
			if (slash) {
				Q_strncpyz(newInfo.skinName, slash + 1, sizeof ( newInfo.skinName));
			}
		}
	} else {
		Q_strncpyz(newInfo.modelName, v, sizeof ( newInfo.modelName));

		slash = strchr(newInfo.modelName, '/');
		if (!slash) {
			// modelName didn not include a skin name
			Q_strncpyz(newInfo.skinName, "default", sizeof ( newInfo.skinName));
		} else {
			Q_strncpyz(newInfo.skinName, slash + 1, sizeof ( newInfo.skinName));
			// truncate modelName
			*slash = 0;
		}
	}
        //CG_Printf("mdl %s\n", newInfo.modelName);
        //CG_Printf("skn %s\n", newInfo.skinName);

	// head model
	v = Info_ValueForKey(configstring, "hmodel");
	if (cg_forceModel.integer) {
		// forcemodel makes everyone use a single model
		// to prevent load hitches
		char modelStr[MAX_QPATH];
		char *skin;

		/* Neon_Knight: Missionpack checks, if != 0, enables this. */
		//if (cg_missionpackChecks.integer) {
			//if (CG_IsATeamGametype(cgs.gametype)) {
			//	Q_strncpyz(newInfo.headModelName, DEFAULT_TEAM_MODEL, sizeof ( newInfo.headModelName));
			//	Q_strncpyz(newInfo.headSkinName, "default", sizeof ( newInfo.headSkinName));
			//}
		//} else {
			trap_Cvar_VariableStringBuffer("headmodel", modelStr, sizeof ( modelStr));
			if ((skin = strchr(modelStr, '/')) == NULL) {
				skin = "default";
			} else {
				*skin++ = 0;
			}

			Q_strncpyz(newInfo.headSkinName, skin, sizeof ( newInfo.headSkinName));
			Q_strncpyz(newInfo.headModelName, modelStr, sizeof ( newInfo.headModelName));
		//}
		/* /Neon_Knight */

		if (CG_IsATeamGametype(cgs.gametype)) {
			// keep skin name
			slash = strchr(v, '/');
			if (slash) {
				Q_strncpyz(newInfo.headSkinName, slash + 1, sizeof ( newInfo.headSkinName));
			}
		}
	} else {
		Q_strncpyz(newInfo.headModelName, v, sizeof ( newInfo.headModelName));

		slash = strchr(newInfo.headModelName, '/');
		if (!slash) {
			// modelName didn not include a skin name
			Q_strncpyz(newInfo.headSkinName, "default", sizeof ( newInfo.headSkinName));
		} else {
			Q_strncpyz(newInfo.headSkinName, slash + 1, sizeof ( newInfo.headSkinName));
			// truncate modelName
			*slash = 0;
		}
	}

	// scan for an existing clientinfo that matches this modelname
	// so we can avoid loading checks if possible
	if (!CG_ScanForExistingClientInfo(&newInfo)) {
		qboolean forceDefer;

		forceDefer = trap_MemoryRemaining() < 4000000;

		// if we are defering loads, just have it pick the first valid
		if (forceDefer || (cg_deferPlayers.integer && !cg_buildScript.integer && !cg.loading)) {
			// keep whatever they had if it won't violate team skins
			CG_SetDeferredClientInfo(clientNum, &newInfo);
			// if we are low on memory, leave them with this model
			if (forceDefer) {
				CG_Printf("Memory is low. Using deferred model.\n");
				newInfo.deferred = qfalse;
			}
		} else {
			CG_LoadClientInfo(clientNum, &newInfo);
		}
	}

	// replace whatever was there with the new one
	newInfo.infoValid = qtrue;
	*ci = newInfo;
}

/*
======================
CG_LoadDeferredPlayers

Called each frame when a player is dead
and the scoreboard is up
so deferred players can be loaded
======================
 */
void CG_LoadDeferredPlayers(void) {
	int i;
	clientInfo_t *ci;

	// scan for a deferred player to load
	for (i = 0, ci = cgs.clientinfo; i < cgs.maxclients; i++, ci++) {
		if (ci->infoValid && ci->deferred) {
			// if we are low on memory, leave it deferred
			if (trap_MemoryRemaining() < 4000000) {
				CG_Printf("Memory is low. Using deferred model.\n");
				ci->deferred = qfalse;
				continue;
			}
			CG_LoadClientInfo(i, ci);
			//			break;
		}
	}
}

/*
=============================================================================

PLAYER ANIMATION

=============================================================================
 */

/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
 */
static void CG_SetLerpFrameAnimation(clientInfo_t *ci, lerpFrame_t *lf, int newAnimation) {
	animation_t *anim;

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if (newAnimation < 0 || newAnimation >= MAX_TOTALANIMATIONS) {
		CG_Error("Bad animation number: %i", newAnimation);
	}

	anim = &ci->animations[ newAnimation ];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if (cg_debugAnim.integer) {
		CG_Printf("Anim: %i\n", newAnimation);
	}
}

/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
 */
static void CG_RunLerpFrame(clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale) {
	int f, numFrames;
	animation_t *anim;

	// debugging tool to get no animations
	if (cg_animSpeed.integer == 0) {
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	// see if the animation sequence is switching
	if (newAnimation != lf->animationNumber || !lf->animation) {
		CG_SetLerpFrameAnimation(ci, lf, newAnimation);
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if (cg.time >= lf->frameTime) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;

		// get the next frame based on the animation
		anim = lf->animation;
		if (!anim->frameLerp) {
			return; // shouldn't happen
		}
		if (cg.time < lf->animationTime) {
			lf->frameTime = lf->animationTime; // initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = (lf->frameTime - lf->animationTime) / anim->frameLerp;
		f *= speedScale; // adjust for haste, etc

		numFrames = anim->numFrames;
		if (anim->flipflop) {
			numFrames *= 2;
		}
		if (f >= numFrames) {
			f -= numFrames;
			if (anim->loopFrames) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		if (anim->reversed) {
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
		} else if (anim->flipflop && f >= anim->numFrames) {
			lf->frame = anim->firstFrame + anim->numFrames - 1 - (f % anim->numFrames);
		} else {
			lf->frame = anim->firstFrame + f;
		}
		if (cg.time > lf->frameTime) {
			lf->frameTime = cg.time;
			if (cg_debugAnim.integer) {
				CG_Printf("Clamp lf->frameTime\n");
			}
		}
	}

	if (lf->frameTime > cg.time + 200) {
		lf->frameTime = cg.time;
	}

	if (lf->oldFrameTime > cg.time) {
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	if (lf->frameTime == lf->oldFrameTime) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float) (cg.time - lf->oldFrameTime) / (lf->frameTime - lf->oldFrameTime);
	}
}

/*
===============
CG_ClearLerpFrame
===============
 */
static void CG_ClearLerpFrame(clientInfo_t *ci, lerpFrame_t *lf, int animationNumber) {
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimation(ci, lf, animationNumber);
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
}

/*
===============
CG_PlayerAnimation
===============
 */
static void CG_PlayerAnimation(centity_t *cent, int *legsOld, int *legs, float *legsBackLerp, int *torsoOld, int *torso, float *torsoBackLerp) {
	clientInfo_t *ci;
	float speedScale;

	if (cg_noPlayerAnims.integer) {
		*legsOld = *legs = *torsoOld = *torso = 0;
		return;
	}

	if (cent->currentState.powerups & (1 << PW_HASTE)) {
		speedScale = 1.5;
	} else {
		speedScale = 1;
	}

	ci = &cgs.clientinfo[ cent->currentState.clientNum ];

	// do the shuffle turn frames locally
	if (cent->pe.legs.yawing && (cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) == LEGS_IDLE) {
		CG_RunLerpFrame(ci, &cent->pe.legs, LEGS_TURN, speedScale);
	} else {
                // Mix3r_Durachok: weird hack for angelyss one-piece-model idle-shooting-anim
                // angelyss/animation.cfg modified (torso_attack first frame fake anim set to 101 to detect angelyss model easily)
                // even if torso is fake, we still can catch it's current played frame and use it to set some decent tits-from-legs-piece frame:

                switch((cent->currentState.legsAnim & ~ANIM_TOGGLEBIT)) {
                   case LEGS_IDLE:
                      if ( cgs.clientinfo[ cent->currentState.clientNum ].animations[TORSO_ATTACK].firstFrame == cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_WALK].firstFrame ) {
                         //CG_Printf("effect %i\n", cent->pe.torso.frame);
                         if (cent->pe.torso.frame >= 101 && cent->pe.torso.frame < 106) {
                            if (cent->pe.torso.frame == 102) {
                              cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].firstFrame = 196;
                            } else {
                              cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].firstFrame = 195;
                            }
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].numFrames = 1;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].loopFrames = 0;
                         } else if (cent->pe.torso.frame >= 136 && cent->pe.torso.frame <= 139) {
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].firstFrame = 101;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].numFrames = 1;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].loopFrames = 0;
                         } else {
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].firstFrame = 181;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].numFrames = 15;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLE].loopFrames = 15;
                         }
                      }
                      break;
                   case LEGS_IDLECR:
                      if ( cgs.clientinfo[ cent->currentState.clientNum ].animations[TORSO_ATTACK].firstFrame == cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_WALK].firstFrame ) {
                         //CG_Printf("effect %i\n", cent->pe.torso.frame);
                         if (cent->pe.torso.frame >= 101 && cent->pe.torso.frame < 103) {
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].firstFrame = 210;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].numFrames = 1;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].loopFrames = 0;
                         } else if (cent->pe.torso.frame >= 136 && cent->pe.torso.frame <= 138) {
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].firstFrame = 210;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].numFrames = 1;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].loopFrames = 0;
                         } else {
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].firstFrame = 197;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].numFrames = 13;
                            cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_IDLECR].loopFrames = 13;
                         }
                      }
                      break;
                   case LEGS_RUN:
                   case LEGS_BACK:
                   case LEGS_STRAFE_LEFT:
                   case LEGS_STRAFE_RIGHT:
                      *legsOld = cgs.clientinfo[ cent->currentState.clientNum ].animations[(cent->currentState.legsAnim & ~ANIM_TOGGLEBIT)].firstFrame; // left footstep frame
                      *legs = (int)(cgs.clientinfo[ cent->currentState.clientNum ].animations[(cent->currentState.legsAnim & ~ANIM_TOGGLEBIT)].numFrames*0.5); // frame count from left to right footstep
                      *legsBackLerp = *legsOld + *legs; // to get middle frame of run animation
                      if ((cent->pe.legs.frame <= *legsBackLerp && cgs.clientinfo[ cent->currentState.clientNum ].animations[MAX_ANIMATIONS].firstFrame > *legsBackLerp) || (cent->pe.legs.frame >= *legsBackLerp && cgs.clientinfo[ cent->currentState.clientNum ].animations[MAX_ANIMATIONS].firstFrame < *legsBackLerp)) {
                         if (cgs.clientinfo[ cent->currentState.clientNum ].animations[MAX_ANIMATIONS].numFrames > cg.time - 600) {
                            switch (cgs.clientinfo[ cent->currentState.clientNum ].animations[MAX_ANIMATIONS].loopFrames) {
			       case 2:
			          trap_S_StartSound(NULL, cent->currentState.number, CHAN_BODY,cgs.media.footsteps[ FOOTSTEP_METAL ][rand()&3]);
			          break;
                               case 3:
				  trap_S_StartSound(NULL, cent->currentState.number, CHAN_BODY,cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3]);
				  break;
			       default:
				  trap_S_StartSound(NULL, cent->currentState.number, CHAN_BODY,cgs.media.footsteps[ cgs.clientinfo[ cent->currentState.clientNum ].footsteps ][rand()&3]);
				  break;
		            }
                         }
                      }
                      cgs.clientinfo[ cent->currentState.clientNum ].animations[MAX_ANIMATIONS].firstFrame = cent->pe.legs.frame;
                      break;
                   // end cases
                }
                CG_RunLerpFrame(ci, &cent->pe.legs, cent->currentState.legsAnim, speedScale);
	}

	*legsOld = cent->pe.legs.oldFrame;
	*legs = cent->pe.legs.frame;
	*legsBackLerp = cent->pe.legs.backlerp;

	CG_RunLerpFrame(ci, &cent->pe.torso, cent->currentState.torsoAnim, speedScale);

	*torsoOld = cent->pe.torso.oldFrame;
	*torso = cent->pe.torso.frame;
	*torsoBackLerp = cent->pe.torso.backlerp;
}

/*
=============================================================================

PLAYER ANGLES

=============================================================================
 */

/*
==================
CG_SwingAngles
==================
 */
static void CG_SwingAngles(float destination, float swingTolerance, float clampTolerance,
		float speed, float *angle, qboolean *swinging) {
	float swing;
	float move;
	float scale;

	if (!*swinging) {
		// see if a swing should be started
		swing = AngleSubtract(*angle, destination);
		if (swing > swingTolerance || swing < -swingTolerance) {
			*swinging = qtrue;
		}
	}

	if (!*swinging) {
		return;
	}

	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract(destination, *angle);
	scale = fabs(swing);
	if (scale < swingTolerance * 0.5) {
		scale = 0.5;
	} else if (scale < swingTolerance) {
		scale = 1.0;
	} else {
		scale = 2.0;
	}

	// swing towards the destination angle
	if (swing >= 0) {
		move = cg.frametime * scale * speed;
		if (move >= swing) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod(*angle + move);
	} else {
		move = cg.frametime * scale * -speed;
		if (move <= swing) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod(*angle + move);
	}

	// clamp to no more than tolerance
	swing = AngleSubtract(destination, *angle);
	if (swing > clampTolerance) {
		*angle = AngleMod(destination - (clampTolerance - 1));
	} else if (swing < -clampTolerance) {
		*angle = AngleMod(destination + (clampTolerance - 1));
	}
}

/*
=================
CG_AddPainTwitch
=================
 */
static void CG_AddPainTwitch(centity_t *cent, vec3_t torsoAngles) {
	int t;
	float f;

	t = cg.time - cent->pe.painTime;
	if (t >= PAIN_TWITCH_TIME) {
		return;
	}

	f = 1.0 - (float) t / PAIN_TWITCH_TIME;

	if (cent->pe.painDirection) {
		torsoAngles[ROLL] += 20 * f;
	} else {
		torsoAngles[ROLL] -= 20 * f;
	}
}


/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
 */

vec3_t eyeat;

static void CG_PlayerAngles(centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3]) {
	vec3_t legsAngles, torsoAngles, headAngles;
	float dest;
	static int movementOffsets[8] = {0, 22, 45, -22, 0, 22, -45, -22};
	vec3_t velocity;
	float speed;
	int dir, clientNum;
	clientInfo_t *ci = NULL;
	//int camereyes;



	{
		VectorCopy(cent->lerpAngles, headAngles);
		headAngles[YAW] = AngleMod(headAngles[YAW]);
	}


	//headAngles[YAW] = AngleMod( headAngles[YAW] );


	VectorClear(legsAngles);
	VectorClear(torsoAngles);


	//camereyes = 0;
	//if (cent->currentState.number == cg.snap->ps.clientNum) {
	//	camereyes = 1; // it's me!
	//}

	// leilei -- new third person camera prep
	//cent->newcamrunning = 0;
	//if (cg_cameramode.integer == 1) {
		//if ((cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_ATTACK) {
		//	cent->newcamrunning = 1;
		//} else {
		//	cent->newcamrunning = 0;
		//}
	//}

	// --------- yaw -------------


	// turn head if reached the eye tolerance limit

	// allow yaw to drift a bit
	if ((cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) != LEGS_IDLE
			|| ((cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_STAND
			&& (cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_STAND2)) {
		// if not standing still, always point all in the same direction
		cent->pe.torso.yawing = qtrue; // always center
		cent->pe.torso.pitching = qtrue; // always center
		cent->pe.legs.yawing = qtrue; // always center

	}




	// etc

	//if (cent->newcamrunning) {
		// lean towards the direction of travel
	//	VectorCopy(cent->currentState.pos.trDelta, velocity);
	//	speed = VectorNormalize(velocity);
	//	if (speed) {
	//		vec3_t veel;
	//		vec3_t fwad, rait;
	//		AngleVectors(veel, velocity, fwad, rait);
	//		speed *= 0.05f;
	//	}
	//}

	// adjust legs for movement dir
	if (cent->currentState.eFlags & EF_DEAD) {
		// don't let dead bodies twitch
		dir = 0;
	} else {
		dir = cent->currentState.angles2[YAW];
		if (dir < 0 || dir > 7) {
			CG_Error("Bad player movement angle");
		}
	}


	legsAngles[YAW] = headAngles[YAW] + movementOffsets[ dir ];
        //legsAngles[YAW] = headAngles[YAW];
	torsoAngles[YAW] = headAngles[YAW] + 0.25 * movementOffsets[ dir ];

	// torso
	CG_SwingAngles(torsoAngles[YAW], 25, 90, cg_swingSpeed.value, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing);
	CG_SwingAngles(legsAngles[YAW], 40, 90, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing);

	torsoAngles[YAW] = cent->pe.torso.yawAngle;
	legsAngles[YAW] = cent->pe.legs.yawAngle;

	// --------- pitch -------------
	// turn head if reached the eye tolerance limit

	// only show a fraction of the pitch angle in the torso

	{
		if (headAngles[PITCH] > 180) {
			dest = (-360 + headAngles[PITCH]) * 0.75f;
		} else {
			dest = headAngles[PITCH] * 0.75f;
		}
	}
	CG_SwingAngles(dest, 15, 30, 0.1f, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching);
	torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

	//
	clientNum = cent->currentState.clientNum;
	if (clientNum >= 0 && clientNum < MAX_CLIENTS) {
		ci = &cgs.clientinfo[ clientNum ];
		if (ci->fixedtorso) {
			torsoAngles[PITCH] = 0.0f;
		}
	//	if (camereyes) {
	//		torsoAngles[PITCH] = 0.0f; // don't pitch 
	//	}
	}



	// --------- roll -------------


	// lean towards the direction of travel
	VectorCopy(cent->currentState.pos.trDelta, velocity);
	speed = VectorNormalize(velocity);
        dir = 0;
	if (speed) {
		vec3_t axis[3];
		float side;

		speed *= 0.05f;

                AnglesToAxis(legsAngles, axis);

		side = speed * DotProduct(velocity, axis[0]);
		legsAngles[PITCH] += side;

                // Mix3r_Durachok: lean only when driving AirHog
                if ((cent->currentState.powerups) && (cent->currentState.powerups & (1 << PW_FLIGHT))) {
                        head[0][0] = side;
		        side = speed * DotProduct(velocity, axis[1]);
                        head[0][1] = -side;
		        legsAngles[ROLL] -= side;
                        dir = 1;
                }
	}

	//
	clientNum = cent->currentState.clientNum;
	if (clientNum >= 0 && clientNum < MAX_CLIENTS) {
		ci = &cgs.clientinfo[ clientNum ];


		if (ci->fixedlegs) {
			legsAngles[YAW] = torsoAngles[YAW];
			legsAngles[PITCH] = 0.0f;
			legsAngles[ROLL] = 0.0f;
		}

	}

	// pain twitch
	CG_AddPainTwitch(cent, torsoAngles);
	// TODO: make eyes erode on pain twitch

	// leilei - eyes hack

	//if (camereyes) {
	//	cent->eyesAngles[YAW] = headAngles[YAW];
	//	cent->eyesAngles[PITCH] = headAngles[PITCH];
	//	cent->eyesAngles[ROLL] = headAngles[ROLL];
	//}

	// pull the angles back out of the hierarchial chain
	AnglesSubtract(headAngles, torsoAngles, headAngles);
	AnglesSubtract(torsoAngles, legsAngles, torsoAngles);
        if (dir != 1) { // Mix3r_Durachok: protect head axis from overwriting, AirHog lean angles stored there
                AnglesToAxis(headAngles, head);
        }
	AnglesToAxis(legsAngles, legs);
	AnglesToAxis(torsoAngles, torso);

	// eyes crap
	//{
		//vec3_t eyelookfrom;

		// offset from the model we have.
		//VectorClear(eyelookfrom);
		//if (ci) {
			//VectorCopy(ci->eyepos, cent->pe.eyepos);
		//}
		//VectorCopy(eyelookfrom, cent->pe.eyepos);			// leilei - copy eye poistion
	//}

}


//==========================================================================

/*
===============
CG_HasteTrail
===============
 */
static void CG_HasteTrail(centity_t *cent) {
	localEntity_t *smoke;
	vec3_t origin;
	int anim;

	if (cent->trailTime > cg.time) {
		return;
	}
	anim = cent->pe.legs.animationNumber & ~ANIM_TOGGLEBIT;
	if (anim != LEGS_RUN && anim != LEGS_BACK) {
		return;
	}

	cent->trailTime += 100;
	if (cent->trailTime < cg.time) {
		cent->trailTime = cg.time;
	}

	VectorCopy(cent->lerpOrigin, origin);
	origin[2] -= 16;

	smoke = CG_SmokePuff(origin, vec3_origin,
			8,
			1, 1, 1, 1,
			500,
			cg.time,
			0,
			0,
			cgs.media.hastePuffShader);

	// use the optimized local entity add
	smoke->leType = LE_SCALE_FADE;
}

/*
===============
CG_BreathPuffs
===============
 */
static void CG_BreathPuffs(centity_t *cent, refEntity_t *head) {
	clientInfo_t *ci;
	vec3_t up, origin;
	int contents;

	ci = &cgs.clientinfo[ cent->currentState.number ];

	if (!cg_enableBreath.integer) {
		return;
	}
	if (cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson) {
		return;
	}
	if (cent->currentState.eFlags & EF_DEAD) {
		return;
	}
	contents = CG_PointContents(head->origin, 0);
	if (contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA)) {
		return;
	}
	if (ci->breathPuffTime > cg.time) {
		return;
	}

	VectorSet(up, 0, 0, 8);
	VectorMA(head->origin, 8, head->axis[0], origin);
	VectorMA(origin, -4, head->axis[2], origin);
	CG_SmokePuff(origin, up, 16, 1, 1, 1, 0.66f, 1500, cg.time, cg.time + 400, LEF_PUFF_DONT_SCALE, cgs.media.shotgunSmokePuffShader);
	ci->breathPuffTime = cg.time + 2000;
}

/*
===============
CG_TrailItem
===============
 */
static void CG_TrailItem(centity_t *cent, qhandle_t hModel, float *z_altitude) {
	refEntity_t ent;
	vec3_t angles;
	// vec3_t axis[3];

        memset(&ent, 0, sizeof ( ent));

	VectorCopy(cent->lerpAngles, angles);
        ent.origin[2] = 16;
        angles[PITCH] = 0;

        if (cent->currentState.number == cg.snap->ps.clientNum) {
        	// angles[ROLL] = -45; //mix3r was 0
                ent.renderfx = RF_DEPTHHACK;
                ent.origin[2] = 6;
        }
        AnglesToAxis(angles, ent.axis);
	VectorMA(cent->lerpOrigin, -ent.origin[2], ent.axis[0], ent.origin);

	angles[YAW] += 0; // mix3r was 90
	AnglesToAxis(angles, ent.axis);

        if (ent.renderfx == RF_DEPTHHACK) {
                VectorScale(ent.axis[0], 0.33, ent.axis[0]);
	        VectorScale(ent.axis[1], 0.33, ent.axis[1]);
	        VectorScale(ent.axis[2], 0.33, ent.axis[2]);
                if (cg.renderingThirdPerson) {
                   if (cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[1] == 0) {
                      ent.origin[2] = *z_altitude + cg.predictedPlayerState.viewheight - 28;
                   } else {
                      ent.origin[2] = *z_altitude + cg.predictedPlayerState.viewheight - 41;
                   }
                } else {
                      ent.origin[2] += cg.predictedPlayerState.viewheight;
                }
                angles[2] = cg.time - cg.duckTime;
	        if ( angles[2] < DUCK_TIME) {
		      ent.origin[2] -= cg.duckChange * (DUCK_TIME - angles[2]) / DUCK_TIME;
	        }
        } else {
                ent.origin[2] += 16;
        }

	ent.hModel = hModel;
	trap_R_AddRefEntityToScene(&ent);
}

/*
===============
CG_PlayerFlag
===============
 */
static void CG_PlayerFlag(centity_t *cent, qhandle_t hSkin, refEntity_t *torso) {
	clientInfo_t *ci;
	refEntity_t pole;
	refEntity_t flag;
	vec3_t angles, dir;
	int legsAnim, flagAnim, updateangles;
	float angle, d;

	// show the flag pole model
	memset(&pole, 0, sizeof (pole));
	pole.hModel = cgs.media.flagPoleModel;
	VectorCopy(torso->lightingOrigin, pole.lightingOrigin);
	pole.shadowPlane = torso->shadowPlane;
	pole.renderfx = torso->renderfx;
	CG_PositionEntityOnTag(&pole, torso, torso->hModel, "tag_flag");
        ///////////////////////
        if (cent->currentState.number == cg.snap->ps.clientNum) {
                VectorScale(pole.axis[0], 0.66, pole.axis[0]);
	        VectorScale(pole.axis[1], 0.66, pole.axis[1]);
	        VectorScale(pole.axis[2], 0.66, pole.axis[2]);
        }
        //////////////////////
	trap_R_AddRefEntityToScene(&pole);

	// show the flag model
	memset(&flag, 0, sizeof (flag));
	flag.hModel = cgs.media.flagFlapModel;
	flag.customSkin = hSkin;
	VectorCopy(torso->lightingOrigin, flag.lightingOrigin);
	flag.shadowPlane = torso->shadowPlane;
	flag.renderfx = torso->renderfx;

	VectorClear(angles);

	updateangles = qfalse;
	legsAnim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	if (legsAnim == LEGS_IDLE || legsAnim == LEGS_IDLECR) {
		flagAnim = FLAG_STAND;
	} else if (legsAnim == LEGS_WALK || legsAnim == LEGS_WALKCR) {
		flagAnim = FLAG_STAND;
		updateangles = qtrue;
	} else {
		flagAnim = FLAG_RUN;
		updateangles = qtrue;
	}

	if (updateangles) {

		VectorCopy(cent->currentState.pos.trDelta, dir);
		// add gravity
		dir[2] += 100;
		VectorNormalize(dir);
		d = DotProduct(pole.axis[2], dir);
		// if there is anough movement orthogonal to the flag pole
		if (fabs(d) < 0.9) {
			//
			d = DotProduct(pole.axis[0], dir);
			if (d > 1.0f) {
				d = 1.0f;
			} else if (d < -1.0f) {
				d = -1.0f;
			}
			angle = acos(d);

			d = DotProduct(pole.axis[1], dir);
			if (d < 0) {
				angles[YAW] = 360 - angle * 180 / M_PI;
			} else {
				angles[YAW] = angle * 180 / M_PI;
			}
			if (angles[YAW] < 0)
				angles[YAW] += 360;
			if (angles[YAW] > 360)
				angles[YAW] -= 360;

			//vectoangles( cent->currentState.pos.trDelta, tmpangles );
			//angles[YAW] = tmpangles[YAW] + 45 - cent->pe.torso.yawAngle;
			// change the yaw angle
			CG_SwingAngles(angles[YAW], 25, 90, 0.15f, &cent->pe.flag.yawAngle, &cent->pe.flag.yawing);
		}
	}

	// set the yaw angle
	angles[YAW] = cent->pe.flag.yawAngle;
	// lerp the flag animation frames
	ci = &cgs.clientinfo[ cent->currentState.clientNum ];
	CG_RunLerpFrame(ci, &cent->pe.flag, flagAnim, 1);
	flag.oldframe = cent->pe.flag.oldFrame;
	flag.frame = cent->pe.flag.frame;
	flag.backlerp = cent->pe.flag.backlerp;

	AnglesToAxis(angles, flag.axis);
	CG_PositionRotatedEntityOnTag(&flag, &pole, pole.hModel, "tag_flag");
        ///////////////////////
        if (cent->currentState.number == cg.snap->ps.clientNum) {
                VectorScale(flag.axis[0], 0.66, flag.axis[0]);
	        VectorScale(flag.axis[1], 0.66, flag.axis[1]);
	        VectorScale(flag.axis[2], 0.66, flag.axis[2]);
        }
        //////////////////////
	trap_R_AddRefEntityToScene(&flag);
}

/*
===============
CG_PlayerTokens
===============
 */
static void CG_PlayerTokens(centity_t *cent, int renderfx) {
	int tokens, i, j;
	float angle;
	refEntity_t ent;
	vec3_t dir, origin;
	skulltrail_t *trail;
	if (cent->currentState.number >= MAX_CLIENTS) {
		return;
	}
	trail = &cg.skulltrails[cent->currentState.number];
	tokens = cent->currentState.generic1;
	if (!tokens) {
		trail->numpositions = 0;
		return;
	}

	if (tokens > MAX_SKULLTRAIL) {
		tokens = MAX_SKULLTRAIL;
	}

	// add skulls if there are more than last time
	for (i = 0; i < tokens - trail->numpositions; i++) {
		for (j = trail->numpositions; j > 0; j--) {
			VectorCopy(trail->positions[j - 1], trail->positions[j]);
		}
		VectorCopy(cent->lerpOrigin, trail->positions[0]);
	}
	trail->numpositions = tokens;

	// move all the skulls along the trail
	VectorCopy(cent->lerpOrigin, origin);
	for (i = 0; i < trail->numpositions; i++) {
		VectorSubtract(trail->positions[i], origin, dir);
		if (VectorNormalize(dir) > 30) {
			VectorMA(origin, 30, dir, trail->positions[i]);
		}
		VectorCopy(trail->positions[i], origin);
	}

	memset(&ent, 0, sizeof ( ent));
	if (cgs.clientinfo[ cent->currentState.clientNum ].team == TEAM_BLUE) {
		ent.hModel = cgs.media.redCubeModel;
	} else {
		ent.hModel = cgs.media.blueCubeModel;
	}
	ent.renderfx = renderfx;

	VectorCopy(cent->lerpOrigin, origin);
	for (i = 0; i < trail->numpositions; i++) {
		VectorSubtract(origin, trail->positions[i], ent.axis[0]);
		ent.axis[0][2] = 0;
		VectorNormalize(ent.axis[0]);
		VectorSet(ent.axis[2], 0, 0, 1);
		CrossProduct(ent.axis[0], ent.axis[2], ent.axis[1]);

		VectorCopy(trail->positions[i], ent.origin);
		angle = (((cg.time + 500 * MAX_SKULLTRAIL - 500 * i) / 16) & 255) * (M_PI * 2) / 255;
		ent.origin[2] += sin(angle) * 10;
		trap_R_AddRefEntityToScene(&ent);
		VectorCopy(trail->positions[i], origin);
	}
}

/*
===============
CG_PlayerPowerups
===============
 */
static void CG_PlayerPowerups(centity_t *cent, refEntity_t *torso, float *zaltitude) {
	int powerups;
	clientInfo_t *ci;

	powerups = cent->currentState.powerups;
	if (!powerups) {
		return;
	}

	// quad gives a dlight
	if (powerups & (1 << PW_QUAD)) {
		trap_R_AddLightToScene(cent->lerpOrigin, 200 + (rand()&31), 0.2f, 0.2f, 1);
	}

	// flight plays a looped sound
	if (powerups & (1 << PW_FLIGHT)) {
                //vec3_t vlct;
                //vlct[0] = cg_gun_x.value;
		//vlct[1] = cg_gun_y.value;
		//vlct[2] = cg_gun_z.value;
                if (cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson) {
                        trap_S_AddLoopingSound(cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.airhinSound);
                } else {
	                trap_S_AddLoopingSound(cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.airhogSound);
                }
	}

	ci = &cgs.clientinfo[ cent->currentState.clientNum ];
	// redflag
	if (powerups & (1 << PW_REDFLAG)) {
		if (ci->newAnims) {
			CG_PlayerFlag(cent, cgs.media.redFlagFlapSkin, torso);
		} else {
			CG_TrailItem(cent, cgs.media.redFlagModel,zaltitude);
		}
		trap_R_AddLightToScene(cent->lerpOrigin, 200 + (rand()&31), 1.0, 0.2f, 0.2f);
	}

	// blueflag
	if (powerups & (1 << PW_BLUEFLAG)) {
		if (ci->newAnims) {
			CG_PlayerFlag(cent, cgs.media.blueFlagFlapSkin, torso);
		} else {
			CG_TrailItem(cent, cgs.media.blueFlagModel,zaltitude);
		}
		trap_R_AddLightToScene(cent->lerpOrigin, 200 + (rand()&31), 0.2f, 0.2f, 1.0);
	}

	// neutralflag
	if (powerups & (1 << PW_NEUTRALFLAG)) {
		if (ci->newAnims) {
			CG_PlayerFlag(cent, cgs.media.neutralFlagFlapSkin, torso);
		} else {
			CG_TrailItem(cent, cgs.media.neutralFlagModel,zaltitude);
		}
		trap_R_AddLightToScene(cent->lerpOrigin, 200 + (rand()&31), 1.0, 1.0, 1.0);
	}

	// haste leaves smoke trails
	if (powerups & (1 << PW_HASTE)) {
		CG_HasteTrail(cent);
	}
}

/*
===============
CG_PlayerFloatSprite

Float a sprite over the player's head
===============
 */
static void CG_PlayerFloatSprite(centity_t *cent, qhandle_t shader) {
	int rf;
	refEntity_t ent;

	if (cent->currentState.number == cg.snap->ps.clientNum) {
		rf = RF_THIRD_PERSON; // only show in mirrors
	} else {
		rf = 0;
	}

	memset(&ent, 0, sizeof ( ent));
	VectorCopy(cent->lerpOrigin, ent.origin);
	ent.origin[2] += 48;
	ent.reType = RT_SPRITE;
	ent.customShader = shader;
	ent.radius = 10;
	ent.renderfx = rf;
	ent.shaderRGBA[0] = 255;
	ent.shaderRGBA[1] = 255;
	ent.shaderRGBA[2] = 255;
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene(&ent);
}

/*
===============
CG_PlayerSprites

Float sprites over the player's head
===============
 */
static void CG_PlayerSprites(centity_t *cent) {
	int team;

	if (cent->currentState.eFlags & EF_CONNECTION) {
		CG_PlayerFloatSprite(cent, cgs.media.connectionShader);
		return;
	}

	if (cent->currentState.eFlags & EF_TALK) {
		CG_PlayerFloatSprite(cent, cgs.media.balloonShader);
		return;
	}

	if (cent->currentState.eFlags & EF_AWARD_IMPRESSIVE) {
		CG_PlayerFloatSprite(cent, cgs.media.medalImpressive);
		return;
	}

	if (cent->currentState.eFlags & EF_AWARD_EXCELLENT) {
		CG_PlayerFloatSprite(cent, cgs.media.medalExcellent);
		return;
	}

	if (cent->currentState.eFlags & EF_AWARD_GAUNTLET) {
		CG_PlayerFloatSprite(cent, cgs.media.medalGauntlet);
		return;
	}

	if (cent->currentState.eFlags & EF_AWARD_DEFEND) {
		CG_PlayerFloatSprite(cent, cgs.media.medalDefend);
		return;
	}

	if (cent->currentState.eFlags & EF_AWARD_ASSIST) {
		CG_PlayerFloatSprite(cent, cgs.media.medalAssist);
		return;
	}

	if (cent->currentState.eFlags & EF_AWARD_CAP) {
		CG_PlayerFloatSprite(cent, cgs.media.medalCapture);
		return;
	}

	team = cgs.clientinfo[ cent->currentState.clientNum ].team;
	if (!(cent->currentState.eFlags & EF_DEAD) &&
			cg.snap->ps.persistant[PERS_TEAM] == team &&
			CG_IsATeamGametype(cgs.gametype)) {
		if (cg_drawFriend.integer) {
			CG_PlayerFloatSprite(cent, cgs.media.friendShader);
		}
		return;
	}
}

/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
 */
static void CG_PlayerSplash(centity_t *cent) {
	vec3_t start, end;
	trace_t trace;
	int contents;
	polyVert_t verts[4];

	if (!cg_shadows.integer) {
		return;
	}

	VectorCopy(cent->lerpOrigin, end);
	end[2] -= 24;

	// if the feet aren't in liquid, don't make a mark
	// this won't handle moving water brushes, but they wouldn't draw right anyway...
	contents = CG_PointContents(end, 0);
	if (!(contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA))) {
		return;
	}

	VectorCopy(cent->lerpOrigin, start);
	start[2] += 32;

	// if the head isn't out of liquid, don't make a mark
	contents = CG_PointContents(start, 0);
	if (contents & (CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA)) {
		return;
	}

	// trace down to find the surface
	trap_CM_BoxTrace(&trace, start, end, NULL, NULL, 0, (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA));

	if (trace.fraction == 1.0) {
		return;
	}

	// create a mark polygon
	VectorCopy(trace.endpos, verts[0].xyz);
	verts[0].xyz[0] -= 32;
	verts[0].xyz[1] -= 32;
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	VectorCopy(trace.endpos, verts[1].xyz);
	verts[1].xyz[0] -= 32;
	verts[1].xyz[1] += 32;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	VectorCopy(trace.endpos, verts[2].xyz);
	verts[2].xyz[0] += 32;
	verts[2].xyz[1] += 32;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	VectorCopy(trace.endpos, verts[3].xyz);
	verts[3].xyz[0] += 32;
	verts[3].xyz[1] -= 32;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;

	trap_R_AddPolyToScene(cgs.media.wakeMarkShader, 4, verts);
}

/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
 */
void CG_AddRefEntityWithPowerups(refEntity_t *ent, entityState_t *state, int team, qboolean isMissile) {

                //VectorScale(ent->axis[0], 2.0, ent->axis[0]);
	        //VectorScale(ent->axis[1], 2.0, ent->axis[1]);
	        //VectorScale(ent->axis[2], 2.0, ent->axis[2]);


	if (state->powerups & (1 << PW_INVIS)) {
		if ((cgs.dmflags & DF_INVIS) == 0) {
			ent->customShader = cgs.media.invisShader;
			trap_R_AddRefEntityToScene(ent);
		}
	} else {
		trap_R_AddRefEntityToScene(ent);
		if (!isMissile && (cgs.dmflags & DF_PLAYER_OVERLAY) && !(state->eFlags & EF_DEAD)) {
			switch (team) {
				case TEAM_RED:
					ent->customShader = cgs.media.redOverlay;
					trap_R_AddRefEntityToScene(ent);
					break;
				case TEAM_BLUE:
					ent->customShader = cgs.media.blueOverlay;
					trap_R_AddRefEntityToScene(ent);
					break;
				default:
					ent->customShader = cgs.media.neutralOverlay;
					trap_R_AddRefEntityToScene(ent);
			}
		}

		if (state->powerups & (1 << PW_QUAD)) {
			if (team == TEAM_RED)
				ent->customShader = cgs.media.redQuadShader;
			else
				ent->customShader = cgs.media.quadShader;
			trap_R_AddRefEntityToScene(ent);
		}
		if (state->powerups & (1 << PW_REGEN)) {
			if (((cg.time / 100) % 10) == 1) {
				ent->customShader = cgs.media.regenShader;
				trap_R_AddRefEntityToScene(ent);
			}
		}
		if (state->powerups & (1 << PW_BATTLESUIT)) {
			ent->customShader = cgs.media.battleSuitShader;
			trap_R_AddRefEntityToScene(ent);
		}
	}
}

/*
=================
CG_LightVerts
=================
 */
int CG_LightVerts(vec3_t normal, int numVerts, polyVert_t *verts) {
	int i, j;
	float incoming;
	vec3_t ambientLight;
	vec3_t lightDir;
	vec3_t directedLight;

	trap_R_LightForPoint(verts[0].xyz, ambientLight, directedLight, lightDir);

	for (i = 0; i < numVerts; i++) {
		incoming = DotProduct(normal, lightDir);
		if (incoming <= 0) {
			verts[i].modulate[0] = ambientLight[0];
			verts[i].modulate[1] = ambientLight[1];
			verts[i].modulate[2] = ambientLight[2];
			verts[i].modulate[3] = 255;
			continue;
		}
		j = (ambientLight[0] + incoming * directedLight[0]);
		if (j > 255) {
			j = 255;
		}
		verts[i].modulate[0] = j;

		j = (ambientLight[1] + incoming * directedLight[1]);
		if (j > 255) {
			j = 255;
		}
		verts[i].modulate[1] = j;

		j = (ambientLight[2] + incoming * directedLight[2]);
		if (j > 255) {
			j = 255;
		}
		verts[i].modulate[2] = j;

		verts[i].modulate[3] = 255;
	}
	return qtrue;
}

/*
===============
CG_Player
===============
 */
void CG_Player(centity_t *cent) {
	clientInfo_t *ci;
	refEntity_t legs;
	refEntity_t torso;
	refEntity_t head;
	int clientNum;
	int renderfx;
        trace_t trace;
	//float shadowPlane = 0.0;

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo

	clientNum = cent->currentState.clientNum;
	if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
		CG_Error("Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[ clientNum ];

	// it is possible to see corpses from disconnected players that may
	// not have valid clientinfo
	if (!ci->infoValid) {
		return;
	}

	// get the player model information
	renderfx = 0;

	memset(&legs, 0, sizeof (legs));
	memset(&torso, 0, sizeof (torso));
	memset(&head, 0, sizeof (head));

	// get the rotation information
	CG_PlayerAngles(cent, legs.axis, torso.axis, head.axis);

	// get the animation state (after rotation, to allow feet shuffle)
	CG_PlayerAnimation(cent, &legs.oldframe, &legs.frame, &legs.backlerp, &torso.oldframe, &torso.frame, &torso.backlerp);

	// add the talk baloon or disconnect icon
	CG_PlayerSprites(cent);

	// add a water splash if partially in and out of water
	CG_PlayerSplash(cent);

	if (cg_shadows.integer == 3) {
		renderfx |= RF_SHADOW_PLANE;
	}
	renderfx |= RF_LIGHTING_ORIGIN; // use the same origin for all
	if (cgs.gametype == GT_HARVESTER) {
		CG_PlayerTokens(cent, renderfx);
	}

	// add the legs
        if ((cent->currentState.powerups) && (cent->currentState.powerups & (1 << PW_FLIGHT))) {
                legs.hModel = cg_items[33].models[0]; // 33 is the flight item index of bg_misc.c
                torso.hModel = legs.hModel; // Mix3r_Durachok: assigning torso at this point means player drives AirHog
        } else {
                legs.hModel = ci->legsModel;
	        legs.customSkin = ci->legsSkin;
        }

	VectorCopy(cent->lerpOrigin, legs.origin);
	VectorCopy(cent->lerpOrigin, legs.lightingOrigin);

        if (cent->currentState.number == cg.snap->ps.clientNum) {
                torso.radius = 0.7; // Mix3r_Durachok: scale runes down for third person viewed
                if (cg.renderingThirdPerson && !cg.zoomed) {
                       // Mix3r_Durachok: smooth out stair climbing for local third person player
                       head.origin[2] = cg.time - cg.stepTime;
                       if ( head.origin[2] < STEP_TIME ) {
		               legs.lightingOrigin[2] = legs.origin[2] -= cg.stepChange * (STEP_TIME - head.origin[2]) / STEP_TIME;
	               }
                       if (torso.hModel) {
                               head.oldorigin[PITCH] = head.axis[0][0];
                               head.oldorigin[ROLL] = head.axis[0][1];
                               head.oldorigin[YAW] = cent->lerpAngles[YAW]; //cg.refdefViewAngles[YAW];
                               AnglesToAxis(head.oldorigin, legs.axis);
                       }
                } else {
                       // Mix3r_Durachok: setup offset and draw first person legs
                       renderfx |= RF_THIRD_PERSON; // draw other parts for mirrors & another players
                       VectorCopy(cg.refdefViewAngles, legs.oldorigin);
                       legs.oldorigin[PITCH] = 0; // get horisontal view direction, drop away pitch
                       AngleVectors( legs.oldorigin, legs.oldorigin, NULL, NULL );
                       head.oldorigin[ROLL] = 0;
                       if (torso.hModel || cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[1] == 0) {
                          head.origin[2] = -40;
                          head.oldorigin[PITCH] = 0;
                          head.oldorigin[YAW] = cg.refdefViewAngles[YAW];
                          if (torso.hModel) {
                                  head.oldorigin[ROLL] = head.axis[0][1] * 0.75;
                          }
                       } else {
                          head.origin[2] = -31;
                          head.oldorigin[PITCH] = -15;
                          head.oldorigin[YAW] = cent->pe.legs.yawAngle;
                       }

                       // let's bend fake first person legs back a bit
                       // but saving normal axis first in unused (at this moment) torso vec3 props

                       VectorCopy(legs.axis[0], torso.lightingOrigin);
                       VectorCopy(legs.axis[1], torso.oldorigin);
                       VectorCopy(legs.axis[2], torso.origin);

                       AnglesToAxis(head.oldorigin, legs.axis);

                       legs.origin[2] -= 6;
                       VectorMA( legs.origin, head.origin[2], legs.oldorigin, legs.origin );
                       VectorMA( legs.lightingOrigin, head.origin[2] / 2, legs.oldorigin, legs.lightingOrigin );
                       cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_JUMPB] = cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_JUMP];
                       legs.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON;
                       CG_AddRefEntityWithPowerups(&legs, &cent->currentState, ci->team, qfalse);

                       // restoring normal legs axis and pos:
                       VectorCopy(cent->lerpOrigin, legs.origin);
                       VectorCopy(torso.lightingOrigin, legs.axis[0]);
                       VectorCopy(torso.oldorigin, legs.axis[1]);
                       VectorCopy(torso.origin, legs.axis[2]);

                }
        } else {
                torso.radius = 1.0;
        }

	VectorCopy(legs.origin, legs.oldorigin); // don't positionally lerp at all

        legs.renderfx = renderfx;

        // add the shadow

        if (cg_shadows.integer == 1) {
                if (cent->currentState.powerups & (1 << PW_INVIS)) {
                        // shadow = qfalse;
                } else {
                        torso.lightingOrigin[0]=torso.lightingOrigin[1]=-15.0;
                        torso.lightingOrigin[2]=0.0;

                        head.lightingOrigin[0]=head.lightingOrigin[1]=15.0;
                        head.lightingOrigin[2]=2.0;

                        head.origin[0] = legs.lightingOrigin[0];
                        head.origin[1] = legs.lightingOrigin[1];
                        head.origin[2] = legs.lightingOrigin[2] - SHADOW_DISTANCE;
                        trap_CM_BoxTrace(&trace, legs.lightingOrigin, head.origin, torso.lightingOrigin, head.lightingOrigin, 0, MASK_PLAYERSOLID);
                        if (trace.fraction == 1.0 || trace.startsolid || trace.allsolid) {
		                // shadow = qfalse;
                   	} else {
                                legs.shadowPlane = trace.endpos[2] + 1;
                                head.origin[2] = 1.0 - trace.fraction;
                                CG_ImpactMark(cgs.media.shadowMarkShader, trace.endpos, trace.plane.normal,
                                cent->pe.legs.yawAngle, head.origin[2], head.origin[2], head.origin[2], 1, qfalse, 24, qtrue);
                        }
                }
	}

        // draw 3rd persons legs

	CG_AddRefEntityWithPowerups(&legs, &cent->currentState, ci->team, qfalse);

	// add the torso
        // Mix3r_Durachok: checking AirHog presence first
        if (torso.hModel) {
                CG_PlayerPowerups(cent, &torso, &legs.origin[2]);
                VectorMA(legs.origin, 13.0, legs.axis[0], legs.origin);
                VectorMA(legs.origin, 2.4, legs.axis[2], legs.origin);
                legs.nonNormalizedAxes = qtrue; // flag to point weapon in direction of this player look
                CG_AddPlayerWeapon(&legs, NULL, cent, ci->team, "tag_torso");
                return;
        }

	torso.hModel = ci->torsoModel;
	if (!torso.hModel) {
		return;
	}

	torso.customSkin = ci->torsoSkin;

	VectorCopy(cent->lerpOrigin, torso.lightingOrigin);

	CG_PositionRotatedEntityOnTag(&torso, &legs, ci->legsModel, "tag_torso");

	torso.shadowPlane = legs.shadowPlane;
	torso.renderfx = renderfx;

        CG_AddRefEntityWithPowerups(&torso, &cent->currentState, ci->team, qfalse);

	// add the head

        VectorCopy(cent->lerpOrigin, head.lightingOrigin);
        head.shadowPlane = legs.shadowPlane;
        head.renderfx = renderfx;
        head.hModel = ci->headModel;

	if (head.hModel) {
                head.customSkin = ci->headSkin;
                CG_PositionRotatedEntityOnTag(&head, &torso, ci->torsoModel, "tag_head");
                if ( cgs.clientinfo[ cent->currentState.clientNum ].animations[TORSO_ATTACK].firstFrame == cgs.clientinfo[ cent->currentState.clientNum ].animations[LEGS_WALK].firstFrame ) {
                        // don't render head for angelyss (because of head-from-tits solid model)
                        head.origin[2] += 12;
                } else {
	                CG_AddRefEntityWithPowerups(&head, &cent->currentState, ci->team, qfalse);
                }
	        CG_BreathPuffs(cent, &head);
	}

        // get current legs anim

        legs.skinNum = cent->pe.legs.animationNumber & ~ANIM_TOGGLEBIT;

        // add dust trail

	if (cg_enableDust.integer) {
                if (cent->dustTrailTime > cg.time) {
                } else {
                        if (legs.skinNum != LEGS_LANDB && legs.skinNum != LEGS_LAND) {
	                } else {
                                cent->dustTrailTime += 40;
	                        if (cent->dustTrailTime < cg.time) {
		                        cent->dustTrailTime = cg.time;
	                        }
                                VectorCopy(cent->currentState.pos.trBase, head.axis[0]);
                                head.axis[0][2] -= 64.0;
                                CG_Trace(&trace, cent->currentState.pos.trBase, NULL, NULL, head.axis[0],
                                cent->currentState.number, MASK_PLAYERSOLID);
                                if (trace.surfaceFlags & SURF_DUST) {
                                        head.axis[0][2] += 48.0;
                                        VectorSet(head.axis[1], 0, 0, -30);
                                        CG_SmokePuff(head.axis[0], head.axis[1],24,.8f, .8f, 0.7f, 0.33f,
                                        500,cg.time,0,0,cgs.media.dustPuffShader);
                                }
                        }
                }
        }

	// add powerups floating behind the player
	CG_PlayerPowerups(cent, &torso, &head.origin[2]);

        // add the gun / barrel / flash
        // Mix3r_Durachok: moved here from end of function to release torso.axis (3 vec3_t vars)
        // for further use. torso.axis[0] used for dir, torso.axis[1] used for angles

        if ( cgs.clientinfo[ cent->currentState.clientNum ].eyepos[1] == 0) {
                torso.radius = 1.0; // set runes scale to 1.0 regardless of pov for arachnotron
                // Mix3r_Durachok: scammerleg (arachnotron) gun position hack
                VectorCopy(legs.origin, head.origin);
                VectorMA(legs.origin, 12.0, legs.axis[0], legs.origin);
                switch (legs.skinNum) {
                        case LEGS_RUN:
                        {
                                head.axis[0][0] = -8.1;
                                break;
                        }
                        case LEGS_BACK:
                        {
                                head.axis[0][0] = -15.2;
                                break;
                        }
                        case LEGS_WALK:
                        {
                                head.axis[0][0] = -10.5;
                                break;
                        }
                        default:
			{
			        head.axis[0][0] = -13.0;
				break;
			}
                }
                VectorMA(legs.origin, head.axis[0][0], legs.axis[2], legs.origin);
                legs.nonNormalizedAxes = qtrue; // flag to point weapon in direction of this player look
                CG_AddPlayerWeapon(&legs, NULL, cent, ci->team, "tag_torso");
                VectorCopy(head.origin, legs.origin);
        } else {
	        CG_AddPlayerWeapon(&torso, NULL, cent, ci->team, "tag_weapon");
        }

        // Mix3r_Durachok let's use head refentity for items and powerups, because head already rendered at this moment

        head.customShader = 0;

	if (cent->currentState.powerups & (1 << PW_GUARD)) {
		head.hModel = cgs.media.guardPowerupModel;
	} else if (cent->currentState.powerups & (1 << PW_SCOUT)) {
		head.hModel = cgs.media.scoutPowerupModel;
	} else if (cent->currentState.powerups & (1 << PW_DOUBLER)) {
		head.hModel = cgs.media.doublerPowerupModel;
	} else if (cent->currentState.powerups & (1 << PW_AMMOREGEN)) {
		head.hModel = cgs.media.ammoRegenPowerupModel;
	} else {
                head.hModel = 0;
        }
        if (head.hModel) {
                VectorScale(legs.axis[0], torso.radius, head.axis[0]);
	        VectorScale(legs.axis[1], torso.radius, head.axis[1]);
	        VectorScale(legs.axis[2], torso.radius, head.axis[2]);
		head.frame = 0;
		head.oldframe = 0;
		head.customSkin = 0;
                head.origin[0] = cent->lerpOrigin[0];
                head.origin[1] = cent->lerpOrigin[1];
                head.origin[2] = torso.origin[2];
		trap_R_AddRefEntityToScene(&head);
        }

	if (cent->currentState.eFlags & EF_KAMIKAZE) {
                head.customSkin = 0;
		if (cent->currentState.eFlags & EF_DEAD) {
			// one skull bobbing above the dead body
			head.rotation = ((cg.time / 7) & 255) * (M_PI * 2) / 255;
			if (head.rotation > M_PI * 2)
				head.rotation -= (float) M_PI * 2;
			torso.axis[0][0] = sin(head.rotation) * 20;
			torso.axis[0][1] = cos(head.rotation) * 20;
			head.rotation = ((cg.time / 4) & 255) * (M_PI * 2) / 255;
			torso.axis[0][2] = 15 + sin(head.rotation) * 8;
			VectorAdd(torso.origin, torso.axis[0], head.origin);

			torso.axis[0][2] = 0;
			VectorCopy(torso.axis[0], head.axis[1]);
			VectorNormalize(head.axis[1]);
			VectorSet(head.axis[2], 0, 0, 1);
			CrossProduct(head.axis[1], head.axis[2], head.axis[0]);

			head.hModel = cgs.media.kamikazeHeadModel;
			trap_R_AddRefEntityToScene(&head);
			head.hModel = cgs.media.kamikazeHeadTrail;
			trap_R_AddRefEntityToScene(&head);
		} else {
			// three skulls spinning around the player
                        head.radius = torso.origin[2] - legs.origin[2];

			head.rotation = ((cg.time / 4) & 255) * (M_PI * 2) / 255;
			torso.axis[1][0] = sin(head.rotation) * 30;
			torso.axis[1][1] = (head.rotation * 180 / M_PI) + 90;
			if (torso.axis[1][1] > 360)
				torso.axis[1][1] -= 360;
			torso.axis[1][2] = 0;
			AnglesToAxis(torso.axis[1], head.axis);

                        if (cent->currentState.number == cg.snap->ps.clientNum) {
                               torso.skinNum = 15;
                               VectorScale(head.axis[0], 0.3, head.axis[0]);
	                       VectorScale(head.axis[1], 0.3, head.axis[1]);
	                       VectorScale(head.axis[2], 0.3, head.axis[2]);
                        } else {
                               torso.skinNum = 20;
                        }
                        torso.axis[0][0] = cos(head.rotation) * torso.skinNum;
			torso.axis[0][1] = sin(head.rotation) * torso.skinNum;
			torso.axis[0][2] = cos(head.rotation) * torso.skinNum;
                        VectorAdd(legs.origin, torso.axis[0], head.origin);
                        head.origin[2] += head.radius;

			head.hModel = cgs.media.kamikazeHeadModel;
			trap_R_AddRefEntityToScene(&head);
			// flip the trail because this skull is spinning in the other direction
			VectorInverse(head.axis[1]);
			head.hModel = cgs.media.kamikazeHeadTrail;
			trap_R_AddRefEntityToScene(&head);

			head.rotation = ((cg.time / 4) & 255) * (M_PI * 2) / 255 + M_PI;
			if (head.rotation > M_PI * 2)
				head.rotation -= (float) M_PI * 2;
			torso.axis[1][0] = cos(head.rotation - 0.5 * M_PI) * 30;
			torso.axis[1][1] = 360 - (head.rotation * 180 / M_PI);
			if (torso.axis[1][1] > 360)
				torso.axis[1][1] -= 360;
			torso.axis[1][2] = 0;
			AnglesToAxis(torso.axis[1], head.axis);

		        if (cent->currentState.number == cg.snap->ps.clientNum) {
                               VectorScale(head.axis[0], 0.3, head.axis[0]);
	                       VectorScale(head.axis[1], 0.3, head.axis[1]);
	                       VectorScale(head.axis[2], 0.3, head.axis[2]);
                        }
                        torso.axis[0][0] = sin(head.rotation) * torso.skinNum;
			torso.axis[0][1] = cos(head.rotation) * torso.skinNum;
			torso.axis[0][2] = cos(head.rotation) * torso.skinNum;
			VectorAdd(legs.origin, torso.axis[0], head.origin);
                        head.origin[2] += head.radius;

			head.hModel = cgs.media.kamikazeHeadModel;
			trap_R_AddRefEntityToScene(&head);
			head.hModel = cgs.media.kamikazeHeadTrail;
			trap_R_AddRefEntityToScene(&head);

			head.rotation = ((cg.time / 3) & 255) * (M_PI * 2) / 255 + 0.5 * M_PI;
			if (head.rotation > M_PI * 2)
			        head.rotation -= (float) M_PI * 2;
                        torso.axis[0][0] = sin(head.rotation) * torso.skinNum;
			torso.axis[0][1] = cos(head.rotation) * torso.skinNum;
			torso.axis[0][2] = 0;
			VectorCopy(torso.axis[0], head.axis[1]);
			VectorNormalize(head.axis[1]);
			VectorSet(head.axis[2], 0, 0, 1);
			CrossProduct(head.axis[1], head.axis[2], head.axis[0]);

                        if (cent->currentState.number == cg.snap->ps.clientNum) {
                                //head.renderfx &= ~RF_THIRD_PERSON;
                                VectorScale(head.axis[0], 0.3, head.axis[0]);
	                        VectorScale(head.axis[1], 0.3, head.axis[1]);
	                        VectorScale(head.axis[2], 0.3, head.axis[2]);
                        }
			VectorAdd(legs.origin, torso.axis[0], head.origin);
                        head.origin[2] += head.radius;

			head.hModel = cgs.media.kamikazeHeadModel;
			trap_R_AddRefEntityToScene(&head);
			head.hModel = cgs.media.kamikazeHeadTrail;
			trap_R_AddRefEntityToScene(&head);
		}
	}

	if ((cent->currentState.powerups & (1 << PW_INVULNERABILITY)) || cg.time - ci->invulnerabilityStopTime < 250) {
                if (cent->currentState.powerups & (1 << PW_INVULNERABILITY)) {
		        if (!ci->invulnerabilityStartTime) {
			        ci->invulnerabilityStartTime = cg.time;
		        }
		        ci->invulnerabilityStopTime = cg.time;
	        } else {
		        ci->invulnerabilityStartTime = 0;
	        }
		head.hModel = cgs.media.invulnerabilityPowerupModel;
		head.frame = 0;
		head.oldframe = 0;
		head.customSkin = 0;
		// always draw
		head.renderfx &= ~RF_THIRD_PERSON;
		VectorCopy(cent->lerpOrigin, head.origin);

		if (cg.time - ci->invulnerabilityStartTime < 250) {
			head.radius = (float) (cg.time - ci->invulnerabilityStartTime) / 250;
		} else if (cg.time - ci->invulnerabilityStopTime < 250) {
			head.radius = (float) (250 - (cg.time - ci->invulnerabilityStopTime)) / 250;
		} else {
			head.radius = 1;
		}
		VectorSet(head.axis[0], head.radius, 0, 0);
		VectorSet(head.axis[1], 0, head.radius, 0);
		VectorSet(head.axis[2], 0, 0, head.radius);
		trap_R_AddRefEntityToScene(&head);
	}

	torso.skinNum = cg.time - ci->medkitUsageTime;
	if (ci->medkitUsageTime && torso.skinNum < 500) {

		head.hModel = cgs.media.medkitUsageModel;
		head.frame = 0;
		head.oldframe = 0;
		head.customSkin = 0;
		// always draw
		head.renderfx &= ~RF_THIRD_PERSON;
		VectorClear(torso.axis[1]);
		AnglesToAxis(torso.axis[1], head.axis);
		VectorCopy(cent->lerpOrigin, head.origin);
		head.origin[2] += -24 + (float) torso.skinNum * 80 / 500;
		if (torso.skinNum > 400) {
			head.radius = (float) (torso.skinNum - 1000) * 0xff / 100;
			head.shaderRGBA[0] = 0xff - head.radius;
			head.shaderRGBA[1] = 0xff - head.radius;
			head.shaderRGBA[2] = 0xff - head.radius;
			head.shaderRGBA[3] = 0xff - head.radius;
		} else {
			head.shaderRGBA[0] = 0xff;
			head.shaderRGBA[1] = 0xff;
			head.shaderRGBA[2] = 0xff;
			head.shaderRGBA[3] = 0xff;
		}
		trap_R_AddRefEntityToScene(&head);
	}
}


//=====================================================================

/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
 */
void CG_ResetPlayerEntity(centity_t *cent) {
	cent->errorTime = -99999; // guarantee no error decay added
	cent->extrapolated = qfalse;

	CG_ClearLerpFrame(&cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.legs, cent->currentState.legsAnim);
	CG_ClearLerpFrame(&cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.torso, cent->currentState.torsoAnim);

	BG_EvaluateTrajectory(&cent->currentState.pos, cg.time, cent->lerpOrigin);
	BG_EvaluateTrajectory(&cent->currentState.apos, cg.time, cent->lerpAngles);

	VectorCopy(cent->lerpOrigin, cent->rawOrigin);
	VectorCopy(cent->lerpAngles, cent->rawAngles);

	memset(&cent->pe.legs, 0, sizeof ( cent->pe.legs));
	cent->pe.legs.yawAngle = cent->rawAngles[YAW];
	cent->pe.legs.yawing = qfalse;
	cent->pe.legs.pitchAngle = 0;
	cent->pe.legs.pitching = qfalse;

	memset(&cent->pe.torso, 0, sizeof ( cent->pe.legs));
	cent->pe.torso.yawAngle = cent->rawAngles[YAW];
	cent->pe.torso.yawing = qfalse;
	cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
	cent->pe.torso.pitching = qfalse;

	memset(&cent->pe.head, 0, sizeof ( cent->pe.head));
	cent->pe.head.yawAngle = cent->rawAngles[YAW];
	cent->pe.head.yawing = qfalse;
	cent->pe.head.pitchAngle = cent->rawAngles[PITCH];
	cent->pe.head.pitching = qfalse;



	if (cg_debugPosition.integer) {
		CG_Printf("%i ResetPlayerEntity yaw=%f\n", cent->currentState.number, cent->pe.torso.yawAngle);
	}
}


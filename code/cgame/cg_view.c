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
// cg_view.c -- setup all the parameters (position, angle, etc)
// for a 3D rendering
#include "cg_local.h"


/*
=============================================================================

  MODEL TESTING

The viewthing and gun positioning tools from Q2 have been integrated and
enhanced into a single model testing facility.

Model viewing can begin with either "testmodel <modelname>" or "testgun <modelname>".

The names must be the full pathname after the basedir, like 
"models/weapons/v_launch/tris.md3" or "players/male/tris.md3"

Testmodel will create a fake entity 100 units in front of the current view
position, directly facing the viewer.  It will remain immobile, so you can
move around it to view it from different angles.

Testgun will cause the model to follow the player around and supress the real
view weapon model.  The default frame 0 of most guns is completely off screen,
so you will probably have to cycle a couple frames to see it.

"nextframe", "prevframe", "nextskin", and "prevskin" commands will change the
frame or skin of the testmodel.  These are bound to F5, F6, F7, and F8 in
q3default.cfg.

If a gun is being tested, the "gun_x", "gun_y", and "gun_z" variables will let
you adjust the positioning.

Note that none of the model testing features update while the game is paused, so
it may be convenient to test with deathmatch set to 1 so that bringing down the
console doesn't pause the game.

=============================================================================
*/

/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void CG_TestModel_f (void) {
	vec3_t		angles;

	cg.testGun = qfalse;
	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	if ( trap_Argc() < 2 ) {
		return;
	}

	Q_strncpyz (cg.testModelName, CG_Argv( 1 ), MAX_QPATH );
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );

	if ( trap_Argc() == 3 ) {
		cg.testModelEntity.backlerp = atof( CG_Argv( 2 ) );
		cg.testModelEntity.frame = 1;
		cg.testModelEntity.oldframe = 0;
	}
	if (! cg.testModelEntity.hModel ) {
		CG_Printf( "Can't register model\n" );
		return;
	}

	VectorMA( cg.refdef.vieworg, 100, cg.refdef.viewaxis[0], cg.testModelEntity.origin );

	angles[PITCH] = 0;
	angles[YAW] = 180 + cg.refdefViewAngles[1];
	angles[ROLL] = 0;

	AnglesToAxis( angles, cg.testModelEntity.axis );
}

/*
=================
CG_TestGun_f

Replaces the current view weapon with the given model
=================
*/
void CG_TestGun_f (void) {
	CG_TestModel_f();

	if ( !cg.testModelEntity.hModel ) {
		return;
	}

	cg.testGun = qtrue;
	cg.testModelEntity.renderfx = RF_MINLIGHT | RF_DEPTHHACK | RF_FIRST_PERSON;
}


void CG_TestModelNextFrame_f (void) {
	cg.testModelEntity.frame++;
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelPrevFrame_f (void) {
	cg.testModelEntity.frame--;
	if ( cg.testModelEntity.frame < 0 ) {
		cg.testModelEntity.frame = 0;
	}
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelNextSkin_f (void) {
	cg.testModelEntity.skinNum++;
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

void CG_TestModelPrevSkin_f (void) {
	cg.testModelEntity.skinNum--;
	if ( cg.testModelEntity.skinNum < 0 ) {
		cg.testModelEntity.skinNum = 0;
	}
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

static void CG_AddTestModel (void) {
	int		i;

	// re-register the model, because the level may have changed
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );
	if (! cg.testModelEntity.hModel ) {
		CG_Printf ("Can't register model\n");
		return;
	}

	// if testing a gun, set the origin reletive to the view origin
	if ( cg.testGun ) {
		VectorCopy( cg.refdef.vieworg, cg.testModelEntity.origin );
		VectorCopy( cg.refdef.viewaxis[0], cg.testModelEntity.axis[0] );
		VectorCopy( cg.refdef.viewaxis[1], cg.testModelEntity.axis[1] );
		VectorCopy( cg.refdef.viewaxis[2], cg.testModelEntity.axis[2] );

		// allow the position to be adjusted
		for (i=0 ; i<3 ; i++) {
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[0][i] * cg_gun_x.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[1][i] * cg_gun_y.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[2][i] * cg_gun_z.value;
		}
	}

	trap_R_AddRefEntityToScene( &cg.testModelEntity );
}



//============================================================================


/*
=================
CG_CalcVrect

Sets the coordinates of the rendered window
=================
*/
static void CG_CalcVrect (void) {
	int		size;
	int		size2;

	// the intermission should allways be full screen
	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		size = 100;
	} else {
		// bound normal viewsize
		if (cg_viewsize.integer < 30) {
			trap_Cvar_Set ("cg_viewsize","30");
			size = 30;
		} else if (cg_viewsize.integer > 120) {
			trap_Cvar_Set ("cg_viewsize","120");	// leilei - increased to 120 for retro sbar disabling
			size = 120;
		} else {
			size = cg_viewsize.integer;
		}

	}

	size2 = size;
	if (size>100){
		size = 100;	// leilei - size should actually be normal...
	}
	cg.refdef.width = cgs.glconfig.vidWidth*size/100;
	cg.refdef.width &= ~1;

	cg.refdef.height = cgs.glconfig.vidHeight*size/100;
	cg.refdef.height &= ~1;

	cg.refdef.x = (cgs.glconfig.vidWidth - cg.refdef.width)/2;
	cg.refdef.y = (cgs.glconfig.vidHeight - cg.refdef.height)/2;

	// leilei - nudge
		if (cg_viewnudge.integer) {
			int nudged = 0;

			if (size2 < 110) {
				nudged = 48;
			}
			else if (size2 < 120) {
				nudged = 24;
			}


			nudged = nudged * (cgs.glconfig.vidHeight / 480.0);
			cg.refdef.y = ( cgs.glconfig.vidHeight  - cg.refdef.height) /2 - nudged;

		}
}

//==============================================================================

// leilei - eyes hack

extern vec3_t headpos;
extern vec3_t headang;

// this causes a compiler bug on mac MrC compiler
static void CG_StepOffset( void ) {
	int		timeDelta;

	// smooth out stair climbing
	timeDelta = cg.time - cg.stepTime;
	if ( timeDelta < STEP_TIME ) {
		cg.refdef.vieworg[2] -= cg.stepChange
			* (STEP_TIME - timeDelta) / STEP_TIME;
	}
}

/*
===============
CG_OffsetThirdPersonView

===============
*/
static void CG_OffsetThirdPersonView( void ) {
        vec3_t		forward, right, up, view;
        trace_t		trace;
        static vec3_t	mins;
	static vec3_t	maxs;
	float		forwardScale, sideScale;
	float		range = cg_thirdPersonRange.value;
        float           fOldz = cg.refdef.vieworg[2];

        cg.refdef.vieworg[2] += cg.predictedPlayerState.viewheight;

        //sideScale = cg.refdef.vieworg[2];

        up[2] = cg.time - cg.duckTime;
	if ( up[2] < DUCK_TIME) {
		cg.refdef.vieworg[2] -= cg.duckChange * (DUCK_TIME - up[2]) / DUCK_TIME;
	}

        // if dead, look at killer
	if (cg.predictedPlayerState.stats[STAT_HEALTH] <= 0) {
                if (!CG_IsARoundBasedGametype(cgs.gametype)) {
                        cg.refdefViewAngles[YAW] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];
                }
	} else {
                cg.refdefViewAngles[YAW] += cg_thirdPersonAngle.value;
        }

        AngleVectors( cg.refdefViewAngles, forward, right, up );

        VectorCopy( cg.refdef.vieworg, view );

        if (cg.predictedPlayerState.powerups[PW_FLIGHT]) {
                forwardScale = 2.0f;
                sideScale = 0.0f;
                view[2] += 24.0f;
        } else {
                forwardScale = 1 + cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[0];  // cos(0) = 1
                sideScale = 0 - cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[1];    //sin(0) = 0 //- cg_leiDebug.value;
                //view[2] += 8;    // Mix3r_Durachok - it was 8, 15 by default in cg_players.c
                view[2] += cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[2];
        }

        mins[2] = view[2];

	VectorMA( view, -range * forwardScale, forward, view );

        if (sideScale == 0) {
                mins[0] = 45; // head up for look down when centered model view used (flight, arachnotron model)
                if (cg.refdefViewAngles[PITCH] > mins[0]) {
                        mins[1] = (cg.refdefViewAngles[PITCH] - mins[0]) * 1.15;
                        VectorMA( view, mins[1], up, view );
                } else {
                        if (view[2] < mins[2] - 25) {
                                view[2] = mins[2] - 25;
                        }
                }
        } else {
                VectorMA( view, -range * sideScale, right, view );
        }

        // Mix3r_Durachok: trace obstacles for camera:

        maxs[0] = maxs[1] = maxs[2] = 13.97;
        mins[0] = mins[1] = mins[2] = -maxs[0];

        // trace rear obstacle
        CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );
        if ( trace.fraction != 1.0 ) {
                VectorCopy( trace.endpos, view );
        }

        //CG_Printf("dist: %.2f \n", Distance( view, cg.refdef.vieworg ));
        //CG_Printf("pitch: %.2f \n", cg.refdefViewAngles[PITCH]);

        sideScale = Distance( view, cg.refdef.vieworg );
        forwardScale = range * forwardScale * 0.5;
        if (sideScale < forwardScale) {
                view[2] += (forwardScale - sideScale) * 0.58; //0.42;
        }

        // trace ceiling
        maxs[0] = maxs[1] = maxs[2] = 5.215; //4.1;
        mins[0] = mins[1] = mins[2] = -maxs[0];
        cg.refdef.vieworg[2] -= 8;
        CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );
        cg.refdef.vieworg[2] += 8;
        if ( trace.fraction != 1.0 ) {
                sideScale = Distance( trace.endpos, cg.refdef.vieworg );
                if ((cg.predictedPlayerState.powerups[PW_FLIGHT] && sideScale < 45) || (cg.submerged && sideScale < 19)) {
                        cg.renderingThirdPerson = qfalse;
                        cg.refdef.vieworg[2] = fOldz;
                        return;
                }
                VectorCopy( trace.endpos, cg.refdef.vieworg );
        } else {
                VectorCopy( view, cg.refdef.vieworg );
        }

        CG_StepOffset();
}

/*
===============
CG_OffsetFirstPersonView

===============
*/
static void CG_OffsetFirstPersonView( void ) {
	float			*origin;
	float			*angles;
	float			delta;
	vec3_t			predictedVelocity;
	
	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		return;
	}

	origin = cg.refdef.vieworg;
	angles = cg.refdefViewAngles;

	// if dead, fix the angle and don't add any kick
	if ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
		angles[ROLL] = 40;
		angles[PITCH] = -15;
		angles[YAW] = cg.snap->ps.stats[STAT_DEAD_YAW];
		origin[2] += cg.predictedPlayerState.viewheight;
		return;
	}

        //Mix3r_Durachok: bob duration resolving, depending on local pawn anim
        //CG_Printf("legs anim: %i \n", (cg.predictedPlayerState.legsAnim & ~ANIM_TOGGLEBIT));
        switch((cg.predictedPlayerState.legsAnim & ~ANIM_TOGGLEBIT)) {
            case LEGS_RUN:
            case LEGS_BACK:
            case LEGS_STRAFE_LEFT:
            case LEGS_STRAFE_RIGHT:
            case LEGS_WALK:
            case LEGS_BACKWALK:
            case LEGS_WALKCR:
            case LEGS_BACKCR:
                cg.iBobDecay = (cg.predictedPlayerState.legsAnim & ~ANIM_TOGGLEBIT);
                cg.iBobDuration = cgs.clientinfo[ cg.predictedPlayerState.clientNum ].animations[cg.iBobDecay].initialLerp * cgs.clientinfo[ cg.predictedPlayerState.clientNum ].animations[cg.iBobDecay].numFrames;
                if (cg.predictedPlayerState.powerups[PW_HASTE]) {
                    cg.iBobDuration *= 0.666666;
                    delta = (cg.predictedPlayerState.powerups[PW_HASTE]-cg.time) / 1000.0f;
                    if (delta > 1) {
                        delta = 1;
                    }
                } else {
                    delta = 1;
                }
                switch(cg.iBobDecay) {
                    case LEGS_WALK:
                    case LEGS_BACKWALK:
                    case LEGS_WALKCR:
                    case LEGS_BACKCR:
                        cg.iBobDecay = cg.iBobDuration * delta;
                        break;
                    default:
                        cg.iBobDecay = cg.iBobDuration * delta;
                        break;
                    // end cases
                }
                break;
            // end cases
        }

	// add angles based on weapon kick
	VectorAdd (angles, cg.kick_angles, angles);

	// add angles based on damage kick
	if ( cg.damageTime && !CG_IsARoundBasedGametype(cgs.gametype)) {
		predictedVelocity[0] = cg.time - cg.damageTime;
		if ( predictedVelocity[0] < DAMAGE_DEFLECT_TIME ) {
			predictedVelocity[0] /= DAMAGE_DEFLECT_TIME;
			angles[PITCH] += predictedVelocity[0] * cg.v_dmg_pitch * cg_kickScale.value;
			angles[ROLL] += predictedVelocity[0] * cg.v_dmg_roll * cg_kickScale.value;
		} else {
			predictedVelocity[0] = 1.0 - ( predictedVelocity[0] - DAMAGE_DEFLECT_TIME ) / DAMAGE_RETURN_TIME;
			if ( predictedVelocity[0] > 0 ) {
				angles[PITCH] += predictedVelocity[0] * cg.v_dmg_pitch * cg_kickScale.value;
				angles[ROLL] += predictedVelocity[0] * cg.v_dmg_roll * cg_kickScale.value;
			}
		}
	}

	// add pitch based on fall kick
#if 0
	predictedVelocity[0] = ( cg.time - cg.landTime) / FALL_TIME;
	if (predictedVelocity[0] < 0)
		predictedVelocity[0] = 0;
	angles[PITCH] += predictedVelocity[0] * cg.fall_value;
#endif

	// add angles based on velocity
	VectorCopy( cg.predictedPlayerState.velocity, predictedVelocity );

	delta = DotProduct ( predictedVelocity, cg.refdef.viewaxis[0]);
	angles[PITCH] += delta * cg_runpitch.value;
	
	delta = DotProduct ( predictedVelocity, cg.refdef.viewaxis[1]);
	angles[ROLL] -= delta * cg_runroll.value;

	// add angles based on bob

	if ( cg_bob.integer ) {
            if ( cg_bob.integer == 6 ) { // leilei - sweeney bob
		vec3_t		forward, right, up;
		predictedVelocity[1] = cg.xyspeed;
		if (predictedVelocity[1] > 320) {
		        predictedVelocity[1] = 320;
		}
		delta = cg.bobfracsin * 0.006 * predictedVelocity[1];
		AngleVectors (angles, forward, right, up);
		VectorMA (origin, delta, right, origin);
	    } else {
		// make sure the bob is visible even at low speeds
                //CG_Printf("xyspd: %.4f xyspd_lerp: %.4f fracsin_lerp: %.4f \n", cg.xyspeed, cg.xyspeed_lerp, cg.bobfracsin);
		predictedVelocity[1] = cg.xyspeed > 200 ? cg.xyspeed : 200;

		predictedVelocity[2] = cg.iBobDuration+0.0000001f;
                delta = cg.iBobDecay/predictedVelocity[2];
                if (delta < 0) {
                        delta = 0;
                }
                if (cg.bobfracsin > 0) {
                        predictedVelocity[2] = cg.bobfracsin;
                } else {
                        predictedVelocity[2] = -cg.bobfracsin;
                }
                predictedVelocity[2] = predictedVelocity[2] * 2 - 1; //fit to -1:1 range
                delta = predictedVelocity[2] * cg_bobpitch.value * 0.25 * delta * predictedVelocity[1]; // twice as fast as cg.bobfracsin


		if (cg.predictedPlayerState.pm_flags & PMF_DUCKED) {
			delta *= 3; // crouching
		}
		// leilei - no pitch for 3 or 4
		if ( cg_bob.integer == 1 || cg_bob.integer == 2 ) {
			angles[PITCH] += delta;
		}
		delta = cg.bobfracsin * cg_bobroll.value * predictedVelocity[1];
		if (cg.predictedPlayerState.pm_flags & PMF_DUCKED) {
			delta *= 3; // crouching accentuates roll
		}
		// leilei - no roll for 2 or 4
		if ( cg_bob.integer == 1 || cg_bob.integer == 3 || cg_bob.integer == 5 ) {
			angles[ROLL] += delta;
		}
            }
            predictedVelocity[2] = cg.bobfracsin * cg.xyspeed * cg_bobup.value;
	    if (predictedVelocity[2] > 6) {
	        predictedVelocity[2] = 6;
	    }
	    origin[2] += predictedVelocity[2];
	}

//===================================

	// add view height
	origin[2] += cg.predictedPlayerState.viewheight;

	// smooth out duck height changes
	predictedVelocity[2] = cg.time - cg.duckTime;
	if ( predictedVelocity[2] < DUCK_TIME) {
	        cg.refdef.vieworg[2] -= cg.duckChange * (DUCK_TIME - predictedVelocity[2]) / DUCK_TIME;
	}

	// add fall height
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		predictedVelocity[0] = delta / LAND_DEFLECT_TIME;
                if (cg.landChange > 0) {
                        predictedVelocity[0] = -predictedVelocity[0];
                }
		cg.refdef.vieworg[2] += cg.landChange * predictedVelocity[0];
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		delta -= LAND_DEFLECT_TIME;
		predictedVelocity[0] = 1.0 - ( delta / LAND_RETURN_TIME );
                if (cg.landChange > 0) {
                        predictedVelocity[0] = -predictedVelocity[0];
                }
		cg.refdef.vieworg[2] += cg.landChange * predictedVelocity[0];
	}

	// add step offset
	CG_StepOffset();

	// add kick offset

	VectorAdd (origin, cg.kick_origin, origin);

        //CG_Printf("vieworg fp: %.2f \n", cg.refdef.vieworg[2] );

	// pivot the eye based on a neck length
#if 0
	{
        #define	NECK_LENGTH		8
	vec3_t			forward, up;
 
	cg.refdef.vieworg[2] -= NECK_LENGTH;
	AngleVectors( cg.refdefViewAngles, forward, NULL, up );
	VectorMA( cg.refdef.vieworg, 3, forward, cg.refdef.vieworg );
	VectorMA( cg.refdef.vieworg, NECK_LENGTH, up, cg.refdef.vieworg );
	}
#endif
}

//======================================================================

void CG_ZoomDown_f( void ) { 
	if ( cg.zoomed ) {
		return;
	} else if (cg.predictedPlayerState.weapon == 1) {
                //Mix3r_Durachok gauntlet block stuff
                if (cg.time - cg.landTime > (LAND_DEFLECT_TIME + LAND_RETURN_TIME)) {
                        cg.landChange = 10;
                        cg.landTime = CG_CrosshairPlayer();
                        if (cg.landTime == -1) {
                        } else {
		                trap_SendClientCommand(va("vtell %i #", cg.landTime));
	                }
		        cg.landTime = cg.time;
                }
                return;
        } else if (cg.centerPrintLines >= 999) {
                return;
        }
	cg.zoomed = qtrue;
	cg.zoomTime = cg.time;
}

void CG_ZoomUp_f( void ) { 
	if ( !cg.zoomed ) {
		return;
	}
	cg.zoomed = qfalse;
	cg.zoomTime = cg.time;
}


/*
====================
CG_CalcFov

Fixed fov at intermissions, otherwise account for fov variable and zooms.
====================
*/
#define	WAVE_AMPLITUDE	1
#define	WAVE_FREQUENCY	0.4

static int CG_CalcFov( void ) {
	int	contents;
	float	fov_x, fov_y;
	float	zoomFov;
	//int	inwater;

	if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
                fov_x = 90;
        } else if (cg.centerPrintLines >= 999) {
		// if in intermission, use a fixed value
		fov_x = 115;
	} else {
		// user selectable
		if ( cgs.dmflags & DF_FIXED_FOV ) {
			// dmflag to prevent wide fov for all clients
			fov_x = 115;
		} else {
			fov_x = cg_fov.value;
			if ( fov_x < 1 ) {
				fov_x = 1;
			} 
			else if ( fov_x > 160 ) {
				fov_x = 160;
			}
			if( (cgs.videoflags & VF_LOCK_CVARS_BASIC) && fov_x>140 ) {
				fov_x = 140;
			}

		}

		if ( cgs.dmflags & DF_FIXED_FOV ) {
			// dmflag to prevent wide fov for all clients
			zoomFov = 22.5;
		} 
		else {
			// account for zooms
			zoomFov = cg_zoomFov.value;
			if ( zoomFov < 1 ) {
				zoomFov = 1;
			} 
			else if ( zoomFov > 160 ) {
				zoomFov = 160;
			}

			if( (cgs.videoflags & VF_LOCK_CVARS_BASIC) && zoomFov>140 ) {
				zoomFov = 140;
			}
		}
                fov_y = ( cg.time - cg.zoomTime ) / (float)ZOOM_TIME;
		if ( cg.zoomed ) {

			if ( fov_y > 1.0 ) {
				fov_x = zoomFov;
			} else {
				fov_x = fov_x + fov_y * ( zoomFov - fov_x );
			}
		} else {
			if ( fov_y > 1.0 ) {
			} else {
				fov_x = zoomFov + fov_y * ( fov_x - zoomFov );
			}
		}
	}

	zoomFov = cg.refdef.width / tan( fov_x / 360 * M_PI );
	fov_y = atan2( cg.refdef.height, zoomFov );
	fov_y = fov_y * 360 / M_PI;

	// warp if underwater
	contents = CG_PointContents( cg.refdef.vieworg, -1 );
	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ){
		zoomFov = cg.time / 1000.0 * WAVE_FREQUENCY * M_PI * 2;
		zoomFov = WAVE_AMPLITUDE * sin( zoomFov );
		fov_x += zoomFov;
		fov_y -= zoomFov;
		//inwater = qtrue;
                cg.submerged = qtrue;
	} else {
		//inwater = qfalse;
                cg.submerged = qfalse;
	}

	// set it
	cg.refdef.fov_x = fov_x;
	cg.refdef.fov_y = fov_y;

	if ( !cg.zoomed ) {
		cg.zoomSensitivity = 1;
	} 
	else {
		cg.zoomSensitivity = cg.refdef.fov_y / 75.0;
	}

	return cg.submerged;
}



/*
===============
CG_DamageBlendBlob

===============
*/
static void CG_DamageBlendBlob( void ) {
	if ( cg.damageValue ) {
        int			t;
	int			maxTime;
	refEntity_t		ent;

	// ragePro systems can't fade blends, so don't obscure the screen
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		return;
	}

	maxTime = DAMAGE_TIME;
	t = cg.time - cg.damageTime;
	if ( t <= 0 || t >= maxTime ) {
		return;
	}


	memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_SPRITE;
	ent.renderfx = RF_FIRST_PERSON;

	VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], ent.origin );
	VectorMA( ent.origin, cg.damageX * -8, cg.refdef.viewaxis[1], ent.origin );
	VectorMA( ent.origin, cg.damageY * 8, cg.refdef.viewaxis[2], ent.origin );

	ent.radius = cg.damageValue * 3;
	ent.customShader = cgs.media.viewBloodShader;
	ent.shaderRGBA[0] = 255;
	ent.shaderRGBA[1] = 255;
	ent.shaderRGBA[2] = 255;
	ent.shaderRGBA[3] = 200 * ( 1.0 - ((float)t / maxTime) );
	trap_R_AddRefEntityToScene( &ent );
        }
}


/*
===============
CG_CalcViewValues

Sets cg.refdef view values
===============
*/
static int CG_CalcViewValues( void ) {
	playerState_t	*ps;

	memset( &cg.refdef, 0, sizeof( cg.refdef ) );

	// strings for in game rendering
	// Q_strncpyz( cg.refdef.text[0], "Park Ranger", sizeof(cg.refdef.text[0]) );
	// Q_strncpyz( cg.refdef.text[1], "19", sizeof(cg.refdef.text[1]) );

	// calculate size of 3D view
	CG_CalcVrect();

	ps = &cg.predictedPlayerState;

	// intermission view
	if ( ps->pm_type == PM_INTERMISSION ) {
		VectorCopy( ps->origin, cg.refdef.vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
		return CG_CalcFov();
	}

        cg.xyspeed = sqrt( ps->velocity[0] * ps->velocity[0] + ps->velocity[1] * ps->velocity[1] );
        cg.xyspeed_lerp = cg.xyspeed_lerp + (cg.xyspeed-cg.xyspeed_lerp) * cg.frametime * 0.009;


        // Mix3r_Durachok: resolving bob phase with linear interpolation between -1:1
        // let's borrow cg.refdef.vieworg storage, because it's values gonna be replaced soon anyway :)

        if (cg.iBobDuration != 0) {
            cg.refdef.vieworg[0] = (float)cg.frametime / cg.iBobDuration;
            cg.fBobFraction += cg.refdef.vieworg[0];

            //CG_Printf("div mod: %.5f \n", cg.refdef.vieworg[0]+cg.fBobOffset);
            cg.refdef.vieworg[1] = (float)cg.iBobDecay / cg.iBobDuration;
            cg.iBobDecay -= cg.frametime;
            if (cg.refdef.vieworg[1] < 0) {
                    cg.refdef.vieworg[1] = 0;
            }

            //cg.refdef.vieworg[1] = sin(cg.fBobFraction * M_PI * 2) * cg.refdef.vieworg[1];
            //cg.bobfracsin = cg.bobfracsin + (cg.refdef.vieworg[1]-cg.bobfracsin) * cg.frametime * 0.15778;  //0.15778;
            cg.bobfracsin = sin(cg.fBobFraction * M_PI * 2) * cg.refdef.vieworg[1];
        }

	VectorCopy( ps->origin, cg.refdef.vieworg ); // replacement happens here :)
	VectorCopy( ps->viewangles, cg.refdefViewAngles );

	if (cg_cameraOrbit.integer) {
                if (cg_cameraOrbit.integer == 999) {
                        // Mix3r_Durachok: third person offset developer adjustment for 3rd person player model
                        // start a map as devmap [map] console command, then type cg_cameraOrbit 999, go third person
                        // then use cg_gun_x, cg_gun_y, cg_gun_z values to adjust 3rd person cam. then write discovered
                        // values into the model's animation.cfg file as following line on start of file:
                        // eyes x y z
                        // where x y z are corresponding cg_gun_* (_x _y _z) values just discovered during adjust
                        cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[0] = cg_gun_x.value;
                        cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[1] = cg_gun_y.value;
                        cgs.clientinfo[ cg.predictedPlayerState.clientNum ].eyepos[2] = cg_gun_z.value;
		} else if (cg.time > cg.nextOrbitTime) {
			cg.nextOrbitTime = cg.time + cg_cameraOrbitDelay.integer;
			cg_thirdPersonAngle.value += cg_cameraOrbit.value;
		}
	}
	// add error decay
	if ( cg_errorDecay.value > 0 ) {
		int		t;
		float	f;

		t = cg.time - cg.predictedErrorTime;
		f = ( cg_errorDecay.value - t ) / cg_errorDecay.value;
		if ( f > 0 && f < 1 ) {
			VectorMA( cg.refdef.vieworg, f, cg.predictedError, cg.refdef.vieworg );
		} else {
			cg.predictedErrorTime = 0;
		}
	}

        if (cg.centerPrintLines >= 999) {
                char    *start;
                char    *token;
                int i;
                vec3_t  vViewpos, vLookangle;
                start = cg.centerPrint+4;
                vLookangle[2] = 0.0f;
                for (i = 0; i < 5; i++) {
		        token = COM_Parse(&start);
			if ( !token[0] ) {
			        break;
			}
                        if (i < 3) {
                                vViewpos[i] = atof(token);
                        } else {
                                vLookangle[i-3] = atof(token);
                        }
		}
                VectorCopy( vLookangle, cg.refdefViewAngles );

                // Mix3r_Durachok: let's implement camera movement:
                token = COM_Parse(&start);
                if (token[0]) {
                        i = atoi(token);
                        token = COM_Parse(&start);
                        if (token[0] && i > 0) {
                                if (token[0] == 'B') {
                                        vLookangle[YAW] += 180; // dolly backward
                                } else if (token[0] == 'R') {
                                        vLookangle[YAW] += 270; // dolly left
                                } else if (token[0] == 'L') {
                                        vLookangle[YAW] += 90; // dolly right
                                } else if (token[0] == 'U') {
                                        vLookangle[PITCH] = -90; // crane up
                                        vLookangle[YAW] = 0;
                                } else if (token[0] == 'D') {
                                        vLookangle[PITCH] = 90; // crane down
                                        vLookangle[YAW] = 0;
                                        // use W or another letter to dolly forward
                                } else if (token[0] == 'P') {
                                        // followcam (fixed look angle) feature
                                        vViewpos[0]+=cg.refdef.vieworg[0];
                                        vViewpos[1]+=cg.refdef.vieworg[1];
                                        vViewpos[2]+=cg.refdef.vieworg[2];
                                        i = 0;
                                        trap_SendConsoleCommand("centerview\n");
                                }
                                if (i) {
                                        AngleVectors( vLookangle, cg.refdef.vieworg, NULL, NULL );
                                        VectorMA( vViewpos, i*((cg.time - cg.centerPrintY)*0.001), cg.refdef.vieworg, vViewpos );
                                }
                        }
                }

                VectorCopy( vViewpos, cg.refdef.vieworg );
	} else if ( cg.renderingThirdPerson && !cg.zoomed) {
		// back away from character. Mix3r_Durachok: third person zoom shouldn't show person
		CG_OffsetThirdPersonView();
                if (!cg.renderingThirdPerson) {
                        CG_OffsetFirstPersonView();
                }
	} else {
		// offset for local bobbing and kicks
		CG_OffsetFirstPersonView();
	}

	// position eye reletive to origin
	AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );

	if ( cg.hyperspace ) {
		cg.refdef.rdflags |= RDF_NOWORLDMODEL | RDF_HYPERSPACE;
	}

	// field of view
	return CG_CalcFov();
}


/*
=====================
CG_PowerupTimerSounds
=====================
*/
static void CG_PowerupTimerSounds( void ) {
	int		i;
	int		t;

	// powerup timers going away
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		t = cg.snap->ps.powerups[i];
		if ( t <= cg.time ) {
			continue;
		}
		if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
			continue;
		}
		if ( ( t - cg.time ) / POWERUP_BLINK_TIME != ( t - cg.oldTime ) / POWERUP_BLINK_TIME ) {
			trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_ITEM, cgs.media.wearOffSound );
		}
	}
}

/*
=====================
CG_AddBufferedSound
=====================
*/
void CG_AddBufferedSound( sfxHandle_t sfx ) {
	if ( !sfx )
		return;
	cg.soundBuffer[cg.soundBufferIn] = sfx;
	cg.soundBufferIn = (cg.soundBufferIn + 1) % MAX_SOUNDBUFFER;
	if (cg.soundBufferIn == cg.soundBufferOut) {
		cg.soundBufferOut++;
	}
}

/*
=====================
CG_PlayBufferedSounds
=====================
*/
static void CG_PlayBufferedSounds( void ) {
	if ( cg.soundTime < cg.time ) {
		if (cg.soundBufferOut != cg.soundBufferIn && cg.soundBuffer[cg.soundBufferOut]) {
			trap_S_StartLocalSound(cg.soundBuffer[cg.soundBufferOut], CHAN_ANNOUNCER);
			cg.soundBuffer[cg.soundBufferOut] = 0;
			cg.soundBufferOut = (cg.soundBufferOut + 1) % MAX_SOUNDBUFFER;
			cg.soundTime = cg.time + 750;
		}
	}
}

//=========================================================================

/*
=================
CG_DrawActiveFrame

Generates and draws a game scene and status information at the given time.
=================
*/
void CG_DrawActiveFrame( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback ) {
	int		inwater;

	cg.time = serverTime;
	cg.demoPlayback = demoPlayback;

	// update cvars
	CG_UpdateCvars();

	// if we are only updating the screen as a loading
	// pacifier, don't even try to read snapshots
	if ( cg.infoScreenText[0] != 0 ) {
// loadingscreen
#ifdef SCRIPTHUD
		CG_DrawLoadingScreen( );
#else
		CG_DrawInformation();
#endif
// end loadingscreen
		return;
	}

	// any looped sounds will be respecified as entities
	// are added to the render list
	trap_S_ClearLoopingSounds(qfalse);

	// clear all the render lists
	trap_R_ClearScene();

	// set up cg.snap and possibly cg.nextSnap
	CG_ProcessSnapshots();

	// if we haven't received any snapshots yet, all
	// we can draw is the information screen
	if ( !cg.snap || ( cg.snap->snapFlags & SNAPFLAG_NOT_ACTIVE ) ) {
// loadingscreen
#ifdef SCRIPTHUD
		CG_DrawLoadingScreen( );
#else
		CG_DrawInformation();
#endif
// end loadingscreen
		return;
	}

	// let the client system know what our weapon and zoom settings are
	trap_SetUserCmdValue( cg.weaponSelect, cg.zoomSensitivity );

	// this counter will be bumped for every valid scene we generate
	cg.clientFrame++;

	// update cg.predictedPlayerState
	CG_PredictPlayerState();

	// decide on third person view

	//if (!cg_deathcam.integer) {
		// leilei - allow first person deathcam
	//	cg.renderingThirdPerson = cg_thirdPerson.integer;
	//} else {
		cg.renderingThirdPerson = cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR && (cg_thirdPerson.integer || cg.snap->ps.stats[STAT_HEALTH] <= 0 || cg.centerPrintLines >= 999);
	//}

	// build cg.refdef
	inwater = CG_CalcViewValues();

	// first person blend blobs, done after AnglesToAxis
	if ( !cg.renderingThirdPerson ) {
		CG_DamageBlendBlob();
	}

	// build the render lists
	if ( !cg.hyperspace ) {
		CG_AddPacketEntities();			// adter calcViewValues, so predicted player state is correct
		CG_AddMarks();
		CG_AddLocalEntities();
	}
	CG_AddViewWeapon( &cg.predictedPlayerState );

	// add buffered sounds
	CG_PlayBufferedSounds();

	// play buffered voice chats
	CG_PlayBufferedVoiceChats();

	// finish up the rest of the refdef
	if ( cg.testModelEntity.hModel ) {
		CG_AddTestModel();
	}
	cg.refdef.time = cg.time;
	memcpy( cg.refdef.areamask, cg.snap->areamask, sizeof( cg.refdef.areamask ) );

	// warning sounds when powerup is wearing off
	CG_PowerupTimerSounds();

	// update audio positions
	trap_S_Respatialize( cg.snap->ps.clientNum, cg.refdef.vieworg, cg.refdef.viewaxis, inwater );

	// make sure the lagometerSample and frame timing isn't done twice when in stereo
	if ( stereoView != STEREO_RIGHT ) {
		cg.frametime = cg.time - cg.oldTime;
		if ( cg.frametime < 0 ) {
			cg.frametime = 0;
		}
		cg.oldTime = cg.time;
		CG_AddLagometerFrameInfo();
	}
	if (cg_timescale.value != cg_timescaleFadeEnd.value) {
		if (cg_timescale.value < cg_timescaleFadeEnd.value) {
			cg_timescale.value += cg_timescaleFadeSpeed.value * ((float)cg.frametime) / 1000;
			if (cg_timescale.value > cg_timescaleFadeEnd.value) {
				cg_timescale.value = cg_timescaleFadeEnd.value;
			}
		}
		else {
			cg_timescale.value -= cg_timescaleFadeSpeed.value * ((float)cg.frametime) / 1000;
			if (cg_timescale.value < cg_timescaleFadeEnd.value) {
				cg_timescale.value = cg_timescaleFadeEnd.value;
			}
		}
		if (cg_timescaleFadeSpeed.value) {
			trap_Cvar_Set("timescale", va("%f", cg_timescale.value));
		}
	}

	// actually issue the rendering calls
	CG_DrawActive( stereoView );

	//if ( cg_stats.integer ) {
	//	CG_Printf( "cg.clientFrame:%i\n", cg.clientFrame );
	//}

}


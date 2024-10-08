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
// cg_weapons.c -- events and effects dealing with weapons
#include "cg_local.h"

/*
==========================
CG_MachineGunEjectBrass
==========================
*/
static void CG_MachineGunEjectBrass( centity_t *cent )
{
	localEntity_t	*le;
	refEntity_t	*re;
	vec3_t		 velocity, xvelocity;
	float		 waterScale = 1.0f;
	vec3_t		 v[3];

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	velocity[0] = 0;
	velocity[1] = -50 + 40 * crandom();
	velocity[2] = 100 + 50 * crandom();

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();

	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - (rand()&15);

	AnglesToAxis( cent->lerpAngles, v );

	VectorCopy( cgs.clientinfo[ cent->currentState.clientNum ].pGunPoint, re->origin );
        VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	AxisCopy( axisDefault, re->axis );
	re->hModel = cgs.media.machinegunBrassModel;

	if (cg_leiBrassNoise.integer)
		le->bounceFactor = 0.6f * waterScale;
	else
		le->bounceFactor = 0.4 * waterScale;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 2;
	le->angles.trDelta[1] = 1;
	le->angles.trDelta[2] = 0;

	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_BRASS;
	le->leMarkType = LEMT_NONE;
}



/*
==========================
CG_ShotgunEjectBrass
==========================
*/
static void CG_ShotgunEjectBrass( centity_t *cent )
{
	localEntity_t	*le;
	refEntity_t	*re;
	vec3_t		 velocity, xvelocity;
	float		 waterScale = 1.0f;
	vec3_t		 v[3];

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	velocity[0] = 0;
	velocity[1] = -50 + 40 * crandom();
	velocity[2] = 100 + 50 * crandom();

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();

	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - (rand()&15);

	AnglesToAxis( cent->lerpAngles, v );

	VectorCopy( cgs.clientinfo[ cent->currentState.clientNum ].pGunPoint, re->origin );
        VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	AxisCopy( axisDefault, re->axis );
	re->hModel = cgs.media.shotgunBrassModel;

	if (cg_leiBrassNoise.integer)
		le->bounceFactor = 0.6f * waterScale;
	else
		le->bounceFactor = 0.4 * waterScale;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 2;
	le->angles.trDelta[1] = 1;
	le->angles.trDelta[2] = 0;

	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_SHELL;
	le->leMarkType = LEMT_NONE;
}


//#ifdef MISSIONPACK
/*
==========================
CG_NailgunEjectBrass
==========================
*/
static void CG_NailgunEjectBrass( centity_t *cent )
{
	localEntity_t	*smoke;
	vec3_t		 origin;
	vec3_t		 up;

        /////////////////////
	VectorCopy( cgs.clientinfo[ cent->currentState.clientNum ].pGunPoint, origin );
        ///////////////////////////

	VectorSet( up, 0, 0, 64 );

	smoke = CG_SmokePuff( origin, up, 32, 1, 1, 1, 0.33f, 700, cg.time, 0, 0, cgs.media.smokePuffShader );
	// use the optimized local entity add
	smoke->leType = LE_SCALE_FADE;
}
//#endif


/*
==========================
CG_RailTrail
==========================
*/
void CG_RailTrail (clientInfo_t *ci, vec3_t start, vec3_t end)
{
	vec3_t axis[36], move, move2, vec, temp;
	float  len;
	int    i, j, skip;

	localEntity_t *le;
	refEntity_t   *re;

#define RADIUS   4
#define ROTATION 1
#define SPACING  5

	//start[2] -= 4;

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + cg_railTrailTime.value;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);

	re->shaderTime = cg.time / 1000.0f;
	re->reType = RT_RAIL_CORE;
	re->customShader = cgs.media.railCoreShader;

	VectorCopy(start, re->origin);
	VectorCopy(end, re->oldorigin);

	re->shaderRGBA[0] = ci->color1[0] * 255;
	re->shaderRGBA[1] = ci->color1[1] * 255;
	re->shaderRGBA[2] = ci->color1[2] * 255;
	re->shaderRGBA[3] = 255;

	le->color[0] = ci->color1[0] * 0.75;
	le->color[1] = ci->color1[1] * 0.75;
	le->color[2] = ci->color1[2] * 0.75;
	le->color[3] = 1.0f;

	AxisClear( re->axis );

	if (cg_oldRail.integer) {
		// nudge down a bit so it isn't exactly in center
		re->origin[2] -= 8;
		re->oldorigin[2] -= 8;

		// leilei - reimplementing the rail discs that were removed in 1.30
		if (cg_oldRail.integer > 1) {

			le = CG_AllocLocalEntity();
			re = &le->refEntity;

			VectorCopy(start, re->origin);
			VectorCopy(end, re->oldorigin);
			le->leType = LE_FADE_RGB;
			le->startTime = cg.time;
			le->endTime = cg.time + cg_railTrailTime.value;
			le->lifeRate = 1.0 / (le->endTime - le->startTime);

			re->shaderTime = cg.time / 1000.0f;
			re->reType = RT_RAIL_RINGS;
			re->customShader = cgs.media.railRingsShader;
			re->shaderRGBA[0] = ci->color1[0] * 255;
			re->shaderRGBA[1] = ci->color1[1] * 255;
			re->shaderRGBA[2] = ci->color1[2] * 255;
			re->shaderRGBA[3] = 255;

			le->color[0] = ci->color1[0] * 0.75;
			le->color[1] = ci->color1[1] * 0.75;
			le->color[2] = ci->color1[2] * 0.75;
			le->color[3] = 1.0f;

			re->origin[2] -= 8;
			re->oldorigin[2] -= 8;

			if (cg_oldRail.integer > 2) {		// use the secondary color instead
				re->shaderRGBA[0] = ci->color2[0] * 255;
				re->shaderRGBA[1] = ci->color2[1] * 255;
				re->shaderRGBA[2] = ci->color2[2] * 255;
				re->shaderRGBA[3] = 255;

				le->color[0] = ci->color2[0] * 0.75;
				le->color[1] = ci->color2[1] * 0.75;
				le->color[2] = ci->color2[2] * 0.75;
				le->color[3] = 1.0f;
			}

		}
		return;
	}

	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);
	PerpendicularVector(temp, vec);
	for (i = 0 ; i < 36; i++) {
		RotatePointAroundVector(axis[i], vec, temp, i * 10);//banshee 2.4 was 10
	}

	VectorMA(move, 20, vec, move);
	VectorScale (vec, SPACING, vec);

	skip = -1;

	j = 18;
	for (i = 0; i < len; i += SPACING) {
		if (i != skip) {
			skip = i + SPACING;
			le = CG_AllocLocalEntity();
			re = &le->refEntity;
			le->leFlags = LEF_PUFF_DONT_SCALE;
			le->leType = LE_MOVE_SCALE_FADE;
			le->startTime = cg.time;
			le->endTime = cg.time + (i>>1) + 600;
			le->lifeRate = 1.0 / (le->endTime - le->startTime);

			re->shaderTime = cg.time / 1000.0f;
			re->reType = RT_SPRITE;
			re->radius = 1.1f;
			re->customShader = cgs.media.railRingsShader;

			re->shaderRGBA[0] = ci->color2[0] * 255;
			re->shaderRGBA[1] = ci->color2[1] * 255;
			re->shaderRGBA[2] = ci->color2[2] * 255;
			re->shaderRGBA[3] = 255;

			le->color[0] = ci->color2[0] * 0.75;
			le->color[1] = ci->color2[1] * 0.75;
			le->color[2] = ci->color2[2] * 0.75;
			le->color[3] = 1.0f;

			le->pos.trType = TR_LINEAR;
			le->pos.trTime = cg.time;

			VectorCopy( move, move2);
			VectorMA(move2, RADIUS , axis[j], move2);
			VectorCopy(move2, le->pos.trBase);

			le->pos.trDelta[0] = axis[j][0]*6;
			le->pos.trDelta[1] = axis[j][1]*6;
			le->pos.trDelta[2] = axis[j][2]*6;
		}

		VectorAdd (move, vec, move);

		j = (j + ROTATION) % 36;
	}
}

/*
==========================
CG_OldRocketTrail	(for the crappy old rocket trail.)
==========================
*/
static void CG_OldRocketTrail( centity_t *ent, const weaponInfo_t *wi )
{
	int		step;
	vec3_t	origin, lastPos;
	int		t;
	int		startTime, contents;
	int		lastContents;
	entityState_t	*es;
	vec3_t	up;
	localEntity_t	*smoke;

	if ( cg_noProjectileTrail.integer ) {
		return;
	}

	up[0] = 0;
	up[1] = 0;
	up[2] = 0;

	step = 50;

	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY ) {
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
	lastContents = CG_PointContents( lastPos, -1 );

	ent->trailTime = cg.time;

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		if ( contents & lastContents & CONTENTS_WATER ) {
			CG_BubbleTrail( lastPos, origin, 8 );
		}
		return;
	}

	for ( ; t <= ent->trailTime ; t += step ) {
		BG_EvaluateTrajectory( &es->pos, t, lastPos );

		smoke = CG_SmokePuff( lastPos, up,
		                      wi->trailRadius,
		                      1, 1, 1, 0.33f,
		                      wi->wiTrailTime,
		                      t,
		                      0,
		                      0,
		                      cgs.media.smokePuffShader );
		// use the optimized local entity add
		smoke->leType = LE_SCALE_FADE;
	}

}

/*
==========================
CG_LeiSmokeTrail
==========================
*/

static void CG_LeiSmokeTrail( centity_t *ent, const weaponInfo_t *wi )
{
	int		step;
	vec3_t	origin, lastPos;
	int		t;
	int		startTime, contents;
	int		lastContents;
	entityState_t	*es;
	vec3_t	up;

	if ( cg_noProjectileTrail.integer ) {
		return;
	}

	up[0] = 5 - 10 * crandom();
	up[1] = 5 - 10 * crandom();
	up[2] = 8 - 5 * crandom();

	step = 18;

	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY ) {
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
	lastContents = CG_PointContents( lastPos, -1 );

	ent->trailTime = cg.time;

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		if ( contents & lastContents & CONTENTS_WATER ) {
			CG_BubbleTrail( lastPos, origin, 8 );
		}
		return;
	}
}


static void CG_LeiPlasmaTrail( centity_t *ent, const weaponInfo_t *wi )
{
	int		step;
	vec3_t	origin, lastPos;
	int		t;
	int		startTime, contents;
	entityState_t	*es;
	vec3_t	up;
	localEntity_t	*smoke;

	if ( cg_noProjectileTrail.integer ) {
		return;
	}

	up[0] = 0;
	up[1] = 0;
	up[2] = 0;

	step = 16;

	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY ) {
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );

	ent->trailTime = cg.time;

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		return;
	}

	for ( ; t <= ent->trailTime ; t += step ) {
		BG_EvaluateTrajectory( &es->pos, t, lastPos );

		smoke = CG_SmokePuff( lastPos, up, 27, 1, 1, 1, 0.9f, wi->wiTrailTime,  t, 0, 0,  cgs.media.lsmkShader1 );
		// use the optimized local entity add
		smoke->leType = LE_SCALE_FADE;
		//smoke->trType = TR_GRAVITY;
	}

}


//#ifdef MISSIONPACK
/*
==========================
CG_PlasmaTrail
==========================
*/
static void CG_NailTrail( centity_t *ent, const weaponInfo_t *wi )
{
	int		step;
	vec3_t	origin, lastPos;
	int		t;
	int		startTime, contents;
	int		lastContents;
	entityState_t	*es;
	vec3_t	up;
	localEntity_t	*smoke;

	if ( cg_noProjectileTrail.integer ) {
		return;
	}

	up[0] = 0;
	up[1] = 0;
	up[2] = 0;

	step = 50;

	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY ) {
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
	lastContents = CG_PointContents( lastPos, -1 );

	ent->trailTime = cg.time;

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		if ( contents & lastContents & CONTENTS_WATER ) {
			CG_BubbleTrail( lastPos, origin, 8 );
		}
		return;
	}

	for ( ; t <= ent->trailTime ; t += step ) {
		BG_EvaluateTrajectory( &es->pos, t, lastPos );

		smoke = CG_SmokePuff( lastPos, up,
		                      wi->trailRadius,
		                      1, 1, 1, 0.33f,
		                      wi->wiTrailTime,
		                      t,
		                      0,
		                      0,
		                      cgs.media.nailPuffShader );
		// use the optimized local entity add
		smoke->leType = LE_SCALE_FADE;
	}

}
//#endif

/*
==========================
CG_NailTrail
==========================
*/
static void CG_OldPlasmaTrail( centity_t *cent, const weaponInfo_t *wi )
{
	localEntity_t	*le;
	refEntity_t		*re;
	entityState_t	*es;
	vec3_t			velocity, xvelocity, origin;
	vec3_t			offset, xoffset;
	vec3_t			v[3];

	float	waterScale = 1.0f;

	if ( cg_noProjectileTrail.integer || cg_oldPlasma.integer ) {
		return;
	}
	es = &cent->currentState;

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	velocity[0] = 60 - 120 * crandom();
	velocity[1] = 40 - 80 * crandom();
	velocity[2] = 100 - 200 * crandom();

	le->leType = LE_MOVE_SCALE_FADE;
	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_NONE;
	le->leMarkType = LEMT_NONE;

	le->startTime = cg.time;
	le->endTime = le->startTime + 600;

	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time;

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 2;
	offset[1] = 2;
	offset[2] = 2;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];

	VectorAdd( origin, xoffset, re->origin );
	VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	AxisCopy( axisDefault, re->axis );
	re->shaderTime = cg.time / 1000.0f;
	re->reType = RT_SPRITE;
	re->radius = 0.25f;
	re->customShader = cgs.media.railRingsShader;
	le->bounceFactor = 0.3f;


	re->shaderRGBA[0] = wi->flashDlightColor[0] * 63;
	re->shaderRGBA[1] = wi->flashDlightColor[1] * 63;
	re->shaderRGBA[2] = wi->flashDlightColor[2] * 63;
	re->shaderRGBA[3] = 63;

	le->color[0] = wi->flashDlightColor[0] * 0.2;
	le->color[1] = wi->flashDlightColor[1] * 0.2;
	le->color[2] = wi->flashDlightColor[2] * 0.2;
	le->color[3] = 0.25f;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 1;
	le->angles.trDelta[1] = 0.5;
	le->angles.trDelta[2] = 0;

}
/*
==========================
CG_GrappleTrail
==========================
*/
void CG_GrappleTrail( centity_t *ent, const weaponInfo_t *wi )
{
	vec3_t	origin;
	entityState_t	*es;
	vec3_t			forward, up;
	refEntity_t		beam;

	es = &ent->currentState;

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	ent->trailTime = cg.time;

	memset( &beam, 0, sizeof( beam ) );
	//FIXME adjust for muzzle position
	VectorCopy ( cg_entities[ ent->currentState.otherEntityNum ].lerpOrigin, beam.origin );
	beam.origin[2] += 26;
	AngleVectors( cg_entities[ ent->currentState.otherEntityNum ].lerpAngles, forward, NULL, up );
	VectorMA( beam.origin, -6, up, beam.origin );
	VectorCopy( origin, beam.oldorigin );

	if (Distance( beam.origin, beam.oldorigin ) < 64 )
		return; // Don't draw if close

	beam.reType = RT_RAIL_CORE;
	beam.customShader = cgs.media.grappleShader;

	AxisClear( beam.axis );
	beam.shaderRGBA[0] = 0xff;
	beam.shaderRGBA[1] = 0xff;
	beam.shaderRGBA[2] = 0xff;
	beam.shaderRGBA[3] = 0xff;
	trap_R_AddRefEntityToScene( &beam );
}

/*
==========================
CG_GrenadeTrail
==========================
*/
// LEILEI enhancment
static void CG_RocketTrail( centity_t *ent, const weaponInfo_t *wi )
{
        //CG_LeiSmokeTrail( ent, wi );
        CG_OldRocketTrail( ent, wi );
}

static void CG_PlasmaTrail( centity_t *ent, const weaponInfo_t *wi )
{
        //CG_LeiPlasmaTrail( ent, wi );
	CG_OldPlasmaTrail( ent, wi );
}


static void CG_GrenadeTrail( centity_t *ent, const weaponInfo_t *wi )
{
	CG_RocketTrail( ent, wi );
}






/*
=================
CG_RegisterWeapon

The server says this item is used on this level
=================
*/
void CG_RegisterWeapon( int weaponNum )
{
	weaponInfo_t	*weaponInfo;
	gitem_t			*item, *ammo;
	char			path[MAX_QPATH];
	vec3_t			mins, maxs;
	int				i;

	weaponInfo = &cg_weapons[weaponNum];

	if ( weaponNum == 0 ) {
		return;
	}

	if ( weaponInfo->registered ) {
		return;
	}

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
	weaponInfo->registered = qtrue;

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType == IT_WEAPON && item->giTag == weaponNum ) {
			weaponInfo->item = item;
			break;
		}
	}
	if ( !item->classname ) {
		CG_Error( "Couldn't find weapon %i", weaponNum );
	}
	CG_RegisterItemVisuals( item - bg_itemlist );

	// load cmodel before model so filecache works
	weaponInfo->weaponModel = trap_R_RegisterModel( item->world_model[0] );

	// calc midpoint for rotation
	trap_R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
	for ( i = 0 ; i < 3 ; i++ ) {
		weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );
	}

	weaponInfo->weaponIcon = trap_R_RegisterShader( item->icon );
	weaponInfo->ammoIcon = trap_R_RegisterShader( item->icon );

	for ( ammo = bg_itemlist + 1 ; ammo->classname ; ammo++ ) {
		if ( ammo->giType == IT_AMMO && ammo->giTag == weaponNum ) {
			break;
		}
	}
	if ( ammo->classname && ammo->world_model[0] ) {
		weaponInfo->ammoModel = trap_R_RegisterModel( ammo->world_model[0] );
	}

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash.md3" );
	weaponInfo->flashModel = trap_R_RegisterModel( path );

	if ( !weaponInfo->flashModel ) {
		weaponInfo->flashModel = trap_R_RegisterModel( "models/weapons2/shotgun/shotgun_flash.md3" );	// default flash
	}

	// leilei - additional flash styles

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash1.md3" );
	weaponInfo->flashModel_type1 = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash2.md3" );
	weaponInfo->flashModel_type2 = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash2a.md3" );
	weaponInfo->flashModel_type2a = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash3.md3" );
	weaponInfo->flashModel_type3 = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash4.md3" );
	weaponInfo->flashModel_type4 = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash5.md3" );
	weaponInfo->flashModel_type5 = trap_R_RegisterModel( path );


	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_flash5a.md3" );
	weaponInfo->flashModel_type5a = trap_R_RegisterModel( path );






	// leilei - revert to default flash model if we really don't have a real one.

	if (!weaponInfo->flashModel_type1) weaponInfo->flashModel_type1 = trap_R_RegisterModel( "models/muzzle/flash1.md3" );
	if (!weaponInfo->flashModel_type2) weaponInfo->flashModel_type2 = trap_R_RegisterModel( "models/muzzle/flash2.md3" ); 
	if (!weaponInfo->flashModel_type2a) weaponInfo->flashModel_type2a = trap_R_RegisterModel( "models/muzzle/flash2a.md3" ); 
	if (!weaponInfo->flashModel_type3) weaponInfo->flashModel_type3 = trap_R_RegisterModel( "models/muzzle/flash3.md3" ); 
	if (!weaponInfo->flashModel_type4) weaponInfo->flashModel_type4 = trap_R_RegisterModel( "models/muzzle/flash4.md3" ); 
	if (!weaponInfo->flashModel_type5) weaponInfo->flashModel_type5 = trap_R_RegisterModel( "models/muzzle/flash5.md3" ); 
	if (!weaponInfo->flashModel_type5a) weaponInfo->flashModel_type5a = trap_R_RegisterModel( "models/muzzle/flash5a.md3" ); 

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_barrel.md3" );
	weaponInfo->barrelModel = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	Q_strcat( path, sizeof(path), "_hand.md3" );
	weaponInfo->handsModel = trap_R_RegisterModel( path );

	if ( !weaponInfo->handsModel ) {
		weaponInfo->handsModel = trap_R_RegisterModel( "models/weapons2/shotgun/shotgun_hand.md3" );
	}

	weaponInfo->loopFireSound = qfalse;

	switch ( weaponNum ) {
	case WP_GAUNTLET:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/melee/fstrun.wav", qfalse );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/melee/fst_swoosh.wav", qfalse );
		weaponInfo->lfx = 0; //  no effect
		break;

	case WP_LIGHTNING:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		// weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/melee/fsthum.wav", qfalse );
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/lightning/lg_hum.wav", qfalse );

		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/lightning/lg_fire.wav", qfalse );
		cgs.media.lightningShader = trap_R_RegisterShader( "lightningBolt");
		cgs.media.lightningExplosionModel = trap_R_RegisterModel( "models/weaphits/crackle.md3" );
		cgs.media.sfx_lghit1 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit.wav", qfalse );
		cgs.media.sfx_lghit2 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit2.wav", qfalse );
		cgs.media.sfx_lghit3 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit3.wav", qfalse );
		weaponInfo->lfx = 0; //  no effect
		break;

	case WP_GRAPPLING_HOOK:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/hook/hook.md3" );
		weaponInfo->missileTrailFunc = CG_GrappleTrail;
		weaponInfo->missileDlight = 0;
		weaponInfo->wiTrailTime = 2000;
		weaponInfo->trailRadius = 64;
		MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
		cgs.media.grappleShader = trap_R_RegisterShader( "grappleRope");
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/grapple/grapfire.wav", qfalse );
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/grapple/grappull.wav", qfalse );
		//cgs.media.lightningShader = trap_R_RegisterShader( "lightningBoltNew");
		weaponInfo->lfx = 0; //  no effect
		break;

//#ifdef MISSIONPACK
	case WP_CHAINGUN:
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/vulcan/wvulfire.wav", qfalse );
		weaponInfo->loopFireSound = qtrue;
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
		// leilei -testing a looping firing sound instead of lots of flash sounds for performance reasons
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf3b.wav", qfalse );
		weaponInfo->flashSound[1] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf4b.wav", qfalse );
		weaponInfo->flashSound[2] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf3b.wav", qfalse );
		weaponInfo->flashSound[3] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf4b.wav", qfalse );
		weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
		cgs.media.bulletExplosionShader = trap_R_RegisterShader( "bulletExplosion" );
		weaponInfo->lfx = 71;
		break;
//#endif

	case WP_MACHINEGUN:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( va("sound/weapons/%s/machgf1b.wav",cg.mg_subst), qfalse );
		weaponInfo->flashSound[1] = trap_S_RegisterSound( va("sound/weapons/%s/machgf2b.wav",cg.mg_subst), qfalse );
		weaponInfo->flashSound[2] = trap_S_RegisterSound( va("sound/weapons/%s/machgf1b.wav",cg.mg_subst), qfalse );
		weaponInfo->flashSound[3] = trap_S_RegisterSound( va("sound/weapons/%s/machgf2b.wav",cg.mg_subst), qfalse );
		weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
		cgs.media.bulletExplosionShader = trap_R_RegisterShader( "bulletExplosion" );
		weaponInfo->lfx = 62;
		break;

	case WP_SHOTGUN:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/shotgun/sshotf1b.wav", qfalse );
		weaponInfo->ejectBrassFunc = CG_ShotgunEjectBrass;
		weaponInfo->lfx = 63;
		break;

	case WP_ROCKET_LAUNCHER:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/rocket/rocket.md3" );
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
		weaponInfo->missileTrailFunc = CG_RocketTrail;
		weaponInfo->missileDlight = 200;
		weaponInfo->wiTrailTime = 2000;
		weaponInfo->trailRadius = 64;

		MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.75f, 0 );

		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
		cgs.media.rocketExplosionShader = trap_R_RegisterShader( "rocketExplosion" );
		weaponInfo->lfx = 65;
		break;

//#ifdef MISSIONPACK
	case WP_PROX_LAUNCHER:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/proxmine.md3" );
		weaponInfo->missileTrailFunc = CG_GrenadeTrail;
		weaponInfo->wiTrailTime = 700;
		weaponInfo->trailRadius = 32;
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.70f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/proxmine/wstbfire.wav", qfalse );
		cgs.media.grenadeExplosionShader = trap_R_RegisterShader( "grenadeExplosion" );
		weaponInfo->lfx = 70;
		break;
//#endif

	case WP_GRENADE_LAUNCHER:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/grenade1.md3" );
		weaponInfo->missileTrailFunc = CG_GrenadeTrail;
		weaponInfo->wiTrailTime = 700;
		weaponInfo->trailRadius = 32;
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.70f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/grenade/grenlf1a.wav", qfalse );
		cgs.media.grenadeExplosionShader = trap_R_RegisterShader( "grenadeExplosion" );
		weaponInfo->lfx = 64;
		break;

//#ifdef MISSIONPACK
	case WP_NAILGUN:
		weaponInfo->ejectBrassFunc = CG_NailgunEjectBrass;
		weaponInfo->missileTrailFunc = CG_NailTrail;
//		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/nailgun/wnalflit.wav", qfalse );
		weaponInfo->trailRadius = 16;
		weaponInfo->wiTrailTime = 250;
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/nail.md3" );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.75f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/nailgun/wnalfire.wav", qfalse );
		weaponInfo->lfx = 69;
		break;
//#endif

	case WP_PLASMAGUN:
//		weaponInfo->missileModel = cgs.media.invulnerabilityPowerupModel;
		weaponInfo->missileTrailFunc = CG_PlasmaTrail;
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/plasma/lasfly.wav", qfalse );
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
		cgs.media.plasmaExplosionShader = trap_R_RegisterShader( "plasmaExplosion" );
		cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
		weaponInfo->lfx = 66;
		break;

	case WP_RAILGUN:
		// weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/railgun/rg_hum.wav", qfalse );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.5f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/railgun/railgf1a.wav", qfalse );
		cgs.media.railExplosionShader = trap_R_RegisterShader( "railExplosion" );
		cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
		cgs.media.railCoreShader = trap_R_RegisterShader( "railCore" );
		weaponInfo->lfx = 67;
		break;

	case WP_BFG:
		// weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
		cgs.media.bfgExplosionShader = trap_R_RegisterShader( "bfgExplosion" );
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/bfg.md3" );
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
		weaponInfo->lfx = 68;
		break;

	default:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
		break;
	}
}

/*
=================
CG_RegisterItemVisuals

The server says this item is used on this level
=================
*/
void CG_RegisterItemVisuals( int itemNum )
{
	itemInfo_t		*itemInfo;
	gitem_t			*item;

	if ( itemNum < 0 || itemNum >= bg_numItems ) {
		CG_Error( "CG_RegisterItemVisuals: itemNum %d out of range [0-%d]", itemNum, bg_numItems-1 );
	}

	itemInfo = &cg_items[ itemNum ];
	if ( itemInfo->registered ) {
		return;
	}

	item = &bg_itemlist[ itemNum ];

        if (itemNum > 0 && itemNum <= 60) {
                item->pickup_name = COM_Localize(itemNum+2);
        }

	memset( itemInfo, 0, sizeof( itemInfo ) );
	itemInfo->registered = qtrue;

        // Mix3r_Durachok: surrogates support - model replacement, specify for each weapon:
        if ( item->giType == IT_WEAPON ) {
                if (item->giTag == WP_MACHINEGUN) {
                        item->world_model[0] = va("models/weapons2/%s/%s.md3",cg.mg_subst,cg.mg_subst);
                } else if ( item->giTag == WP_LIGHTNING ) {
                        if (cg.lg_subst[0] == 'f') {
                                item->pickup_name = COM_Localize(97);
                                item->icon = "icons/iconw_fnozzle";
                        }
                } else if ( item->giTag == WP_GRAPPLING_HOOK ) {
                        // Mix3r_Durachok: surrogates support - map dependent mission pickup items as grapple pickup substitute:
                        item->world_model[0] = va("models/mission/%s_pickup.md3",strchr(cgs.mapname,'/')+1);
                }
        }


        itemInfo->models[0] = trap_R_RegisterModel( item->world_model[0] );

	itemInfo->icon = trap_R_RegisterShader( item->icon );

	if ( item->giType == IT_WEAPON ) {
		CG_RegisterWeapon( item->giTag );
	}

	//
	// powerups have an accompanying ring or sphere
	//
	if ( item->giType == IT_POWERUP || item->giType == IT_HEALTH ||
	        item->giType == IT_ARMOR || item->giType == IT_HOLDABLE ) {
		if ( item->world_model[1] ) {
			itemInfo->models[1] = trap_R_RegisterModel( item->world_model[1] );
		}
	}
}


/*
========================================================================================

VIEW WEAPON

========================================================================================
*/

/*
=================
CG_MapTorsoToWeaponFrame

=================
*/
static int CG_MapTorsoToWeaponFrame( clientInfo_t *ci, int frame )
{

	// change weapon
	if ( frame >= ci->animations[TORSO_DROP].firstFrame
	        && frame < ci->animations[TORSO_DROP].firstFrame + 9 ) {
		return frame - ci->animations[TORSO_DROP].firstFrame + 6;
	}

	// stand attack
	if ( frame >= ci->animations[TORSO_ATTACK].firstFrame
	        && frame < ci->animations[TORSO_ATTACK].firstFrame + 6 ) {
		return 1 + frame - ci->animations[TORSO_ATTACK].firstFrame;
	}

	// stand attack 2
	if ( frame >= ci->animations[TORSO_ATTACK2].firstFrame
	        && frame < ci->animations[TORSO_ATTACK2].firstFrame + 6 ) {
		return 1 + frame - ci->animations[TORSO_ATTACK2].firstFrame;
	}

	return 0;
}


/*
==============
CG_CalculateWeaponPosition
==============
*/
static void CG_CalculateWeaponPosition( vec3_t origin, vec3_t angles )
{
	float	scale, fracsin;
	int	delta;

	VectorCopy( cg.refdef.vieworg, origin );
	VectorCopy( cg.refdefViewAngles, angles );

	scale = cg.xyspeed_lerp;

	// gun angles from bobbing

	// Engoo bobbing port
    // Mix3r_Durachok: refactoring, added airhog-held gun and angle-gun-on-run anim
    if (cg.predictedPlayerState.powerups[PW_FLIGHT]) {
        delta = 0;
    } else if (cg_bobmodel.integer) {
		vec3_t forward, right, up;
		AngleVectors (angles, forward, right, up);
        switch (cg_bobmodel.integer) {
            case 1:
                forward[0] = scale * 2 * 0.05 * cg.bobfracsin * 0.04;
			    VectorMA (origin, forward[0], right, origin);
			    forward[0] = cos(scale * 0.07 * cg.bobfracsin * 0.05) - cos(scale * 0.07 * cg.bobfracsin * 0.1);
			    VectorMA (origin, forward[0], up, origin);
            break;
            case 2:
                up[0] = scale * -2 * 0.05 * cg.bobfracsin * 0.04;
			    VectorMA (origin, up[0], forward, origin);
            break;
            case 3:
			    forward[2] =  0.001f * scale;
			    if (forward[2] < 0) forward[2] *= -1;
                if (cg.bobfracsin > 0) {
                    forward[1] = cg.bobfracsin;
                } else {
                    forward[1] = -cg.bobfracsin;
                }
                forward[1] = forward[1]*2 - 1; // double frequency of cg.bobfracsin
                VectorMA (origin, cg.bobfracsin * forward[2], right, origin);
			    VectorMA (origin, forward[1] * 1.5 * forward[2], up, origin);
            break;
        }
        delta = 1;
	} else {
                //vec3_t forward, right, dest_point;
                //AngleVectors( angles, forward, right, NULL );
                //VectorMA (origin, 10, forward, dest_point);
                //VectorMA (dest_point, -13, right, dest_point);
                //VectorSubtract( dest_point, origin, dest_point );
                //vectoangles(dest_point, angles);

		angles[ROLL] += scale * cg.bobfracsin * 0.005;
		angles[YAW] += scale * cg.bobfracsin * 0.01;
		angles[PITCH] += cg.xyspeed_lerp * cg.bobfracsin * 0.005;
        delta = 2;
	}

	// leilei - fudgeweapon ported directly from quake :D  cg_leiDebug.value

	if (cg_viewnudge.integer) {
                switch (cg_viewsize.integer) {
                case 110:
                        origin[2] += 1;
                break;
                case 100:
                        origin[2] += 2;
                break;
                case 90:
                        origin[2] += 1;
                break;
                case 80:
                        origin[2] += 0.5;
                break;
                }
	}

	// idle drift
	if (delta) {
		scale = cg.xyspeed_lerp + 40;
		//fracsin = sin( cg.time * 0.001 );
                fracsin = sin( cg.time * 0.0021 );
		//angles[ROLL] += scale * fracsin * 0.01;
		//angles[YAW] += scale * fracsin * 0.01;
                angles[ROLL] += 40 * fracsin * 0.01; //// no more scale
		angles[YAW] += 40 * fracsin * 0.01; //// no more scale
		//angles[PITCH] += scale * fracsin * 0.01;
                angles[PITCH] += 40 * fracsin * 0.01;

                if ( cg.predictedPlayerState.eFlags & EF_FIRING ) {
                        cg.headEndYaw = cg.time;
                        //CG_Printf("aangleF: %.4f \n", cg.autoAnglesFast[0] );
                } else if (delta > 1 || cg.landChange > 0) {
                        fracsin = cg.xyspeed_lerp * 0.09;
                        if (fracsin > 55) {
                                fracsin = 55;
                        }
                        scale = (cg.time - cg.headEndYaw) * 0.0005;
                        //CG_Printf("sin: %.4f \n", sin(cg.refdefViewAngles[PITCH] / 180 * M_PI) );
                        if (cg.landChange > 0) {
                                scale = cg.time - cg.landTime;
                                if (scale < LAND_DEFLECT_TIME + LAND_RETURN_TIME) {
                                        scale = ((LAND_DEFLECT_TIME + LAND_RETURN_TIME)-scale)/(LAND_DEFLECT_TIME + LAND_RETURN_TIME);
                                        //CG_Printf("scale: %.4f \n", scale);
                                        scale = sin((scale*180) / 180 * M_PI);
                                } else {
                                        cg.landChange = 0;
                                        scale = 0;
                                }

                                fracsin = 55 * scale * 0.8;
                        }
                        if (scale > 1) {
                                scale = 1;
                        }
                        //fracsin = fracsin * sin(angles[PITCH]);

                        delta = 1; // YAW

                        //angles[ROLL] += fracsin * cg_leiDebug.value * sin(cg.refdefViewAngles[PITCH] / 180 * M_PI);

                        switch ( cg_drawGun.integer ) {
                        case 2:
                                scale = -scale;
                        break;
                        case 3:
                                scale = scale * 0.25;
                                delta = 0; // PITCH
                        break;
                        }

                        angles[delta] += fracsin * scale * cos(cg.refdefViewAngles[PITCH] / 180 * M_PI);

                        //angles[ROLL] += fracsin * scale * cg_leiDebug.value * fabs(sin(cg.refdefViewAngles[PITCH] / 180 * M_PI));
                        //angles[PITCH] += fracsin * scale * sin(cg.refdefViewAngles[PITCH] / 180 * M_PI);

                        //angles[ROLL] += cg_leiDebug.value;
                }
	}

    // Mix3r_Durachok: shotgun reload anim
    if (cg.predictedPlayerState.weapon == 3) {
        if (cg.predictedPlayerState.weaponTime > 200 && cg.predictedPlayerState.weaponTime < 500) {
            scale = sin(((cg.predictedPlayerState.weaponTime - 200)/300.0f)*M_PI);
            angles[PITCH] += 10*scale;
            if (cg_drawGun.integer == 2) {
                angles[ROLL] += 15*scale;
                angles[YAW] += -20*scale;
            } else {
                angles[ROLL] += -30*scale;
                angles[YAW] += 20*scale;
            }
        }
    }
	// drop the weapon when landing
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		origin[2] += cg.landChange*0.25 * delta / LAND_DEFLECT_TIME;
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		origin[2] += cg.landChange*0.25 *
		             (LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
	} else {
                // Mix3r_Durachok: simulate pickup movement
                delta = cg.time - cg.itemPickupTime;
                if (delta < 800) {
                        origin[0] -= (800-delta) * 0.005;
                }
    }

#if 0	// leilei - this may be dead pre-q3 1.01 code
	// drop the weapon when stair climbing
	delta = cg.time - cg.stepTime;
	if ( delta < STEP_TIME/2 ) {
		origin[2] -= cg.stepChange*0.25 * delta / (STEP_TIME/2);
	}
	else if ( delta < STEP_TIME ) {
		origin[2] -= cg.stepChange*0.25 * (STEP_TIME - delta) / (STEP_TIME/2);
	}
#endif
}


/*
===============
CG_LightningBolt

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
The cent should be the non-predicted cent if it is from the player,
so the endpoint will reflect the simulated strike (lagging the predicted
angle)
===============
*/
static void CG_LightningBolt( centity_t *cent, vec3_t origin )
{
	trace_t  trace;
	refEntity_t  beam;
	vec3_t   forward;
	vec3_t   muzzlePoint, endPoint;

	if (cent->currentState.weapon != WP_LIGHTNING) {
		return;
	}

	memset( &beam, 0, sizeof( beam ) );

//unlagged - attack prediction #1
	// if the entity is us, unlagged is on server-side, and we've got it on for the lightning gun
	if ( (cent->currentState.number == cg.predictedPlayerState.clientNum) && cgs.delagHitscan &&
	        ( cg_delag.integer & 1 || cg_delag.integer & 8 ) ) {
		// always shoot straight forward from our current position
		AngleVectors( cg.predictedPlayerState.viewangles, forward, NULL, NULL );
		VectorCopy( cg.predictedPlayerState.origin, muzzlePoint );
	}
	else
//unlagged - attack prediction #1

		// CPMA  "true" lightning
		if ((cent->currentState.number == cg.predictedPlayerState.clientNum) && (cg_trueLightning.value != 0)) {
			vec3_t angle;
			int i;

//unlagged - true lightning
			// might as well fix up true lightning while we're at it
			vec3_t viewangles;
			VectorCopy( cg.predictedPlayerState.viewangles, viewangles );
//unlagged - true lightning

			for (i = 0; i < 3; i++) {
				float a = cent->lerpAngles[i] - viewangles[i]; //unlagged: was cg.refdefViewAngles[i];
				if (a > 180) {
					a -= 360;
				}
				if (a < -180) {
					a += 360;
				}

				angle[i] = viewangles[i] /*unlagged: was cg.refdefViewAngles[i]*/ + a * (1.0 - cg_trueLightning.value);
				if (angle[i] < 0) {
					angle[i] += 360;
				}
				if (angle[i] > 360) {
					angle[i] -= 360;
				}
			}

			AngleVectors(angle, forward, NULL, NULL );
//unlagged - true lightning
//		VectorCopy(cent->lerpOrigin, muzzlePoint );
//		VectorCopy(cg.refdef.vieworg, muzzlePoint );
			// *this* is the correct origin for true lightning
			VectorCopy(cg.predictedPlayerState.origin, muzzlePoint );
//unlagged - true lightning
		}
		else {
			// !CPMA
			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			VectorCopy(cent->lerpOrigin, muzzlePoint );
		}

	// FIXME: crouch
	muzzlePoint[2] += DEFAULT_VIEWHEIGHT;

	VectorMA( muzzlePoint, 14, forward, muzzlePoint );

	// project forward by the lightning range
	VectorMA( muzzlePoint, LIGHTNING_RANGE, forward, endPoint );

	// see if it hit a wall
	CG_Trace( &trace, muzzlePoint, vec3_origin, vec3_origin, endPoint,
	          cent->currentState.number, MASK_SHOT );

	// this is the endpoint
	VectorCopy( trace.endpos, beam.oldorigin );

	// use the provided origin, even though it may be slightly
	// different than the muzzle origin
	VectorCopy( origin, beam.origin );

	beam.reType = RT_LIGHTNING;
	beam.customShader = cgs.media.lightningShader;
	trap_R_AddRefEntityToScene( &beam );

	// add the impact flare if it hit something
	if ( trace.fraction < 1.0 ) {
		vec3_t	angles;
		vec3_t	dir;

		VectorSubtract( beam.oldorigin, beam.origin, dir );
		VectorNormalize( dir );

		memset( &beam, 0, sizeof( beam ) );
		beam.hModel = cgs.media.lightningExplosionModel;

		VectorMA( trace.endpos, -16, dir, beam.origin );

		// make a random orientation
		angles[0] = rand() % 360;
		angles[1] = rand() % 360;
		angles[2] = rand() % 360;
		AnglesToAxis( angles, beam.axis );
		trap_R_AddRefEntityToScene( &beam );
	}
}
/*

static void CG_LightningBolt( centity_t *cent, vec3_t origin ) {
	trace_t		trace;
	refEntity_t		beam;
	vec3_t			forward;
	vec3_t			muzzlePoint, endPoint;

	if ( cent->currentState.weapon != WP_LIGHTNING ) {
		return;
	}

	memset( &beam, 0, sizeof( beam ) );

	// find muzzle point for this frame
	VectorCopy( cent->lerpOrigin, muzzlePoint );
	AngleVectors( cent->lerpAngles, forward, NULL, NULL );

	// FIXME: crouch
	muzzlePoint[2] += DEFAULT_VIEWHEIGHT;

	VectorMA( muzzlePoint, 14, forward, muzzlePoint );

	// project forward by the lightning range
	VectorMA( muzzlePoint, LIGHTNING_RANGE, forward, endPoint );

	// see if it hit a wall
	CG_Trace( &trace, muzzlePoint, vec3_origin, vec3_origin, endPoint,
		cent->currentState.number, MASK_SHOT );

	// this is the endpoint
	VectorCopy( trace.endpos, beam.oldorigin );

	// use the provided origin, even though it may be slightly
	// different than the muzzle origin
	VectorCopy( origin, beam.origin );

	beam.reType = RT_LIGHTNING;
	beam.customShader = cgs.media.lightningShader;
	trap_R_AddRefEntityToScene( &beam );

	// add the impact flare if it hit something
	if ( trace.fraction < 1.0 ) {
		vec3_t	angles;
		vec3_t	dir;

		VectorSubtract( beam.oldorigin, beam.origin, dir );
		VectorNormalize( dir );

		memset( &beam, 0, sizeof( beam ) );
		beam.hModel = cgs.media.lightningExplosionModel;

		VectorMA( trace.endpos, -16, dir, beam.origin );

		// make a random orientation
		angles[0] = rand() % 360;
		angles[1] = rand() % 360;
		angles[2] = rand() % 360;
		AnglesToAxis( angles, beam.axis );
		trap_R_AddRefEntityToScene( &beam );
	}
}
*/

/*
===============
CG_SpawnRailTrail

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
===============
*/
static void CG_SpawnRailTrail( centity_t *cent, vec3_t origin )
{
	clientInfo_t	*ci;

	if ( cent->currentState.weapon != WP_RAILGUN ) {
		return;
	}
	if ( !cent->pe.railgunFlash ) {
		return;
	}
	cent->pe.railgunFlash = qtrue;
	ci = &cgs.clientinfo[ cent->currentState.clientNum ];
	CG_RailTrail( ci, origin, cent->pe.railgunImpact );
}


/*
======================
CG_MachinegunSpinAngle
======================
*/
#define		SPIN_SPEED	0.9
#define		COAST_TIME	1000
static float	CG_MachinegunSpinAngle( centity_t *cent )
{
	int		delta;
	float	angle;
	float	speed;

	delta = cg.time - cent->pe.barrelTime;
	if ( cent->pe.barrelSpinning ) {
		angle = cent->pe.barrelAngle + delta * SPIN_SPEED;
	}
	else {
		if ( delta > COAST_TIME ) {
			delta = COAST_TIME;
		}

		speed = 0.5 * ( SPIN_SPEED + (float)( COAST_TIME - delta ) / COAST_TIME );
		angle = cent->pe.barrelAngle + delta * speed;
	}

	if ( cent->pe.barrelSpinning == !(cent->currentState.eFlags & EF_FIRING) ) {
		cent->pe.barrelTime = cg.time;
		cent->pe.barrelAngle = AngleMod( angle );
		cent->pe.barrelSpinning = !!(cent->currentState.eFlags & EF_FIRING);
//#ifdef MISSIONPACK
		if ( cent->currentState.weapon == WP_CHAINGUN && !cent->pe.barrelSpinning ) {
			trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, trap_S_RegisterSound( "sound/weapons/vulcan/wvulwind.wav", qfalse ) );
		}
//#endif
	}

	return angle;
}


/*
========================
CG_AddWeaponWithPowerups
========================
*/
static void CG_AddWeaponWithPowerups( refEntity_t *gun, int powerups )
{
	// add powerup effects
	if ( powerups & ( 1 << PW_INVIS ) ) {
		if( (cgs.dmflags & DF_INVIS) == 0) {
			gun->customShader = cgs.media.invisShader;
			trap_R_AddRefEntityToScene( gun );
		}
	}
	else {
		trap_R_AddRefEntityToScene( gun );

		if ( powerups & ( 1 << PW_BATTLESUIT ) ) {
			gun->customShader = cgs.media.battleWeaponShader;
			trap_R_AddRefEntityToScene( gun );
		}
		if ( powerups & ( 1 << PW_QUAD ) ) {
			gun->customShader = cgs.media.quadWeaponShader;
			trap_R_AddRefEntityToScene( gun );
		}
	}
}

static void MuzzleFlashStyleQ1(refEntity_t *parent, playerState_t *ps, centity_t *cent, weapon_t weaponNum, centity_t *nonPredictedCent, refEntity_t	*gun, weaponInfo_t *weapon) {
	vec3_t		angles;
	refEntity_t	flash;
	if ( ( weaponNum == WP_LIGHTNING || weaponNum == WP_GAUNTLET || weaponNum == WP_GRAPPLING_HOOK )
			&& ( nonPredictedCent->currentState.eFlags & EF_FIRING ) ) {
		// continuous flash
	}
	else {
		// impulse flash
		if ( cg.time - cent->muzzleFlashTime > 200 && !cent->pe.railgunFlash ) {
			return;
		}
	}
	memset( &flash, 0, sizeof( flash ) );
	VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
	flash.shadowPlane = parent->shadowPlane;
	flash.renderfx = parent->renderfx;

	flash.hModel = weapon->flashModel_type1;
	if (!flash.hModel) {
		return;
	}
	angles[YAW] = 0;
	angles[PITCH] = 0;
	angles[ROLL] = 0;	// don't roll randomly.
	AnglesToAxis( angles, flash.axis );


	// downscale hack
	if ( cg.time - cent->muzzleFlashTime > 100 ){
		VectorScale(flash.axis[0], 1.5, flash.axis[0]);
		VectorScale(flash.axis[1], 1.5, flash.axis[1]);
		VectorScale(flash.axis[2], 1.5, flash.axis[2]);
	}

	// colorize the railgun blast
	if ( weaponNum == WP_RAILGUN ) {
		clientInfo_t	*ci;

		ci = &cgs.clientinfo[ cent->currentState.clientNum ];
		flash.shaderRGBA[0] = 255 * ci->color1[0];
		flash.shaderRGBA[1] = 255 * ci->color1[1];
		flash.shaderRGBA[2] = 255 * ci->color1[2];
	}

	CG_PositionRotatedEntityOnTag( &flash, gun, weapon->weaponModel, "tag_flash");

	// leilei - allow the flash to go away 
	trap_R_AddRefEntityToScene( &flash );

	if ( ps || cg.renderingThirdPerson ||
			cent->currentState.number != cg.predictedPlayerState.clientNum ) {
		// add lightning bolt
		if ( cg.time - cent->muzzleFlashTime < 100 && !cent->pe.railgunFlash )// leilei - don't prolong the lightning
		CG_LightningBolt( nonPredictedCent, flash.origin );

		// add rail trail
		CG_SpawnRailTrail( cent, flash.origin );

		if ( cg.time - cent->muzzleFlashTime < 100 )	// flash for 0.1 sec
		if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] ) {
			trap_R_AddLightToScene( parent->origin, 300, 0.95,
									0.95, 0.95 );	// mono lighting
		}
	}
}

/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/

void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent, int team, char tagname[11] )
{
	refEntity_t	gun;
	refEntity_t	flash;
	weapon_t	weaponNum;
	weaponInfo_t	*weapon;
	centity_t	*nonPredictedCent;
	orientation_t	lerped;


	weaponNum = cent->currentState.weapon;

	CG_RegisterWeapon( weaponNum );
	weapon = &cg_weapons[weaponNum];

	// add the weapon
	memset( &gun, 0, sizeof( gun ) );

	// set custom shading for railgun refire rate
	if ( ps || cent->currentState.clientNum == cg.predictedPlayerState.clientNum ) {
		if ( cg.predictedPlayerState.weapon == WP_RAILGUN && cg.predictedPlayerState.weaponstate == WEAPON_FIRING ) {
			gun.shadowPlane = (float)cg.predictedPlayerState.weaponTime / 1500;
			gun.shaderRGBA[1] = 0;
			gun.shaderRGBA[0] =
			gun.shaderRGBA[2] = 255 * ( 1.0 - gun.shadowPlane );
		} else {
			gun.shaderRGBA[0] = 255;
			gun.shaderRGBA[1] = 255;
			gun.shaderRGBA[2] = 255;
			gun.shaderRGBA[3] = 255;
		}
	}

	gun.hModel = weapon->weaponModel;
	if (!gun.hModel) {
		return;
	}

        trap_R_LerpTag(&lerped, parent->hModel, parent->oldframe, parent->frame,1.0 - parent->backlerp, tagname);
	VectorCopy(parent->origin, gun.origin);

        VectorMA(gun.origin, lerped.origin[0], parent->axis[0], gun.origin);

	// Make weapon appear left-handed for 2 and centered for 3

        if (ps) {
                if (cg.zoomed || (cent->currentState.powerups & ( 1 << PW_FLIGHT ))) {
                } else if (cg_drawGun.integer < 3) {
                        VectorMA(gun.origin, lerped.origin[1] * (-2 * cg_drawGun.integer + 3), parent->axis[1], gun.origin);
                }
        } else {
                VectorMA(gun.origin, lerped.origin[1], parent->axis[1], gun.origin);
                // add weapon ready sound
		cent->pe.lightningFiring = qfalse;
		if ( ( cent->currentState.eFlags & EF_FIRING ) && weapon->firingSound ) {
			// lightning gun and guantlet make a different sound when fire is held down
			trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->firingSound );
			cent->pe.lightningFiring = qtrue;
		} else if ( weapon->readySound ) {
			trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->readySound );
		}
        }

	VectorMA(gun.origin, lerped.origin[2], parent->axis[2], gun.origin);

        //if ( (parent->nonNormalizedAxes) || (weaponNum != WP_GAUNTLET && (cent->currentState.eFlags & EF_FIRING)) ) {
        if ( (parent->nonNormalizedAxes) || ((cent->currentState.eFlags & EF_FIRING) && !ps && weaponNum != WP_GAUNTLET) ) {
            AnglesToAxis( cent->lerpAngles, gun.axis );
        } else {
	        MatrixMultiply(lerped.axis, ((refEntity_t *)parent)->axis, gun.axis);
        }

	gun.backlerp = parent->backlerp;
        VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
        gun.shadowPlane = parent->shadowPlane;
	gun.renderfx = parent->renderfx;

	CG_AddWeaponWithPowerups( &gun, cent->currentState.powerups );

	// add the spinning barrel
	if ( weapon->barrelModel ) {
		memset( &flash, 0, sizeof( flash ) );
		flash.hModel = weapon->barrelModel;
		flash.lightingOrigin[YAW] = 0;
		flash.lightingOrigin[PITCH] = 0;
		flash.lightingOrigin[ROLL] = CG_MachinegunSpinAngle( cent );
		AnglesToAxis( flash.lightingOrigin, flash.axis );

		CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->weaponModel, "tag_barrel" );
                VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
		flash.shadowPlane = parent->shadowPlane;
		flash.renderfx = parent->renderfx;
		CG_AddWeaponWithPowerups( &flash, cent->currentState.powerups );
	}

        gun.radius = cent->lerpAngles[PITCH];
        if (gun.radius < 0) {
                gun.radius = -gun.radius;
        }
        gun.radius = 12.0 - (gun.radius / 87.8906) * 7.0;
        //CG_Printf("gunradius: %.4f \n", gun.radius );
        VectorMA(gun.origin, gun.radius, gun.axis[2], cgs.clientinfo[ cent->currentState.clientNum ].pGunPoint);

	// make sure we aren't looking at cg.predictedPlayerEntity for LG
	nonPredictedCent = &cg_entities[cent->currentState.number];

	// if the index of the nonPredictedCent is not the same as the clientNum
	// then this is a fake player (like on teh single player podiums), so
	// go ahead and use the cent
	if( ( nonPredictedCent - cg_entities ) != cent->currentState.clientNum ) {
		nonPredictedCent = cent;
	}

	if ( ( weaponNum == WP_LIGHTNING || weaponNum == WP_GAUNTLET || weaponNum == WP_GRAPPLING_HOOK )
			&& ( nonPredictedCent->currentState.eFlags & EF_FIRING ) ) {
		// continuous flash
	} else {
		// impulse flash
		if ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME && !cent->pe.railgunFlash ) {
			return;
		}
	}

	memset( &flash, 0, sizeof( flash ) );

	flash.hModel = weapon->flashModel;
	if (!flash.hModel) {
		return;
	}
	flash.lightingOrigin[YAW] = 0;
	flash.lightingOrigin[PITCH] = 0;
	flash.lightingOrigin[ROLL] = crandom() * 10;
	AnglesToAxis( flash.lightingOrigin, flash.axis );

	// colorize the railgun blast
	if ( weaponNum == WP_RAILGUN ) {
		clientInfo_t	*ci;

		ci = &cgs.clientinfo[ cent->currentState.clientNum ];
		flash.shaderRGBA[0] = 255 * ci->color1[0];
		flash.shaderRGBA[1] = 255 * ci->color1[1];
		flash.shaderRGBA[2] = 255 * ci->color1[2];
	}
	CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->weaponModel, "tag_flash");

        VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
	flash.shadowPlane = parent->shadowPlane;
	flash.renderfx = parent->renderfx;

	// leilei - allow the flash to go away 
	trap_R_AddRefEntityToScene( &flash );

	if ( ps || cg.renderingThirdPerson || cent->currentState.number != cg.predictedPlayerState.clientNum ) {
		// add lightning bolt
		CG_LightningBolt( nonPredictedCent, flash.origin );

		// add rail trail
		CG_SpawnRailTrail( cent, cgs.clientinfo[ cent->currentState.clientNum ].pGunPoint );

                // Mix3r_Durachok !ps to avoid first person ugly flashing
		if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] ) {
			trap_R_AddLightToScene( flash.origin, 100 + (rand()&31), weapon->flashDlightColor[0],weapon->flashDlightColor[1], weapon->flashDlightColor[2] );
		}
	}
}

/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void CG_AddViewWeapon( playerState_t *ps )
{
	refEntity_t	hand;
	centity_t	*cent;
	clientInfo_t	*ci;
	vec3_t		angles;
	weaponInfo_t	*weapon;

	if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR || ps->pm_type == PM_INTERMISSION ) {
		return;
	}

	// no gun if in third person view or a camera is active
	if ( cg.renderingThirdPerson ) {
		return;
	}


	// allow the gun to be completely removed
	if ( !cg_drawGun.integer ) {
		vec3_t		origin;

		if ( cg.predictedPlayerState.eFlags & EF_FIRING ) {
			// special hack for lightning gun...
			VectorCopy( cg.refdef.vieworg, origin );
			VectorMA( origin, -8, cg.refdef.viewaxis[2], origin );
			CG_LightningBolt( &cg_entities[ps->clientNum], origin );
		}
		return;
	}

	// don't draw if testing a gun model
	if ( cg.testGun ) {
		return;
	}

	cent = &cg.predictedPlayerEntity;	// &cg_entities[cg.snap->ps.clientNum];
	CG_RegisterWeapon( ps->weapon );
	weapon = &cg_weapons[ ps->weapon ];

	memset (&hand, 0, sizeof(hand));

	// set up gun position
	CG_CalculateWeaponPosition( hand.origin, angles );

        AnglesToAxis( angles, hand.axis );

        // Mix3r_Durachok: store aspect factor & fov factor for 1-st person gun height adjust
        if ( cgs.glconfig.vidHeight > 0 ) {
                angles[0] = -8.32033 + 4.74 * cgs.glconfig.vidWidth / cgs.glconfig.vidHeight;
        } else {
                angles[0] = 0.1;
        }
        angles[2] = -3 - (cg_fov.integer - 90) * 0.08 + cg.headEndPitch[ps->weapon];

        VectorMA( hand.origin, angles[0], cg.refdef.viewaxis[0], hand.origin );
	//VectorMA( hand.origin, cg_gun_y.value, cg.refdef.viewaxis[1], hand.origin );
	VectorMA( hand.origin, angles[2], cg.refdef.viewaxis[2], hand.origin );

        //CG_Printf("hepitch: %.6f \n", cg.headEndPitch);
        //CG_Printf("angles0: %.6f ", angles[0]);
        //CG_Printf("angles2: %.6f \n", angles[2]);
        //CG_Printf("psweapon: %.6f \n", cg.headEndPitch[ps->weapon]);

	// map torso animations to weapon animations
	if ( cg_gun_frame.integer ) {
		// development tool
		hand.frame = hand.oldframe = cg_gun_frame.integer;
		hand.backlerp = 0;
	} else {
		// get clientinfo for animation map
		ci = &cgs.clientinfo[ cent->currentState.clientNum ];
		hand.frame = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.frame );
		hand.oldframe = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.oldFrame );
		hand.backlerp = cent->pe.torso.backlerp;
	}

	hand.hModel = weapon->handsModel;
	hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT;

	// add everything onto the hand
	CG_AddPlayerWeapon( &hand, ps, &cg.predictedPlayerEntity, ps->persistant[PERS_TEAM], "tag_weapon" );

        if (ci->handModel) {
            hand.hModel = ci->handModel;

            VectorCopy( cg.refdef.vieworg, hand.origin );
            AxisCopy( cg.refdef.viewaxis, hand.axis );
            angles[0]=cg.bobfracsin;
            angles[1]=(cg.refdefViewAngles[PITCH]-87.890) / -175.780; // 0 on look down, 1 on look up
            angles[2] = 0.48;

            if (angles[0]<0) {
                if (cg.predictedPlayerState.weapon == 1 && !weapon->weaponModel) {
                    angles[0] = -angles[0];
                    if (cg_drawGun.integer == 2) {
                        hand.frame = 0;
                    } else {
                        hand.frame = 1;
                        angles[2] = -angles[2];
                    }
                } else {
                    angles[0]=0;
                }
            } else {
                if (cg_drawGun.integer == 2) {
                    hand.frame = 1;
                    angles[2] = -angles[2];
                } else {
                    hand.frame = 0;
                }
            }

            hand.lightingOrigin[0]=cg.xyspeed_lerp/320.0f;
            if (hand.lightingOrigin[0] < 1) {
                angles[0] *= hand.lightingOrigin[0];
            }

            // Mix3r_Durachok: hand z axis adjustment for 4:3 aspect ratio
            if (cgs.glconfig.vidWidth != 0) {
                hand.lightingOrigin[0] = (cgs.glconfig.vidHeight/(float)cgs.glconfig.vidWidth - 0.5625) / 0.1875 * -1.18;
            } else {
                hand.lightingOrigin[0] = 0;
            }

            for (hand.oldframe=0 ; hand.oldframe<3 ; hand.oldframe++) {
	            hand.origin[hand.oldframe] += cg.refdef.viewaxis[0][hand.oldframe] * 4.005;
	            hand.origin[hand.oldframe] += cg.refdef.viewaxis[2][hand.oldframe] * (hand.lightingOrigin[0]-1.15+2.82*angles[0]-2.82*angles[1]); //angles[0]: bob fraction, angles [1]: look up fraction
	        }
            hand.oldframe = hand.frame;
            VectorCopy( hand.origin, hand.lightingOrigin );

            if ( cent->currentState.powerups & ( 1 << PW_INVIS ) ) {
		        if( (cgs.dmflags & DF_INVIS) == 0) {
		            hand.customShader = cgs.media.invisShader;
		        }
                trap_R_AddRefEntityToScene( &hand ); // invisible hand
            } else {
                trap_R_AddRefEntityToScene( &hand ); // normal hand

		        if ( cent->currentState.powerups & ( 1 << PW_BATTLESUIT ) ) {
                    hand.customShader = cgs.media.battleWeaponShader;
                    hand.oldframe = -1;
		        } else if ( cent->currentState.powerups & ( 1 << PW_QUAD ) ) {
		            hand.customShader = cgs.media.quadWeaponShader;
                    hand.oldframe = -1;
		        }
                if ( hand.oldframe == -1 ) {
                    for (hand.oldframe=0 ; hand.oldframe<3 ; hand.oldframe++) {
                        hand.origin[hand.oldframe] -= cg.refdef.viewaxis[0][hand.oldframe] * 0.014;
                        hand.origin[hand.oldframe] += cg.refdef.viewaxis[1][hand.oldframe] * angles[2];
                    }
                    hand.oldframe = hand.frame;
                    trap_R_AddRefEntityToScene( &hand ); // effect-overlay hand
                }
            }
        }
}

/*
==============================================================================

WEAPON SELECTION

==============================================================================
*/

/*
===================
CG_DrawWeaponSelect
===================
*/
void CG_DrawWeaponSelect( void )
{
	int		i;
	int		bits;
	int		count;
	float		*color;
	vec4_t		realColor;

	// don't display if dead
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	//Elimination: Always show weapon bar
	if (cg_weaponBarStyle.integer < 0) {
		realColor[0] = 1.0;
		realColor[1] = 1.0;
		realColor[2] = 1.0;
		realColor[3] = 1.0;
		color = realColor;
	} else {
                color = CG_FadeColor( cg.weaponSelectTime, WEAPON_SELECT_TIME );
                if ( !color ) {
		        return;
	        }
        }

	trap_R_SetColor( color );

	// showing weapon select clears pickup item display, but not the blend blob
	//cg.itemPickupTime = 0;

	// count the number of weapons owned
	bits = cg.snap->ps.stats[ STAT_WEAPONS ];
	count = 0;

	for ( i = 1 ; i < MAX_WEAPONS ; i++ ) {
		if ( bits & ( 1 << i ) ) {
			count++;
		}
	}

	switch(cg_weaponBarStyle.integer) {
	case 1:
        case -1:
		CG_DrawWeaponBar0(count,bits, color);
		break;
	case 2:
        case -2:
		CG_DrawWeaponBar1(count,bits);
		break;
	case 3:
        case -3:
		CG_DrawWeaponBar2(count,bits, color);
		break;
	case 4:
        case -4:
		CG_DrawWeaponBar3(count,bits, color);
		break;
	case 5:
        case -5:
		CG_DrawWeaponBar4(count,bits, color);
		break;
	case 6:
        case -6:
		CG_DrawWeaponBar5(count,bits, color);
		break;
	case 7:
        case -7:
		CG_DrawWeaponBar6(count,bits, color);
		break;
	case 8:
        case -8:
		CG_DrawWeaponBar7(count,bits);
		break;
	}
	trap_R_SetColor(NULL);
	return;
}

/*
===============
CG_DrawWeaponBar0
===============
*/

void CG_DrawWeaponBar0(int count, int bits, float *color)
{
	int y = 390;
	int x = 320 - count * 22;
	int i;
	float ammo;
	float max;
	int w;
	char *s;
	float yellow[4];
	float boxColor[4];

	//boxColor[1]=0;
	boxColor[3]=0.5f;

	yellow[0] = 1.0f;
	yellow[1] = 1.0f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

                w = cg.snap->ps.ammo[i];
                switch(i) {
                case WP_GAUNTLET:
                        w = 4;
                        break;
                case WP_PROX_LAUNCHER:
                        w = 4;
                        break;
                case WP_GRAPPLING_HOOK:
                        w = 4;
                        break;
                }
                if (w > 3) {
                        boxColor[0] = 0.0f;
                        boxColor[1] = 0.0f;
                        boxColor[2] = 0.0f;
                } else {
                        boxColor[0] = 0.5f;
                        boxColor[1] = 0.0f;
                        boxColor[2] = 0.0f;
                }

		if ( i == cg.weaponSelect) {
			CG_FillRect( x, y , 44 , 20, boxColor );
                        boxColor[0] = 0.75f;
                        boxColor[1] = 1.0f;
                        boxColor[2] = 0.75f;
			CG_DrawRect( x, y, 44 ,20 ,2, boxColor);
		} else {
			CG_FillRect( x, y,44, 20, boxColor );
		}
		CG_RegisterWeapon( i );
		//CG_DrawPic( x+7, y+2, 16, 16, cg_weapons[i].weaponIcon );
                CG_DrawPic( x, y+2, 16, 16, cg_weapons[i].weaponIcon );

		if(cg.snap->ps.ammo[ i ]!=-1) {
			s = va("%i", cg.snap->ps.ammo[ i ] );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(x - w/2 + 29, y+2, s, color);
		}

		x += 44;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar1
===============
*/

void CG_DrawWeaponBar1(int count, int bits)
{

	int y = 380;
	int x = 320 - count * 20;
	int i;
	int ammo;
	int br;
	int max;
	float red[4];
	float yellow[4];
	float green[4];

	red[0] = 1.0f;
	red[1] = 0;
	red[2] = 0;
	red[3] = 1.0f;

	yellow[0] = 1.0f;
	yellow[1] = 0.6f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

	green[0] = 0;
	green[1] = 1.0f;
	green[2] = 0;
	green[3] = 1.0f;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		ammo=cg.snap->ps.ammo[i];

		switch(i) {
		case WP_MACHINEGUN:
			max = 100;
			break;
		case WP_SHOTGUN:
			max = 10;
			break;
		case WP_GRENADE_LAUNCHER:
			max = 10;
			break;
		case WP_ROCKET_LAUNCHER:
			max = 10;
			break;
		case WP_LIGHTNING:
			max = 100;
			break;
		case WP_RAILGUN:
			max = 10;
			break;
		case WP_PLASMAGUN:
			max = 50;
			break;
		case WP_BFG:
			max = 10;
			break;
		case WP_NAILGUN:
			max = 10;
			break;
		case WP_PROX_LAUNCHER:
			max = 5;
			break;
		case WP_CHAINGUN:
			max = 100;
			break;
		default:
			max = 1;
			break;
		}

		ammo = (ammo*100)/max;
		if(ammo >=100)
			ammo=100;

		br=ammo*32/100;

		if(i!=WP_GAUNTLET && i!=WP_GRAPPLING_HOOK) {
			if(ammo <= 20)
				CG_FillRect( x, y+38, br,4, red);
			if(ammo > 20 && ammo <= 50)
				CG_FillRect( x, y+38, br, 4, yellow);
			if(ammo > 50)
				CG_FillRect( x, y+38, br, 4, green);
		}

		CG_RegisterWeapon( i );
		// draw weapon icon
		CG_DrawPic( x, y, 32, 32, cg_weapons[i].weaponIcon );

		// draw selection marker
		if ( i == cg.weaponSelect ) {
			CG_DrawPic( x-4, y-4, 40, 40, cgs.media.selectShader );
		}

		// no ammo cross on top
		if ( !cg.snap->ps.ammo[ i ] ) {
			CG_DrawPic( x, y, 32, 32, cgs.media.noammoShader );
		}

		x += 40;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar2
===============
*/

void CG_DrawWeaponBar2(int count, int bits, float *color)
{

	int y = 200 + count * 12;
	int x = 0;
	int i;
	int w;
	char *s;
	float red[4];
	float yellow[4];
	float blue[4];

	red[0] = 1.0f;
	red[1] = 0;
	red[2] = 0;
	red[3] = 0.4f;

	yellow[0] = 1.0f;
	yellow[1] = 1.0f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

	blue[0] = 0;
	blue[1] = 0;
	blue[2] = 1.0f;
	blue[3] = 0.4f;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		if(cg.snap->ps.ammo[i]) {
			if ( i == cg.weaponSelect) {
				CG_FillRect( x, y, 50, 24, blue );
				CG_DrawRect( x, y, 50, 24, 2, yellow);
			}
			else {
				CG_FillRect( x, y,50, 24, blue );
			}
		}
		else {
			if ( i == cg.weaponSelect) {
				CG_FillRect( x, y, 50, 24, red );
				CG_DrawRect( x, y, 50, 24, 2, yellow);
			}
			else {
				CG_FillRect( x, y,50, 24, red );
			}
		}

		CG_RegisterWeapon( i );
		// draw weapon icon
		CG_DrawPic( x+2, y+4, 16, 16, cg_weapons[i].weaponIcon );

		/** Draw Weapon Ammo **/
		if(cg.snap->ps.ammo[ i ]!=-1) {
			s = va("%i", cg.snap->ps.ammo[ i ] );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(x - w/2 + 32, y+4, s, color);
		}

		y -= 24;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar3
===============
*/

void CG_DrawWeaponBar3(int count, int bits, float *color)
{

	int y = 200 + count * 12;
	int x = 0;
	int i;
	int ammo;
	int max;
	int br;
	int w;
	char *s;
	float red[4];
	float yellow[4];
	float green[4];
	float blue[4];

	red[0] = 1.0f;
	red[1] = 0;
	red[2] = 0;
	red[3] = 0.4f;

	yellow[0] = 1.0f;
	yellow[1] = 1.0f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

	green[0] = 0;
	green[1] = 1.0f;
	green[2] = 0;
	green[3] = 1.0f;

	blue[0] = 0;
	blue[1] = 0;
	blue[2] = 1.0f;
	blue[3] = 0.4f;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		ammo=cg.snap->ps.ammo[i];

		switch(i) {
		case WP_MACHINEGUN:
			max = 100;
			break;
		case WP_SHOTGUN:
			max = 10;
			break;
		case WP_GRENADE_LAUNCHER:
			max = 10;
			break;
		case WP_ROCKET_LAUNCHER:
			max = 10;
			break;
		case WP_LIGHTNING:
			max = 100;
			break;
		case WP_RAILGUN:
			max = 10;
			break;
		case WP_PLASMAGUN:
			max = 50;
			break;
		case WP_BFG:
			max = 10;
			break;
		case WP_NAILGUN:
			max = 10;
			break;
		case WP_PROX_LAUNCHER:
			max = 5;
			break;
		case WP_CHAINGUN:
			max = 100;
			break;
		default:
			max = 1;
			break;
		}

		ammo = (ammo*100)/max;
		if(ammo >=100)
			ammo=100;

		br=ammo*20/100;

		if(i!=WP_GAUNTLET && i!=WP_GRAPPLING_HOOK) {
			if(ammo <= 20)
				CG_FillRect( 51, y+2+20-br, 4,br, red);
			if(ammo > 20 && ammo <= 50)
				CG_FillRect( 51, y+2+20-br, 4,br, yellow);
			if(ammo > 50)
				CG_FillRect( 51, y+2+20-br, 4,br, green);
		}

		if(cg.snap->ps.ammo[i]) {
			if ( i == cg.weaponSelect) {
				CG_FillRect( x, y, 50, 24, blue );
				CG_DrawRect( x, y, 50, 24, 2, yellow);
			}
			else {
				CG_FillRect( x, y,50, 24, blue );
			}
		}
		else {
			if ( i == cg.weaponSelect) {
				CG_FillRect( x, y, 50, 24, red );
				CG_DrawRect( x, y, 50, 24, 2, yellow);
			}
			else {
				CG_FillRect( x, y,50, 24, red );
			}
		}
		CG_RegisterWeapon( i );
		// draw weapon icon
		CG_DrawPic( x+2, y+4, 16, 16, cg_weapons[i].weaponIcon );

		/** Draw Weapon Ammo **/
		if(cg.snap->ps.ammo[ i ]!=-1) {
			s = va("%i", cg.snap->ps.ammo[ i ] );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(x - w/2 + 32, y+4, s, color);
		}

		y -= 24;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar4
===============
*/

void CG_DrawWeaponBar4(int count, int bits, float *color)
{

	int y = 200 + count * 12;
	int x = 0;
	int i;
	float ammo;
	int max;
	int w;
	char *s;
	float boxColor[4];
	float yellow[4];

	boxColor[1]=0;
	boxColor[3]=0.4f;

	yellow[0] = 1.0f;
	yellow[1] = 1.0f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		ammo=cg.snap->ps.ammo[i];

		switch(i) {
		case WP_MACHINEGUN:
			max = 100;
			break;
		case WP_SHOTGUN:
			max = 10;
			break;
		case WP_GRENADE_LAUNCHER:
			max = 10;
			break;
		case WP_ROCKET_LAUNCHER:
			max = 10;
			break;
		case WP_LIGHTNING:
			max = 100;
			break;
		case WP_RAILGUN:
			max = 10;
			break;
		case WP_PLASMAGUN:
			max = 50;
			break;
		case WP_BFG:
			max = 10;
			break;
		case WP_NAILGUN:
			max = 10;
			break;
		case WP_PROX_LAUNCHER:
			max = 5;
			break;
		case WP_CHAINGUN:
			max = 100;
			break;
		default:
			max = 1;
			break;
		}

		ammo = (ammo*100)/max;

		if((ammo >=100) || (ammo < 0))
			ammo=100;

		boxColor[2]=(ammo/100.0f)*1.0f;
		boxColor[0]=1.0f-(ammo/100.0f)*1.0f;

		if ( i == cg.weaponSelect) {
			CG_FillRect( x, y, 50, 24, boxColor );
			CG_DrawRect( x, y, 50, 24, 2, yellow);
		}
		else {
			CG_FillRect( x, y,50, 24, boxColor );
		}
		CG_RegisterWeapon( i );
		// draw weapon icon
		CG_DrawPic( x+2, y+4, 16, 16, cg_weapons[i].weaponIcon );

		/** Draw Weapon Ammo **/
		if(cg.snap->ps.ammo[ i ]!=-1) {
			s = va("%i", cg.snap->ps.ammo[ i ] );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(x - w/2 + 32, y+4, s, color);
		}

		y -= 24;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar5
===============
*/

void CG_DrawWeaponBar5(int count, int bits, float *color)
{

	int y = 380;
	int x = 320 - count * 20;
	int i;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		CG_RegisterWeapon( i );
		// draw weapon icon
		CG_DrawPic( x, y, 32, 32, cg_weapons[i].weaponIcon );

		// draw selection marker
		if ( i == cg.weaponSelect ) {
			CG_DrawPic( x-4, y-4, 40, 40, cgs.media.selectShader );
		}

		// no ammo cross on top
		if ( !cg.snap->ps.ammo[ i ] ) {
			CG_DrawPic( x, y, 32, 32, cgs.media.noammoShader );
		}

		x += 40;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar6
===============
*/

void CG_DrawWeaponBar6(int count, int bits, float *color)
{

	int y = 380;
	int x = 320 - count * 15;
	int i;
	int ammo;
	int max;
	int br;
	int w;
	char *s;
	float red[4];
	float yellow[4];
	float green[4];
	float blue[4];

	red[0] = 1.0f;
	red[1] = 0;
	red[2] = 0;
	red[3] = 0.4f;

	yellow[0] = 1.0f;
	yellow[1] = 1.0f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

	green[0] = 0;
	green[1] = 1.0f;
	green[2] = 0;
	green[3] = 1.0f;

	blue[0] = 0;
	blue[1] = 0;
	blue[2] = 1.0f;
	blue[3] = 0.4f;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		ammo=cg.snap->ps.ammo[i];

		switch(i) {
		case WP_MACHINEGUN:
			max = 100;
			break;
		case WP_SHOTGUN:
			max = 10;
			break;
		case WP_GRENADE_LAUNCHER:
			max = 10;
			break;
		case WP_ROCKET_LAUNCHER:
			max = 10;
			break;
		case WP_LIGHTNING:
			max = 100;
			break;
		case WP_RAILGUN:
			max = 10;
			break;
		case WP_PLASMAGUN:
			max = 50;
			break;
		case WP_BFG:
			max = 10;
			break;
		case WP_NAILGUN:
			max = 10;
			break;
		case WP_PROX_LAUNCHER:
			max = 5;
			break;
		case WP_CHAINGUN:
			max = 100;
			break;
		default:
			max = 1;
			break;
		}

		ammo = (ammo*100)/max;

		if(ammo >=100)
			ammo=100;

		br=ammo*26/100;

		if(i!=WP_GAUNTLET && i!=WP_GRAPPLING_HOOK) {
			if(ammo <= 20)
				CG_FillRect( x+2, y +40, br, 4, red);
			if(ammo > 20 && ammo <= 50)
				CG_FillRect( x+2, y+40, br, 4, yellow);
			if(ammo > 50)
				CG_FillRect( x+2, y+40, br, 4, green);
		}

		if(cg.snap->ps.ammo[i]) {
			if ( i == cg.weaponSelect) {
				CG_FillRect( x, y , 30 , 38, blue );
				CG_DrawRect( x, y, 30 ,38 ,2, yellow);
			}
			else {
				CG_FillRect( x, y,30, 38, blue );
			}
		}
		else {
			if ( i == cg.weaponSelect) {
				CG_FillRect( x, y , 30 , 38, red );
				CG_DrawRect( x , y, 30,38,2, yellow);
			}
			else {
				CG_FillRect( x, y,30, 38, red );
			}
		}
		CG_RegisterWeapon( i );
		CG_DrawPic( x+7, y+2, 16, 16, cg_weapons[i].weaponIcon );

		if(cg.snap->ps.ammo[ i ]!=-1) {
			s = va("%i", cg.snap->ps.ammo[ i ] );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(x - w/2 + 15, y+20, s, color);
		}

		x += 30;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}

/*
===============
CG_DrawWeaponBar7
===============
*/

void CG_DrawWeaponBar7(int count, int bits)
{
	int y = 2;
	int x = 320 - count * 15;
	int i;
	int w;
	char *s;
	//float red[4];
	float yellow[4];
	float blue[4];

	//red[0] = 1.0f;
	//red[1] = 0;
	//red[2] = 0;
	//red[3] = 0.4f;

	yellow[0] = 1.0f;
	yellow[1] = 1.0f;
	yellow[2] = 0;
	yellow[3] = 1.0f;

        blue[0] = 0.0;
	blue[1] = 0.0;
        blue[2] = 0.0f;
	blue[3] = 0.33f;

        trap_R_SetColor(blue);
        CG_DrawPic(x, y, count * 30, 36, cgs.media.teamStatusBar);


	//blue[2] = 1.0f;
	//blue[3] = 0.4f;

        //y = cg_leiDebug.value;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		//Sago: Do mad change of grapple placement:
		if(i==10)
			continue;
		if(i==0)
			i=10;
		if ( !( bits & ( 1 << i ) ) ) {
			if(i==10)
				i=0;
			continue;
		}

		if(cg.snap->ps.ammo[i]) {
			if ( i == cg.weaponSelect) {
				//CG_FillRect( x, y , 30 , 38, blue );
				CG_DrawRect( x, y, 30 ,38 ,2, yellow);
			}
		} else {
			if ( i == cg.weaponSelect) {
				//CG_FillRect( x, y , 30 , 38, red );
				CG_DrawRect( x , y, 30,38,2, yellow);
			}
		}
		CG_RegisterWeapon( i );
		CG_DrawPic( x+7, y+2, 16, 16, cg_weapons[i].weaponIcon );

		if(cg.snap->ps.ammo[ i ]!=-1) {
			s = va("%i", cg.snap->ps.ammo[ i ] );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallStringColor(x - w/2 + 15, y+19, s, colorWhite);
		}

		x += 30;
		//Sago: Undo mad change of weapons
		if(i==10)
			i=0;
	}
}


/*
===============
CG_WeaponSelectable
===============
*/
static qboolean CG_WeaponSelectable( int i )
{
	if ( !cg.snap->ps.ammo[i] ) {
		return qfalse;
	}
	if ( ! (cg.snap->ps.stats[ STAT_WEAPONS ] & ( 1 << i ) ) ) {
		return qfalse;
	}

	return qtrue;
}

static int CG_GetBestWeapon(void);

void CG_BestWeapon_f( void ) {
	int		newW;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	newW = CG_GetBestWeapon();
	if (newW != WP_NONE) {
		cg.weaponSelectTime = cg.time;
		cg.weaponSelect = newW;
	}
}

/*
===============
CG_NextWeapon_f
===============
*/
void CG_NextWeapon_f( void )
{
	int		i;
	int		original;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	cg.weaponSelectTime = cg.time;
	original = cg.weaponSelect;
	//Part of mad hook select code:
	if(cg.weaponSelect == WP_GRAPPLING_HOOK)
		cg.weaponSelect = 0;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		cg.weaponSelect++;
		if ( cg.weaponSelect == MAX_WEAPONS ) {
			cg.weaponSelect = 0;
		}
		if ( cg.weaponSelect == WP_GAUNTLET ) {
			continue;		// never cycle to gauntlet
		}
		//Sago: Mad change of grapple order
		if( cg.weaponSelect == WP_GRAPPLING_HOOK)  {
			continue;
		}
		if( cg.weaponSelect == 0)
			cg.weaponSelect = WP_GRAPPLING_HOOK;
		if ( cg.weaponSelect == WP_GRAPPLING_HOOK && !cg_cyclegrapple.integer ) {
			cg.weaponSelect = 0;
			continue;		// never cycle to grapple unless the client wants it
		}
		if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
			break;
		}
		if( cg.weaponSelect == WP_GRAPPLING_HOOK)
			cg.weaponSelect = 0;
	}
	if ( i == MAX_WEAPONS ) {
		cg.weaponSelect = original;
	}
}

/*
===============
CG_PrevWeapon_f
===============
*/
void CG_PrevWeapon_f( void )
{
	int		i;
	int		original;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	cg.weaponSelectTime = cg.time;
	original = cg.weaponSelect;
	//Part of mad hook select code:
	if(cg.weaponSelect == WP_GRAPPLING_HOOK)
		cg.weaponSelect = 0;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		cg.weaponSelect--;
		if ( cg.weaponSelect == -1 ) {
			cg.weaponSelect = MAX_WEAPONS - 1;
		}
		if ( cg.weaponSelect == WP_GAUNTLET ) {
			continue;		// never cycle to gauntlet
		}
		//Sago: Mad change of grapple order
		if( cg.weaponSelect == WP_GRAPPLING_HOOK)  {
			continue;
		}
		if( cg.weaponSelect == 0)
			cg.weaponSelect = WP_GRAPPLING_HOOK;
		if ( cg.weaponSelect == WP_GRAPPLING_HOOK && !cg_cyclegrapple.integer ) {
			cg.weaponSelect = 0;
			continue;		// never cycle to grapple unless the client wants it
		}
		if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
			break;
		}
		if( cg.weaponSelect == WP_GRAPPLING_HOOK)
			cg.weaponSelect = 0;
	}
	if ( i == MAX_WEAPONS ) {
		cg.weaponSelect = original;
	}
}

/*
===============
CG_Weapon_f
===============
*/
void CG_Weapon_f( void )
{
	int		num;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	num = atoi( CG_Argv( 1 ) );

	if ( num < 1 || num > MAX_WEAPONS-1 ) {
		return;
	}

	cg.weaponSelectTime = cg.time;

	if ( ! ( cg.snap->ps.stats[STAT_WEAPONS] & ( 1 << num ) ) ) {
		return;		// don't have the weapon
	}

	cg.weaponSelect = num;
}

/**
 * Gives the best weapon with ammo based on cg_weaponOrder
 * If it cannot find a weapon WP_NONE is returned
 * @return A weapon number or WP_NONE if none is found
 */
static int CG_GetBestWeapon(void) {
	int ret = WP_NONE;
	const char* currentPlace;
	int newWeaponNumber;
	for (currentPlace = strstr(cg_weaponOrder.string, "/"); currentPlace; currentPlace = strstr(currentPlace, "/") ) {
		currentPlace++;
		newWeaponNumber = atoi(currentPlace);
		if ( CG_WeaponSelectable(newWeaponNumber) ) {
			ret = newWeaponNumber;
		}
	}
	return ret;
}

/*
===================
CG_OutOfAmmoChange

The current weapon has just run out of ammo
===================
*/
void CG_OutOfAmmoChange( void )
{
	int		i;

	cg.weaponSelectTime = cg.time;
	cg.weaponSelect = CG_GetBestWeapon();
	if (cg.weaponSelect == WP_NONE) {
		for ( i = MAX_WEAPONS-1 ; i > 0 ; i-- ) {
			if ( CG_WeaponSelectable( i ) && i != WP_GRAPPLING_HOOK ) {
				cg.weaponSelect = i;
				break;
			}
		}
	}
}



/*
===================================================================================================

WEAPON EVENTS

===================================================================================================
*/

/*
================
CG_FireWeapon

Caused by an EV_FIRE_WEAPON event
================
*/
void CG_FireWeapon( centity_t *cent )
{
	entityState_t *ent;
	int				c;
	weaponInfo_t	*weap;

	if(CG_IsARoundBasedGametype(cgs.gametype) && CG_IsATeamGametype(cgs.gametype) && cgs.roundStartTime>=cg.time)
		return; //if we havn't started in ELIMINATION then do not fire

	ent = &cent->currentState;
	if ( ent->weapon == WP_NONE ) {
		return;
	}
	if ( ent->weapon >= WP_NUM_WEAPONS ) {
		CG_Error( "CG_FireWeapon: ent->weapon >= WP_NUM_WEAPONS" );
		return;
	}
	weap = &cg_weapons[ ent->weapon ];

	// mark the entity as muzzle flashing, so when it is added it will
	// append the flash to the weapon model
	cent->muzzleFlashTime = cg.time;

	// leilei - hack for multiple muzzleflashes
	cent->muztime[cent->muzslot] = cg.time;
	cent->muzslot += 1;
	if (cent->muzslot > 2)
	cent->muzslot = 0;

	// lightning gun only does this this on initial press
	if ( ent->weapon == WP_LIGHTNING ) {
		if ( cent->pe.lightningFiring ) {
			return;
		}
	}

	// play quad sound if needed
	if ( cent->currentState.powerups & ( 1 << PW_QUAD ) ) {
                if (cg.time > cg.nQuadSoundUnleash) {
		    trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.media.quadSound );
                    cg.nQuadSoundUnleash = cg.time+750;
                }
	}

	// play a sound
    // Mix3r_Durachok: play underwater dull sound
    if (CG_WaterLevel(cent) == 3) {
        trap_S_StartSound (NULL, ent->number, CHAN_WEAPON, cgs.media.UwFireSound );
    } else {
	    for ( c = 0 ; c < 4 ; c++ ) {
		    if ( !weap->flashSound[c] ) {
			    break;
		    }
	    }
        if ( c > 0 ) {
		    c = rand() % c;
		    if ( weap->flashSound[c] ) {
			    trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound[c] );
		    }
	    }
    }

	// do brass ejection
	if ( weap->ejectBrassFunc && cg_brassTime.integer > 0 ) {
		weap->ejectBrassFunc( cent );
	}

        //unlagged - attack prediction #1
	CG_PredictWeaponEffects( cent );
        //unlagged - attack prediction #1
}


/*
==========================
CG_Explosionia LEILEI
==========================

static void CG_Explosionia ( centity_t *cent ) {
	localEntity_t	*le;
	ec3_t			velocity, xvelocity;
	vec3_t			offset, xoffset;
	float			waterScale = 1.0f;
	vec3_t			v[3];

	if ( cg_brassTime.integer <= 0 ) {
		return;
	}

	le = CG_AllocLocalEntity();


	velocity[0] = -50 + 100 * crandom();
	velocity[1] = -50 + 100 * crandom();
	velocity[2] = -50 + 100 * crandom();

	le->leType = LE_FALL_SCALE_FADE;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();

	//le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - (rand()&15);

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 8;
	offset[1] = -4;
	offset[2] = 24;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
	VectorAdd( cent->lerpOrigin, xoffset, re->origin );

	VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	le->bounceFactor = 0.4 * waterScale;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 2;
	le->angles.trDelta[1] = 1;
	le->angles.trDelta[2] = 0;
	le = CG_SmokePuff( le->origin, le->velocity,
					  30,		// radius
					  1, 1, 1, 1,	// color
					  2000,		// trailTime
					  cg.time,		// startTime
					  0,		// fadeInTime
					  0,		// flags
					  cgs.media.lbumShader1 );
	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_NONE;
	le->leMarkType = LEMT_NONE;
}

*/

/*
=================
CG_MissileHitWall

Caused by an EV_MISSILE_MISS event, or directly by local bullet tracing
=================
*/
void CG_MissileHitWall( int weapon, int clientNum, vec3_t origin, vec3_t dir, impactSound_t soundType )
{
	qhandle_t		mod;
	qhandle_t		mark;
	qhandle_t		shader;
	sfxHandle_t		sfx;
	float			radius;
	float			light;
	vec3_t			lightColor;
	localEntity_t	*le;
	int				r;
	qboolean		alphaFade;
	qboolean		isSprite;
	int				duration;

	mark = 0;
	radius = 32;
	sfx = 0;
	mod = 0;
	shader = 0;
	light = 0;
	lightColor[0] = 1;
	lightColor[1] = 1;
	lightColor[2] = 0;

	// set defaults
	isSprite = qfalse;
	duration = 600;

	switch ( weapon ) {
	default:
//#ifdef MISSIONPACK
	case WP_NAILGUN:
		if( soundType == IMPACTSOUND_FLESH ) {
			sfx = cgs.media.sfx_nghitflesh;
		}
		else if( soundType == IMPACTSOUND_METAL ) {
			sfx = cgs.media.sfx_nghitmetal;
		}
		else {
			sfx = cgs.media.sfx_nghit;
		}
		mark = cgs.media.holeMarkShader;
		radius = 12;
		break;
//#endif
	case WP_LIGHTNING:
		// no explosion at LG impact, it is added with the beam
		r = rand() & 3;
		if ( r < 2 ) {
			sfx = cgs.media.sfx_lghit2;
		}
		else if ( r == 2 ) {
			sfx = cgs.media.sfx_lghit1;
		}
		else {
			sfx = cgs.media.sfx_lghit3;
		}
		mark = cgs.media.holeMarkShader;
		radius = 12;
		break;
//#ifdef MISSIONPACK
	case WP_PROX_LAUNCHER:
		mod = cgs.media.dishFlashModel;
		shader = cgs.media.grenadeExplosionShader;
		sfx = cgs.media.sfx_proxexp;
		mark = cgs.media.burnMarkShader;
		radius = 64;
		light = 300;
		isSprite = qtrue;
		break;
//#endif
	case WP_GRENADE_LAUNCHER:
		mod = cgs.media.dishFlashModel;
		shader = cgs.media.grenadeExplosionShader;
		sfx = cgs.media.sfx_rockexp;
		mark = cgs.media.burnMarkShader;
		radius = 64;
		light = 300;
		isSprite = qtrue;
		break;
	case WP_ROCKET_LAUNCHER:
		mod = cgs.media.dishFlashModel;
		shader = cgs.media.rocketExplosionShader;
		sfx = cgs.media.sfx_rockexp;
		mark = cgs.media.burnMarkShader;
		radius = 64;
		light = 300;
		isSprite = qtrue;
		duration = 1000;
		lightColor[0] = 1;
		lightColor[1] = 0.75;
		lightColor[2] = 0.0;
		break;
	case WP_RAILGUN:
		mod = cgs.media.ringFlashModel;
		shader = cgs.media.railExplosionShader;
		sfx = cgs.media.sfx_plasmaexp;
		mark = cgs.media.energyMarkShader;
		radius = 24;
		break;
	case WP_PLASMAGUN:
		mod = cgs.media.ringFlashModel;
		shader = cgs.media.plasmaExplosionShader;
		sfx = cgs.media.sfx_plasmaexp;
		mark = cgs.media.energyMarkShader;
		radius = 16;
		break;
	case WP_BFG:
		mod = cgs.media.dishFlashModel;
		shader = cgs.media.bfgExplosionShader;
		sfx = cgs.media.sfx_rockexp;
		mark = cgs.media.burnMarkShader;
		radius = 32;
		isSprite = qtrue;
		break;
	case WP_SHOTGUN:
		mod = cgs.media.bulletFlashModel;
		shader = cgs.media.bulletExplosionShader;
		mark = cgs.media.bulletMarkShader;
                if ( soundType == IMPACTSOUND_METAL ) {
			sfx = cgs.media.sfx_nghitmetal;
		} else {
                        sfx = 0;
                }
		radius = 4;
                duration = 300;
		break;

	case WP_CHAINGUN:
		mod = cgs.media.bulletFlashModel;

		if( soundType == IMPACTSOUND_FLESH ) {
			sfx = cgs.media.sfx_chghitflesh;
		}
		else if( soundType == IMPACTSOUND_METAL ) {
			sfx = cgs.media.sfx_chghitmetal;
		}
		else {
			sfx = cgs.media.sfx_chghit;
		}

		mark = cgs.media.bulletMarkShader;
		r = rand() & 3;
		if ( r < 2 ) {
			sfx = cgs.media.sfx_ric1;
		}
		else if ( r == 2 ) {
			sfx = cgs.media.sfx_ric2;
		}
		else {
			sfx = cgs.media.sfx_ric3;
		}

		radius = 8;
		break;

	// Shared with Vulcan/Chaingun
	case WP_MACHINEGUN:
		mod = cgs.media.bulletFlashModel;
		shader = cgs.media.bulletExplosionShader;
		mark = cgs.media.bulletMarkShader;

		r = rand() & 3;
		if ( r == 0 ) {
			sfx = cgs.media.sfx_ric1;
		}
		else if ( r == 1 ) {
			sfx = cgs.media.sfx_ric2;
		}
		else {
			sfx = cgs.media.sfx_ric3;
		}

		radius = 8;
                duration = 300;
		break;
	}

	if ( sfx ) {
		trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, sfx );
	}

	//
	// create the explosion
	//
	if ( mod ) {
		le = CG_MakeExplosion( origin, dir,
		                       mod,	shader,
		                       duration, isSprite );
		le->light = light;
		VectorCopy( lightColor, le->lightColor );
		if ( weapon == WP_RAILGUN ) {
			// colorize with client color
			VectorCopy( cgs.clientinfo[clientNum].color1, le->color );
			le->refEntity.shaderRGBA[0] = le->color[0] * 0xff;
			le->refEntity.shaderRGBA[1] = le->color[1] * 0xff;
			le->refEntity.shaderRGBA[2] = le->color[2] * 0xff;
			le->refEntity.shaderRGBA[3] = 0xff;
		}
	}

	//
	// impact mark
	//
	alphaFade = (mark == cgs.media.energyMarkShader);	// plasma fades alpha, all others fade color
	if ( weapon == WP_RAILGUN ) {
		float	*color;

		// colorize with client color
		color = cgs.clientinfo[clientNum].color1;
		CG_ImpactMark( mark, origin, dir, random()*360, color[0],color[1], color[2],1, alphaFade, radius, qfalse );
	}
	else {
		CG_ImpactMark( mark, origin, dir, random()*360, 1,1,1,1, alphaFade, radius, qfalse );
	}
}


/*
=================
CG_MissileHitPlayer
=================
*/
void CG_MissileHitPlayer( int weapon, vec3_t origin, vec3_t dir, int entityNum )
{
        if ( weapon != WP_NAILGUN && weapon != WP_GAUNTLET ) {
                CG_Bleed( origin, entityNum );
        }
	switch ( weapon ) {
	case WP_GRENADE_LAUNCHER:
	case WP_ROCKET_LAUNCHER:
//#ifdef MISSIONPACK
	case WP_NAILGUN:
	case WP_PROX_LAUNCHER:
//#endif
		CG_MissileHitWall( weapon, 0, origin, dir, IMPACTSOUND_FLESH );
		break;
	default:
		break;
	}
}

/*
============================================================================

SHOTGUN TRACING

============================================================================
*/

/*
================
CG_ShotgunPellet
================
*/
static void CG_ShotgunPellet( vec3_t start, vec3_t end, int skipNum )
{
	trace_t		tr;
	int sourceContentType, destContentType;

// LEILEI ENHACNEMENT
	vec3_t  kapow;



	CG_Trace( &tr, start, NULL, NULL, end, skipNum, MASK_SHOT );

	sourceContentType = CG_PointContents( start, 0 );
	destContentType = CG_PointContents( tr.endpos, 0 );

	// FIXME: should probably move this cruft into CG_BubbleTrail
	if ( sourceContentType == destContentType ) {
		if ( sourceContentType & CONTENTS_WATER ) {
			CG_BubbleTrail( start, tr.endpos, 32 );
		}
	}
	else if ( sourceContentType & CONTENTS_WATER ) {
		trace_t trace;

		trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
		CG_BubbleTrail( start, trace.endpos, 32 );
	}
	else if ( destContentType & CONTENTS_WATER ) {
		trace_t trace;

		trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
		CG_BubbleTrail( tr.endpos, trace.endpos, 32 );
	}

	if (  tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	if ( cg_entities[tr.entityNum].currentState.eType == ET_PLAYER ) {
		CG_MissileHitPlayer( WP_SHOTGUN, tr.endpos, tr.plane.normal, tr.entityNum );
	} else {
		if ( tr.surfaceFlags & SURF_NOIMPACT ) {
			// SURF_NOIMPACT will not make a flame puff or a mark
			return;
		}
		if ( tr.surfaceFlags & SURF_METALSTEPS ) {
			CG_MissileHitWall( WP_SHOTGUN, 0, tr.endpos, tr.plane.normal, IMPACTSOUND_METAL );
		}
		else {
			CG_MissileHitWall( WP_SHOTGUN, 0, tr.endpos, tr.plane.normal, IMPACTSOUND_DEFAULT );
		}
	}
}

/*
================
CG_ShotgunPattern

Perform the same traces the server did to locate the
hit splashes
================
*/
//unlagged - attack prediction
// made this non-static for access from cg_unlagged.c
void CG_ShotgunPattern( vec3_t origin, vec3_t origin2, int seed, int otherEntNum )
{
	int			i;
	float		r, u;
	vec3_t		end;
	vec3_t		forward, right, up;

	// derive the right and up vectors from the forward vector, because
	// the client won't have any other information
	VectorNormalize2( origin2, forward );
	PerpendicularVector( right, forward );
	CrossProduct( forward, right, up );

	// generate the "random" spread pattern
	for ( i = 0 ; i < DEFAULT_SHOTGUN_COUNT ; i++ ) {
		r = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		u = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		VectorMA( origin, 8192 * 16, forward, end);
		VectorMA (end, r, right, end);
		VectorMA (end, u, up, end);

		CG_ShotgunPellet( origin, end, otherEntNum );
	}
}

/*
==============
CG_ShotgunFire
==============
*/
void CG_ShotgunFire( entityState_t *es )
{
	vec3_t	v;
	int		contents;

	VectorSubtract( es->origin2, es->pos.trBase, v );
	VectorNormalize( v );
	VectorScale( v, 32, v );
	VectorAdd( es->pos.trBase, v, v );
	if ( cgs.glconfig.hardwareType != GLHW_RAGEPRO ) {
		// ragepro can't alpha fade, so don't even bother with smoke
		vec3_t			up;

		contents = CG_PointContents( es->pos.trBase, 0 );
		if ( !( contents & CONTENTS_WATER ) ) {
			VectorSet( up, 0, 0, 8 );
		}
		else {
			CG_SmokePuff( v, up, 32, 1, 1, 1, 0.33f, 900, cg.time, 0, LEF_PUFF_DONT_SCALE, cgs.media.shotgunSmokePuffShader );
		}

	}
	CG_ShotgunPattern( es->pos.trBase, es->origin2, es->eventParm, es->otherEntityNum );
}

/*
============================================================================

BULLETS

============================================================================
*/


/*
===============
CG_Tracer
===============
*/
void CG_Tracer( vec3_t source, vec3_t dest )
{
	vec3_t		forward, right;
	polyVert_t	verts[4];
	vec3_t		line;
	float		len, begin, end;
	vec3_t		start, finish;
	vec3_t		midpoint;

	// tracer
	VectorSubtract( dest, source, forward );
	len = VectorNormalize( forward );

	// start at least a little ways from the muzzle
	if ( len < 100 ) {
		return;
	}
	begin = 50 + random() * (len - 60);
	end = begin + cg_tracerLength.value;
	if ( end > len ) {
		end = len;
	}
	VectorMA( source, begin, forward, start );
	VectorMA( source, end, forward, finish );

	line[0] = DotProduct( forward, cg.refdef.viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef.viewaxis[2] );

	VectorScale( cg.refdef.viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef.viewaxis[2], right );
	VectorNormalize( right );

	VectorMA( finish, cg_tracerWidth.value, right, verts[0].xyz );
	verts[0].st[0] = 0;
	verts[0].st[1] = 1;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	VectorMA( finish, -cg_tracerWidth.value, right, verts[1].xyz );
	verts[1].st[0] = 1;
	verts[1].st[1] = 0;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	VectorMA( start, -cg_tracerWidth.value, right, verts[2].xyz );
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	VectorMA( start, cg_tracerWidth.value, right, verts[3].xyz );
	verts[3].st[0] = 0;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;

	trap_R_AddPolyToScene( cgs.media.tracerShader, 4, verts );

	midpoint[0] = ( start[0] + finish[0] ) * 0.5;
	midpoint[1] = ( start[1] + finish[1] ) * 0.5;
	midpoint[2] = ( start[2] + finish[2] ) * 0.5;

	// add the tracer sound
	trap_S_StartSound( midpoint, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.tracerSound );

}


/*
======================
CG_CalcMuzzlePoint
======================
*/
static qboolean	CG_CalcMuzzlePoint( int entityNum, vec3_t muzzle )
{
	vec3_t		forward;
	centity_t	*cent;
	int			anim;

	if ( entityNum == cg.snap->ps.clientNum ) {
		VectorCopy( cg.snap->ps.origin, muzzle );
		muzzle[2] += cg.snap->ps.viewheight;
		AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
		VectorMA( muzzle, 14, forward, muzzle );
		return qtrue;
	}

	cent = &cg_entities[entityNum];
	if ( !cent->currentValid ) {
		return qfalse;
	}

	VectorCopy( cent->currentState.pos.trBase, muzzle );

	AngleVectors( cent->currentState.apos.trBase, forward, NULL, NULL );
	anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	if ( anim == LEGS_WALKCR || anim == LEGS_IDLECR ) {
		muzzle[2] += CROUCH_VIEWHEIGHT;
	}
	else {
		if (cg_enableQ.integer)
			muzzle[2] += QUACK_VIEWHEIGHT;
		else
			muzzle[2] += DEFAULT_VIEWHEIGHT;
	}

	VectorMA( muzzle, 14, forward, muzzle );

	return qtrue;

}

/*
======================
CG_Bullet

Renders bullet effects.
======================
*/
void CG_Bullet( vec3_t end, int sourceEntityNum, vec3_t normal, qboolean flesh, int fleshEntityNum )
{
	trace_t trace;
	int sourceContentType, destContentType;
	vec3_t	start;

	// if the shooter is currently valid, calc a source point and possibly
	// do trail effects
	if ( sourceEntityNum >= 0 && cg_tracerChance.value > 0 ) {
		if ( CG_CalcMuzzlePoint( sourceEntityNum, start ) ) {
			sourceContentType = CG_PointContents( start, 0 );
			destContentType = CG_PointContents( end, 0 );

			// do a complete bubble trail if necessary
			if ( ( sourceContentType == destContentType ) && ( sourceContentType & CONTENTS_WATER ) ) {
				CG_BubbleTrail( start, end, 32 );
			}
			// bubble trail from water into air
			else if ( ( sourceContentType & CONTENTS_WATER ) ) {
				trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
				CG_BubbleTrail( start, trace.endpos, 32 );
			}
			// bubble trail from air into water
			else if ( ( destContentType & CONTENTS_WATER ) ) {
				trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
				CG_BubbleTrail( trace.endpos, end, 32 );
			}

			// draw a tracer
			if ( random() < cg_tracerChance.value ) {
				CG_Tracer( start, end );
			}
		}
	}
	// impact splash and mark
	if ( flesh ) {
                CG_Bleed( end, fleshEntityNum );
	} else {
		CG_MissileHitWall( WP_MACHINEGUN, 0, end, normal, IMPACTSOUND_DEFAULT );
	}
}

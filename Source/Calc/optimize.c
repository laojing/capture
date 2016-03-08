#include "../Lib/persist.h"
#include "optimize.h"

gint oldPowerTime = 0;
gint gPowerSpan = 60;
gint gPowerDuration = 7200;

gint gPitchStart = 1447084800;
gint gPitchSpan = 60*60*24*7;
gint gPitchHist = 0;

gint oldFollowTime = 0;
gint gFollowSpan = 60;

gint oldWindTime = 0;
gint gWindSpan = 60;
gint gWindDuration = 7200;

void CalcOptimize ( gint curtime ) {
	gint len = 0;
	// Power
	if ( curtime - oldPowerTime > gPowerSpan ) {
		TenData **tens = GetTenData( curtime, gPowerDuration, &len );
		if ( len > 0 ) OptPower ( tens, len );
		//释放内存
		for ( int i=0; i<len; i++ ) {
			g_slice_free ( TenData, tens[i] );
		}
		free ( tens );
		oldPowerTime = curtime;
	}

	// Pitch
	if( gPitchHist == 0 ) gPitchHist = (curtime / gPitchSpan) * gPitchSpan;
	gint curpitch = (curtime / gPitchSpan) * gPitchSpan;
	if( HasPitchData ( curpitch ) != 66 ) {
		len = 0;
		TenData **tens = GetTenData( curpitch-gPitchSpan, curpitch, &len );
		if ( len > 0 ) OptPitch ( tens, len, curpitch );
		//释放内存
		for ( int i=0; i<len; i++ ) {
			g_slice_free ( TenData, tens[i] );
		}
		free ( tens );
	}
	if( gPitchHist > gPitchStart
		&& HasPitchData ( gPitchHist ) != 66 ) {
		len = 0;
		TenData **tens = GetTenData( gPitchHist-gPitchSpan, gPitchHist, &len );
		if ( len > 0 ) OptPitch ( tens, len, gPitchHist );
		//释放内存
		for ( int i=0; i<len; i++ ) {
			g_slice_free ( TenData, tens[i] );
		}
		free ( tens );
	}
	if( gPitchHist > gPitchStart ) gPitchHist -= gPitchSpan;

	// Follow
	if ( curtime - oldFollowTime > gFollowSpan ) {
		len = 0;
		TenData **tens = GetTenData( curtime, CALCSPAN, &len );
		if ( len > 0 ) OptFollow ( tens, len );
		//释放内存
		for ( int i=0; i<len; i++ ) {
			g_slice_free ( TenData, tens[i] );
		}
		free ( tens );
		oldFollowTime = curtime;
	}

	// Wind
	if ( curtime - oldWindTime > gWindSpan ) {
		TenData **tens = GetTenData( curtime, gWindDuration, &len );

		if ( len > 0 ) OptWind ( tens, len );

		//释放内存
		for ( int i=0; i<len; i++ ) {
			g_slice_free ( TenData, tens[i] );
		}
		free ( tens );
		oldWindTime = curtime;
	}

}

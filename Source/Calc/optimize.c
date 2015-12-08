#include "../Lib/persist.h"
#include "optimize.h"

gint gPowerSpan = 60;
gint gPowerDuration = 7200;

gint gFollowSpan = 60;

gint gPitchStart = 1437321600;
gint gPitchSpan = 60*60*24*7;

gint gWindSpan = 60;
gint gWindDuration = 7200;

gint oldPowerTime = 0;
gint oldFollowTime = 0;
gint oldWindTime = 0;

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

	// Pitch
	if ( curtime - gPitchStart > gPitchSpan ) {
		len = 0;
		TenData **tens = GetTenData( gPitchStart+gPitchSpan, gPitchSpan, &len );
		if ( len > 0 ) OptPitch ( tens, len, gPitchStart+gPitchSpan );

		//释放内存
		for ( int i=0; i<len; i++ ) {
			g_slice_free ( TenData, tens[i] );
		}
		free ( tens );
		gPitchStart += gPitchSpan;
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

#include <math.h>
#include "../Lib/persist.h"
#include "optimize.h"

gint gPitch1fTrqMin = 200000;
gint gPitch1fTrqMax = 400000;
gint gPitch1fSpdMin = 11.5;
gint gPitch1fSpdMax = 17;
gint gPitch2fTrqMin = 1500;
gint gPitch2fTrqMax = 3500;
gint gPitch2fSpdMin = 1100;
gint gPitch2fSpdMax = 1750;

void 
OptPitch ( TenData **tens, gint len, gint savetime ) {
	printf ( "Pitch Running...\n" ); fflush ( stdout );
	gfloat *torque = (gfloat*)(g_malloc0 ( sizeof(gfloat) * len ));

	gfloat gains[66];
	gint gainsnum[66];
	gfloat deno[66];
	gfloat mole[66];

	for ( int i=0; i<66; i++ ) {
		gainsnum[i] = 0, deno[i] = 0, mole[i] = 0;
	}

	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->Speed > 4 ) {
			torque[i] = 9550*tens[i]->Power / tens[i]->Speed;

			if ( ( tens[i]->turbnum >= 1 && tens[i]->turbnum <= 33 
						&& torque[i]>gPitch1fTrqMin && torque[i]<gPitch1fTrqMax 
						&& tens[i]->Speed>gPitch1fSpdMin && tens[i]->Speed<gPitch1fSpdMax ) 
			|| ( tens[i]->turbnum >= 33 && tens[i]->turbnum <= 66 
				&& torque[i]>gPitch2fTrqMin && torque[i]<gPitch2fTrqMax
				&& tens[i]->Speed>gPitch2fSpdMin && tens[i]->Speed<gPitch2fSpdMax ) ) { 
				gainsnum[tens[i]->turbnum-1]++;
				deno[tens[i]->turbnum-1] += pow( tens[i]->Speed, 4.0 );
				mole[tens[i]->turbnum-1] += torque[i] * pow( tens[i]->Speed, 2.0 );
			}
		}
	}

	for ( int i=0; i<66; i++ ) {
		if ( gainsnum[i] > 0 && deno[i] != 0 ) {
			gains[i] = mole[i] / deno[i];
		} else {
			gains[i] = 0;
		}
	}

	SavePitchs ( gains, savetime );

	g_free ( torque );
	printf ( "Pitch Done\n" ); fflush ( stdout );
}


#include <math.h>
#include "../Lib/persist.h"
#include "optimize.h"

float def1[] = {20.85,45.34,75.55,111.15,154.9,209.2,274.26,351.19,440.93,544.81,663.46,794.22,929.41,1070.26,1212.07,1353.78,1491.7,1500,1500};
float def2[] = {4,27,50,86,126,181,243,315,402,502,620,739,886,1027,1187,1326,1435,1478,1500};

int isGood( float wind, float power ) {
	int start = (wind - 3)*2;
	int curpower = 0.0;
	// 小于3m/s风速时
	if( start < 0 ) {
		curpower = 0.0;
	}else if( start+1 >= 19 ) {
		curpower = 1500.0;
	}else {
		curpower = def1[start] + (def1[start+1] - def1[start])*2*(wind-3-start/2);
	}
	if( power < curpower*0.9 || power > curpower*1.1 ) {
		return 0;
	} else {
		return 1;
	}
}

void OptPower ( TenData **tens, gint len ) {
	printf ( "Power Running...\n" ); fflush ( stdout );
	PowerTurb turbs[66];
	for ( int i=0; i<66; i++ ) {
		turbs[i].effi = 0.0;
		for ( int j=0; j<50; j++ ) {
			turbs[i].bins[j].num = 0;
			turbs[i].bins[j].w = 0;
			turbs[i].bins[j].p = 0;
		}
	}
	for ( int i=0; i<len; i++ ) {
		int index = tens[i]->Wind*2;
		if( index<50 && isGood( tens[i]->Wind, tens[i]->Power ) ) {
			turbs[tens[i]->turbnum-1].bins[index].num++;
			turbs[tens[i]->turbnum-1].bins[index].w += tens[i]->Wind;
			turbs[tens[i]->turbnum-1].bins[index].p += tens[i]->Power;
		}
	}

	PowerTurb avg;
	for ( int j=0; j<50; j++ ) {
		avg.bins[j].num = 0;
		avg.bins[j].w = j*0.5;
		avg.bins[j].p = 0;
	}
	for ( int i=0; i<66; i++ ) {
		for ( int j=0; j<50; j++ ) {
			if ( turbs[i].bins[j].num > 0 ) {
				turbs[i].bins[j].w /= turbs[i].bins[j].num;
				turbs[i].bins[j].p /= turbs[i].bins[j].num;
			}
			if ( turbs[i].bins[j].p > 0 ) {
				avg.bins[j].num ++;
				avg.bins[j].w += turbs[i].bins[j].w;
				avg.bins[j].p += turbs[i].bins[j].p;
			}
		}
	}
	for ( int j=0; j<50; j++ ) {
		if ( avg.bins[j].num > 0 ) {
			avg.bins[j].w /= avg.bins[j].num;
			avg.bins[j].p /= avg.bins[j].num;
		}
	}

	// Calc Effi
	for ( int i=0; i<66; i++ ) {
		int num = 0;
		for ( int j=0; j<50; j++ ) {
			if ( turbs[i].bins[j].p > 0 ) {
//				turbs[i].effi += pow( (turbs[i].bins[j].p - avg.bins[j].p)/avg.bins[j].p, 2 );
				turbs[i].effi += (turbs[i].bins[j].p - avg.bins[j].p)/avg.bins[j].p;
				num++;
			}
		}
//		if ( num > 0 ) turbs[i].effi = sqrt( turbs[i].effi/num );
		if ( num > 0 ) turbs[i].effi /=  num;
		else turbs[i].effi = -123.456;
	}
	SaveEffi( turbs, 66 );

	printf ( "Power Done\n" ); fflush ( stdout );
}

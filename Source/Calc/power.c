#include "../Lib/persist.h"
#include "optimize.h"

void CalcBin ( TenData **datas, gint len, PowerTurb *turb, gint turbnum ) {
	// 抽取出每台风机对应的数据
	for ( int i=0; i<len; i++ ) {
		if ( datas[i]->turbnum == turbnum ) {
			int index = datas[i]->Wind*2;
			turb->bins[index].winds[turb->bins[index].num] = datas[i]->Wind;
			turb->bins[index].powers[turb->bins[index].num++] = datas[i]->Power;
		}
	}
	for ( int j=0; j<50; j++ ) {
		float sumw = 0;
		float sump = 0;
		for ( int i=0; i<turb->bins[j].num; i++ ) {
			sumw += turb->bins[j].winds[i];
			sump += turb->bins[j].powers[i];
		}
		if ( turb->bins[j].num > 0 ) {
			turb->bins[j].w = sumw / turb->bins[j].num;
			turb->bins[j].p = sump / turb->bins[j].num;
		}
	}
}

void OptPower ( TenData **tens, gint len ) {
	printf ( "Power Running...\n" ); fflush ( stdout );
	PowerTurb turbs[66];
	for ( int i=0; i<66; i++ ) {
		turbs[i].effi = 0.0;
		for ( int j=0; j<50; j++ ) {
			turbs[i].bins[j].num = 0;
			turbs[i].bins[j].w = j*0.5;
			turbs[i].bins[j].p = 0;
			turbs[i].bins[j].winds = (float*)malloc(sizeof(float)*len);
			turbs[i].bins[j].powers = (float*)malloc(sizeof(float)*len);
		}
		CalcBin ( tens, len, turbs+i, i+1 );
	}

	PowerTurb avg;
	for ( int j=0; j<50; j++ ) {
		avg.bins[j].num = 0;
		avg.bins[j].w = j*0.5;
		avg.bins[j].p = 0;
	}
	for ( int i=0; i<66; i++ ) {
		for ( int j=0; j<50; j++ ) {
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
				turbs[i].effi += ( turbs[i].bins[j].p - avg.bins[j].p ) / avg.bins[j].p;
				num++;
			}
		}
		if ( num > 0 ) turbs[i].effi /= num;
		else turbs[i].effi = -123.456;
	}
	SaveEffi( turbs, 66 );

	//释放内存
	for ( int i=0; i<66; i++ ) {
		for ( int j=0; j<50; j++ ) {
			free ( turbs[i].bins[j].winds );
			free ( turbs[i].bins[j].powers );
		}
	}
	printf ( "Power Done\n" ); fflush ( stdout );
}

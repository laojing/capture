#include "capdata.h"
#include "persist.h"
#include "common.h"

#ifndef linux
#include <windows.h>
#include "dnaapi.h"
#include "ezdnaapi.h"
#endif

// 统计数据个数
#define F1NUM   15
#define F2NUM   15

int f1Data[] = {	2,	7,	8,	3,	4,	5,	15,	16,	17,	11,	18,	19,	20, 25,	24 };
int f2Data[] = {	5,	9,	10,	12,	13,	14,	15,	16,	17,	21,	18,	19,	20,	6,	7 };
gchar *names[] = {"wind", "power", "var", "voltagea", "voltageb", "voltagec", "currenta", "currentb", "currentc", "speed", "pitch1", "pitch2", "pitch3", "direct", "position"};

static float f1Temp[F1NUM];
static float f2Temp[F2NUM];
static float total[] = { 0, 0, 0, 0, 0, 0 };

	/*
	// 初始化，一般不用	
    int nRet = DnaClientInit (0);
    nRet = DnaClientSvcDirConnect ("10.69.20.214.service");
    nRet = DnaClientStop ();
	*/
#ifdef linux
int curindex = 0;
int totalval = 0;
double values[600];

gchar* GetName( gint turb, gint val ) {
	if( turb <= 33 ) {
		for( int i=0; i<F1NUM; i++ ) {
			if( f1Data[i] == val ){
				return names[i];
			}
		}
	} else {
		for( int i=0; i<F2NUM; i++ ) {
			if( f2Data[i] == val ){
				return names[i];
			}
		}
	}
}

int dispnum = 0;
int DnaGetHistAvgUTC ( gchar* szPoint, int start, int end, int span, unsigned long *pulKey ) {
	int farm = szPoint[15] - '1';
	int turb = atoi(g_strndup(szPoint+17,2)) + farm*33;
	int val = atoi(g_strndup(szPoint+21,2));
	if( (turb>0 && turb<8)
		|| (turb>33 && turb<36)
		|| (turb>36 && turb<41)
		|| (turb>47 && turb<54) ) {

		values[0] = GetRealAvgValue( turb, GetName(turb, val), start, end );
		curindex = 0;
		totalval = 1;
	} else {
		// 此风机没有数据
		curindex = 0;
		totalval = 1;
		values[0] = 0.0;
	}

	return 0;
}

int DnaGetHistRawUTC ( gchar* szPoint, int start, int end, unsigned long *pulKey ) {
	int farm = szPoint[15] - '1';
	int turb = atoi(g_strndup(szPoint+17,2)) + farm*33;
	int val = atoi(g_strndup(szPoint+21,2));
	if( (turb>0 && turb<8)
		|| (turb>33 && turb<36)
		|| (turb>36 && turb<41)
		|| (turb>47 && turb<54) ) {
		totalval = GetRealRawValue( turb, GetName(turb, val), start, end, values );
		curindex = 0;
	} else {
		// 此风机没有数据
		curindex = 0;
		totalval = 1;
		if( val == 6 ) values[0] = 0.0;
		else values[0] = 180.0;
	}
	return 0;
}
int DnaGetNextHistUTC ( unsigned long pulKey, double *pdValue, long *ptTime, gchar* szStatus, unsigned short nStatus ) {
	*pdValue = values[curindex];
	if( curindex++ >= totalval ) return 1;
	else return 0;
}

#endif

float 
GetRealData ( int turb, int var, int tentime ) {
	int farm = 1;
	if ( turb > 33 ) {
		farm = 2;
		turb-= 33;
	}

	gchar *szPoint = g_strdup_printf ( "DBXNY.DLTSUNIV.%dF%02dA%03d", farm, turb, var );

	double pdValue = 0;
	long ptTime = 0;
	char szStatus[32];
	unsigned short nStatus = 32;
	unsigned long pulKey = 0;

	if( (farm == 1 && (var == 24 || var == 25) ) 
		|| (farm == 2 && (var == 6 || var == 7) ) ) {
		int ret = DnaGetHistRawUTC ( szPoint, tentime-599, tentime, &pulKey );
		int total = 0;
		float value = 0;

		// 金凤误差角或东汽位置
		if( var == 25 || var == 7 ) {
			while ( ret == 0 ) {
				ret = DnaGetNextHistUTC ( pulKey, &pdValue, &ptTime, szStatus, nStatus );
				if ( ret == 0 ) {
					value += pdValue - 180;
					total++;
				}
			}
			return total>0?value/total:0;
		// 金凤位置
		}else if( var == 24 ) {
			while ( ret == 0 ) {
				ret = DnaGetNextHistUTC ( pulKey, &pdValue, &ptTime, szStatus, nStatus );
				if ( ret == 0 ) {
					while( pdValue < 0 ) pdValue += 360;
					while( pdValue > 360 ) pdValue -= 360;
					value += pdValue - 180;
					total++;
				}
			}
			return total>0?value/total:0;
		// 东汽误差角
		}else if( var == 6 ) {
			while ( ret == 0 ) {
				ret = DnaGetNextHistUTC ( pulKey, &pdValue, &ptTime, szStatus, nStatus );
				if ( ret == 0 ) {
					if( pdValue > 180 ) pdValue -= 360;
					if( pdValue < -180 ) pdValue += 360;
					value += pdValue;
					total++;
				}
			}
			return total>0?value/total:0;
		}
	} else {
		int ret = DnaGetHistAvgUTC ( szPoint, tentime-599, tentime, 600, &pulKey );
		while( ret == 0 ) {
			ret = DnaGetNextHistUTC( pulKey, &pdValue, &ptTime, szStatus, nStatus );
			if ( ret ==0 ) return pdValue;
		}
	}
	return 0.0;
}

extern int stop;

void CapTenData ( gint tentime ) {
	total[0] = 0; total[1] = 0;
	total[2] = 0; total[3] = 0;
	total[4] = 0; total[5] = 0;
	for ( int turb=1; turb<=33; turb++ ) {

		for ( int var=0; var<F1NUM; var++ ) {
		// 获得实时数据
			f1Temp[var] = GetRealData ( turb, f1Data[var], tentime );
		}
		InsertTenData ( turb, tentime, F1NUM, f1Temp );
//		if ( f1Temp[1] > 10 ) {
			total[0]++;
			total[1] += f1Temp[0];
			total[2] += f1Temp[1];
			total[3] += f1Temp[2];
			total[4] += f1Temp[13];
//		}

	}

	for ( int turb=34; turb<=66; turb++ ) {
		for ( int var=0; var<F2NUM; var++ ) {
		// 获得实时数据
			f2Temp[var] = GetRealData ( turb, f2Data[var], tentime );
		}
		InsertTenData ( turb, tentime, F2NUM, f2Temp );
//		if ( f2Temp[1] > 10 ) {
			total[0]++;
			total[1] += f2Temp[0];
			total[2] += f2Temp[1];
			total[3] += f2Temp[2];
			total[4] += f2Temp[13];
//		}
	}
	if ( total[0] > 0 ) { total[1] /= total[0]; }
	f2Temp[0] = total[1];
	f2Temp[1] = total[2];
	f2Temp[2] = total[3];
	if ( total[4] > 0 ) { total[4] /= total[0]; }
	f2Temp[13] = total[4];
	InsertTenData ( 0, tentime, F1NUM, f2Temp );
}


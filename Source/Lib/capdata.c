#include "capdata.h"
#include "persist.h"

#ifndef linux
#include "dnaapi.h"
#include "ezdnaapi.h"
#endif


	/*
	// 初始化，一般不用	
    int nRet = DnaClientInit (0);
    nRet = DnaClientSvcDirConnect ("10.69.20.214.service");
    nRet = DnaClientStop ();
	*/
#ifndef linux
float 
GetRealData ( int turb, int var, int tentime ) {
	char szPoint[32];
	int farm = 1;
	if ( turb > 33 ) {
		farm = 2;
		turb-= 33;
	}
	strcpy ( szPoint, g_strdup_printf ( "DBXNY.DLTSUNIV.%dF%02dA%03d", farm, turb, var ) );

	unsigned long pulKey = 0;
	int ret = DnaGetHistAvgUTC ( szPoint, tentime-599, tentime, 600, &pulKey );

	double pdValue = 0;
	long ptTime = 0;
	char szStatus[32];
	unsigned short nStatus = 32;

	while( ret == 0 ) {
		ret = DnaGetNextHistUTC( pulKey, &pdValue, &ptTime, szStatus, nStatus );
		if ( ret ==0 ) return pdValue;
	}

	return 0.0;
}
#endif


// 统计数据个数
#define F1NUM   15
#define F2NUM   15

int f1Data[] = {	2,	7,	8,	3,	4,	5,	15,	16,	17,	11,	18,	19,	20, 25,	24 };
int f2Data[] = {	5,	9,	10,	12,	13,	14,	15,	16,	17,	21,	18,	19,	20,	6,	7 };

static float f1Temp[F1NUM];
static float f2Temp[F2NUM];
static float total[] = { 0, 0, 0, 0, 0, 0 };

extern int stop;

void CapTenData ( gint tentime ) {
	total[0] = 0; total[1] = 0;
	total[2] = 0; total[3] = 0;
	total[4] = 0; total[5] = 0;
	for ( int turb=1; turb<=33; turb++ ) {

#ifndef SUTTEST
		GetLocalTenData ( turb, tentime, F1NUM, f1Temp );
#else
		for ( int var=0; var<F1NUM; var++ ) {
		// 获得实时数据
			f1Temp[var] = GetRealData ( turb, f1Data[var], tentime );
		}
#endif
		InsertTenData ( turb, tentime, F1NUM, f1Temp );
		if ( f1Temp[1] > 10 ) {
			total[0]++;
			total[1] += f1Temp[0];
			total[2] += f1Temp[1];
			total[3] += f1Temp[2];
			total[4] += f1Temp[13];
		}

	}

	for ( int turb=34; turb<=66; turb++ ) {
#ifndef SUTTEST
		GetLocalTenData ( turb, tentime, F2NUM, f2Temp );
#else
		for ( int var=0; var<F2NUM; var++ ) {
		// 获得实时数据
			f2Temp[var] = GetRealData ( turb, f2Data[var], tentime );
		}
#endif
		InsertTenData ( turb, tentime, F2NUM, f2Temp );
		if ( f2Temp[1] > 10 ) {
			total[0]++;
			total[1] += f2Temp[0];
			total[2] += f2Temp[1];
			total[3] += f2Temp[2];
			total[4] += f2Temp[13];
		}
	}
	if ( total[0] > 0 ) { total[1] /= total[0]; }
	f2Temp[0] = total[1];
	f2Temp[1] = total[2];
	f2Temp[2] = total[3];
	if ( total[4] > 0 ) { total[4] /= total[0]; }
	f2Temp[13] = total[4];
	InsertTenData ( 0, tentime, F1NUM, f2Temp );
}


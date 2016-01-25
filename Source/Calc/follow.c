#include "../Lib/persist.h"
#include "optimize.h"
#include<math.h>

int lines[5][20] = {
	{11,1,2,3,4,5,6,7,8,9,10,11},
	{11,12,13,14,15,16,17,18,19,20,21,22},
	{11,23,24,25,26,27,28,29,30,31,32,33},
	{15,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{18,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33}
};

void SaveFollowPower ( Follow *fol, int turbnum, float value, float power ) {
	fol->turbnum = turbnum;
	fol->Value = value;
	fol->Power = power;
	fol->Var = value;
}

void SaveFollow ( Follow *fol, int turbnum, float value ) {
	fol->turbnum = turbnum;
	fol->Value = value;
	fol->Power = 0;
	fol->Var = 0;
}

void OptFollow ( TenData **tens, gint len ) {

	float SN = 1600000;
	float UN = 36000;

	float turbLosta = 0;
	float avol = 0;
	float avolcount = 0;
	float acur = 0;
	float realvals = 0;

	float totalpower = 0;

	printf ( "Follow Running...\n" ); fflush ( stdout );
	// 单台风机变压器损耗
	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->turbnum == 0 ) continue;
		if ( tens[i]->Voltagea > 600 ) {
			turbLosta += 0.1/10000*SN + 6.5*3*tens[i]->Voltagea*tens[i]->Currenta*3*tens[i]->Voltagea*tens[i]->Currenta/10000/SN * (UN/tens[i]->Voltagea) * (UN/tens[i]->Voltagea);
			avol += tens[i]->Voltagea;
			acur += tens[i]->Currenta;
			realvals += tens[i]->Var;
			avolcount++;
		}

		if ( tens[i]->Power > 100 ) {
			totalpower += tens[i]->Power*1000;
		}
	}
	avol = avol/avolcount;
	acur = acur/avolcount;
	// 支路损耗
	turbLosta += avol*avol * 50 * 8e-12 * 13700;
	avol = avol * 35000 / 690;
	acur = acur / 35000 * 690;
	// 主线路损耗
	turbLosta += avol*avol * 50 * 8e-12 * 8700;

	SN = 120000000;
	UN = 225000;
	// 主变压器损耗
	turbLosta += 0.4/10000*SN + 12*3*avol*acur*3*avol*acur/10000/SN * (UN/avol) * (UN/avol);

	float deltaq = 1102500000 * (35000 - avol) / 35000;
	float capvar = 0;
	if ( totalpower < 1 ) totalpower = 1;
	Follow fols[100];
	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->turbnum == 0 ) continue;
		int index = tens[i]->turbnum-1;
		fols[index].turbnum = tens[i]->turbnum;
		fols[index].Power = tens[i]->Power;
		fols[index].Var = tens[i]->Var;
		if ( tens[i]->Power > 100 ) {
			float temp = deltaq * tens[i]->Power / totalpower;
			if ( temp > 300 ) {
				capvar += temp - 300;
				fols[index].Value = 300;
			} else if ( temp < -300 ) {
			printf( "%f ", temp );
			fflush( stdout );
				capvar += temp + 300;
				fols[index].Value = -300;
			} else {
				fols[index].Value = temp;
			}
		} else {
			fols[index].Value = 0.0;
		}
	}
	float turbLostb = 0;

	SN = 1600000;
	UN = 36000;

	avol = 0;
	avolcount = 0;
	acur = 0;

	// 单台风机变压器损耗
	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->turbnum == 0 ) continue;
		if ( tens[i]->Voltagea > 600 ) {
			turbLostb += 0.1/10000*SN + 6.5*3*690*tens[i]->Currenta*3*690*tens[i]->Currenta/10000/SN * (UN/690) * (UN/690);
			avol += tens[i]->Voltagea;
			acur += tens[i]->Currenta;
			avolcount++;
		}
	}

	avol = avol/avolcount;
	acur = acur/avolcount;
	// 支路损耗
	turbLostb += 690*690.0 * 50.0 * 8e-12 * 13700;
	avol = avol * 35000 / 690;
	acur = acur / 35000 * 690;
	// 主线路损耗
	turbLostb += 35000.0*35000.0 * 50.0 * 8e-12 * 8700;

	SN = 120000000;
	UN = 225000;
	// 主变压器损耗
	turbLostb += 0.4/10000*SN + 12*3*35000*acur*3*35000*acur/10000/SN * (UN/35000) * (UN/35000);

	// 66 优化前损耗
	// 67 优化前损耗
	if ( turbLosta > turbLostb ) {
		SaveFollow ( fols+66, 100, turbLosta/1000 );
		SaveFollow ( fols+67, 101, turbLostb/1000 );
	} else {
		SaveFollow ( fols+66, 100, turbLostb/1000 );
		SaveFollow ( fols+67, 101, turbLosta/1000 );
	}
	// 68 风场无功补偿
	SaveFollow ( fols+68, 102, capvar );
	// 69 所有机组无功补偿
	SaveFollow ( fols+69, 103, deltaq/1000 - capvar );
	// 70 机组初始无功
	SaveFollow ( fols+70, 104, realvals );
	// 71 风场无功补偿前
	SaveFollow ( fols+71, 105, deltaq/1000 - realvals );



	float itotal[5] = {0,0,0,0,0};
	for( int i=0; i<5; i++ ) {
		for( int j=0; j<lines[i][0]; j++ ) {
			itotal[i] += tens[lines[i][j+1]]->Currenta;
		}
	}

	float itotals = 0;
	float rtotal[5];
	for( int i=0; i<5; i++ ) {
		float r = 0.002516;
		if( i>2 ) r = 0.00646;
		rtotal[i] = r * pow(itotal[i],2);
		itotals += itotal[i];
	}
	float translost = 67.8 + 1.05*377.2*pow( (35*itotals)/SN, 2 );

	float iopts[66];
	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->turbnum == 0 ) continue;
		int index = tens[i]->turbnum-1;
		if( tens[i]->Power > 0 ) {
			iopts[index] = tens[i]->Power / sqrt( pow( tens[i]->Power, 2) + pow( fols[index].Value, 2 ) )
				* tens[i]->Currenta;
		} else {
			iopts[index] = 0;
		}
	}

	float itotalopt[5] = {0,0,0,0,0};
	for( int i=0; i<5; i++ ) {
		for( int j=0; j<lines[i][0]; j++ ) {
			itotalopt[i] += iopts[lines[i][j+1]-1];
		}
	}

	float itotalsopt = 0;
	float rtotalopt[5];
	for( int i=0; i<5; i++ ) {
		float r = 0.002516;
		if( i>2 ) r = 0.00646;
		rtotalopt[i] = r * pow(itotalopt[i],2);
		itotalsopt += itotalopt[i];
	}
	float translostopt = 67.8 + 1.05*377.2*pow( (35*itotalsopt)/SN, 2 );


	int folindex = 72;
	for( int i=0; i<5; i++ ) {
		SaveFollowPower ( fols+folindex++, 201+i, rtotal[i]/1000, rtotalopt[i]/1000 );
		SaveFollow ( fols+folindex++, 301+i, rtotalopt[i]/1000 );
	}
	SaveFollow ( fols+folindex++, 200, translost );
	SaveFollow ( fols+folindex++, 300, translostopt );


	SaveFollows ( fols, folindex );
	printf ( "Follow Done\n" ); fflush ( stdout );
//	printf( "%4.2f:%4.2f ", tens[i]->Currenta, iopts[index] );
//	fflush( stdout );
}

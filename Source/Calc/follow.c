#include "../Lib/persist.h"
#include "optimize.h"

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

	float totalpower = 0;

	printf ( "Follow Running...\n" ); fflush ( stdout );
	// 单台风机变压器损耗
	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->turbnum == 0 ) continue;
		if ( tens[i]->Voltagea > 600 ) {
			turbLosta += 0.1/10000*SN + 6.5*3*tens[i]->Voltagea*tens[i]->Currenta*3*tens[i]->Voltagea*tens[i]->Currenta/10000/SN * (UN/tens[i]->Voltagea) * (UN/tens[i]->Voltagea);
			avol += tens[i]->Voltagea;
			acur += tens[i]->Currenta;
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
	Follow fols[70];
	for ( int i=0; i<len; i++ ) {
		if ( tens[i]->turbnum == 0 ) continue;
		int index = tens[i]->turbnum-1;
		fols[index].Power = tens[i]->Power;
		fols[index].Var = tens[i]->Var;
		if ( tens[i]->Power > 100 ) {
			float temp = deltaq * tens[i]->Power / totalpower;
			if ( temp > 300 ) {
				capvar += temp - 300;
				fols[index].Value = 300;
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

	if ( turbLosta > turbLostb ) {
		SaveFollow ( fols+66, 100, turbLosta/1000 );
		SaveFollow ( fols+67, 101, turbLostb/1000 );
	} else {
		SaveFollow ( fols+66, 100, turbLostb/1000 );
		SaveFollow ( fols+67, 101, turbLosta/1000 );
	}
	SaveFollow ( fols+68, 102, capvar );
	SaveFollow ( fols+69, 103, deltaq/1000 - capvar );
	SaveFollows ( fols, 70 );
	printf ( "Follow Done\n" ); fflush ( stdout );
}

#include <math.h>
#include "../Lib/persist.h"
#include "optimize.h"

gint gWindBeta = 0.3;
gint gWindAlpha = 0.8;

typedef struct _WindBin WindBin;
struct _WindBin
{
	int num;
	float w;
	float p;
};

void CalcFactor ( TenData **tens, gint len, gint n, gint index, gfloat *beta, gfloat *alpha, gfloat *error ) {
	gfloat cur = 0;
	gfloat total = 0;
	for ( int i=0; i<len; i+=67 ) {
		total += tens[i]->Direct;
		cur += tens[i+index]->Direct;
	}
	cur /= 66;
	total /= 66;

	gfloat betanum = 0;
	gfloat betadeno1 = 0;
	gfloat betadeno2 = 0;
	gfloat alphanum = 0;
	gfloat alphadeno = 0;
	for ( int i=0; i<len; i+=67 ) {
		if ( i==0 ) *error = tens[i+index]->Direct - tens[i+index]->Position;
		betanum += (tens[i]->Direct - total)*(tens[i+index]->Direct - cur);
		betadeno1 += (tens[i]->Direct - total)*(tens[i]->Direct - total);
		betadeno2 += (tens[i+index]->Direct - cur)*(tens[i+index]->Direct - cur);

		alphanum += (tens[i+index]->Direct - cur);
		alphadeno += (tens[i+index]->Direct - cur)*(tens[i+index]->Direct - cur);
	}

	if ( betadeno1 && betadeno2 ) *beta = betanum / sqrt(betadeno1*betadeno2);
	if ( alphadeno ) *alpha = alphanum / alphadeno;
}

void OptWind ( TenData **tens, gint len ) {
	printf ( "Wind Running...\n" ); fflush ( stdout );
	gint n = len/67;
	gint state[66];
	gfloat beta[66];
	gfloat alpha[66];
	for ( int i=0; i<66; i++ ) {
		gfloat error = 0;
		beta[i] = 0, alpha[i] = 0;
		CalcFactor ( tens, len, n, i+1, &beta[i], &alpha[i], &error );
		if ( abs(error) > 10 ) {
			if ( beta[i] > gWindBeta ) {
				state[i] = 1;
			} else if ( alpha[i] > gWindAlpha ) {
				state[i] = 2;
			} else {
				state[i] = 3;
			}
		} else {
			state[i] = 0;
		}
	}
	SaveWind ( state, beta, alpha, 66 );
	printf ( "Wind Done\n" ); fflush ( stdout );
}

#include "persist.h"

static gint waiting = 0;

static MYSQL *mysql = NULL;
static void print_error ( gchar *sql ) {
	printf ( "sql:%s\nerror:%s\n", sql, mysql_error(mysql) ); fflush ( stdout );
	mysql_close ( mysql );
	exit ( 1 );
}

void OpenMysql() {
	if ( mysql ) return;
	mysql = g_slice_new0 ( MYSQL );
	mysql_init( mysql );
	mysql_real_connect( mysql,
			"localhost",
			"root", "cai",
			"zdt", 3306,
			NULL, 0 );
	printf ( "Open Mysql\n" ); fflush ( stdout );
}

void CloseMysql() {
	if ( mysql ) {
		mysql_close( mysql );
		mysql = NULL;
		printf ( "Close Mysql\n" ); fflush ( stdout );
	}
}

gint 
HasTenData ( int tentime ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;

	gchar *sql = g_strdup_printf ( "select * from ten where savetime=%d;", tentime );
	if ( mysql_query(mysql, sql) ) print_error(sql);
	g_free ( sql );

	MYSQL_RES *result = mysql_store_result ( mysql );
	unsigned long lengths = mysql_num_rows ( result );
	mysql_free_result ( result );

	waiting = 0;
	return lengths;
}

void 
InsertTenData ( gint turb, gint tentime, gint num, gfloat *temp ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;
	GString *str = g_string_new ( "insert into ten (savetime,turbnum,wind,power,var,voltagea,voltageb,voltagec,currenta,currentb,currentc,speed,pitch1,pitch2,pitch3,direct,position) values(" );
	g_string_append_printf ( str, "%d,%d", tentime, turb ); 
	for ( int i=0; i<num; i++ ) {
		g_string_append_printf ( str, ",%f", temp[i] );
	}
	g_string_append_printf ( str, ");" );
	if ( mysql_query(mysql, str->str) ) print_error(str->str);
	g_string_free ( str, TRUE );
	MYSQL_RES *result = mysql_store_result ( mysql );
	mysql_free_result ( result );
	waiting = 0;
}
void 
DelTenData ( int tentime ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;

	gchar *sql = g_strdup_printf ( "delete from ten where savetime=%d;", tentime );
	if ( mysql_query(mysql, sql) ) print_error(sql);
	g_free ( sql );

	MYSQL_RES *result = mysql_store_result ( mysql );
	mysql_free_result ( result );

	waiting = 0;
}

TenData** GetTenData( gint curtime, gint duration, gint *len ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;
	gchar *sql = g_strdup_printf ( "select savetime,turbnum,wind,power,var,voltagea,voltageb,voltagec,currenta,currentb,currentc,speed,pitch1,pitch2,pitch3,direct,position from ten where savetime>%d and savetime<%d;", 
			curtime - duration, curtime );
	if ( mysql_query(mysql, sql) ) print_error(sql);
	g_free ( sql );
	MYSQL_RES *result = mysql_store_result ( mysql );
	unsigned long lengths = mysql_num_rows ( result );
	*len = lengths;
	TenData **tens = (TenData**)malloc( sizeof(TenData*)*(*len) );
	if ( result == NULL ) print_error("---");
	MYSQL_ROW row;
	gint i = 0;
	while ( row = mysql_fetch_row(result) ) {
		TenData *da = g_slice_new0 ( TenData );
		da->savetime = atoi(row[0]);
		da->turbnum = atoi(row[1]);
		da->Wind = atof(row[2]);
		da->Power = atof(row[3]);
		da->Var = atof(row[4]);
		da->Voltagea = atof(row[5]);
		da->Voltageb = atof(row[6]);
		da->Voltagec = atof(row[7]);
		da->Currenta = atof(row[8]);
		da->Currentb = atof(row[9]);
		da->Currentc = atof(row[10]);
		da->Speed = atof(row[11]);
		da->Pitch1 = atof(row[12]);
		da->Pitch2 = atof(row[13]);
		da->Pitch3 = atof(row[14]);
		da->Direct = atof(row[15]);
		da->Position = atof(row[16]);
		tens[i++] = da;
	}
	mysql_free_result ( result );
	waiting = 0;
	return tens;
}

int haseffi = FALSE;
void SaveEffi( PowerTurb *turbs, gint len ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;

	/*
	if ( haseffi ) {
		GString *str = g_string_new ( "update effi set value=case turbnum " );
		for ( int i=0; i<len; i++ ) {
			g_string_append_printf ( str, "when %d then %f ", i+1, turbs[i].effi );
		}
		g_string_append_printf ( str, " else value end" );
		if ( mysql_query(mysql, str->str) ) print_error(str->str);
		g_string_free ( str, TRUE );
		MYSQL_RES *result = mysql_store_result ( mysql );
		mysql_free_result ( result );
	} else {
		*/
		GString *str = g_string_new ( "insert into effi values" );
		for ( int i=0; i<len; i++ ) {
			if ( i==0 ) g_string_append_printf ( str, "(null,0,%d,%f)", i+1, turbs[i].effi );
			else g_string_append_printf ( str, ",(null,0,%d,%f)", i+1, turbs[i].effi );
		}
		if ( mysql_query(mysql, "delete from effi") ) print_error("delete effi");
		MYSQL_RES *result = mysql_store_result ( mysql );
		mysql_free_result ( result );
		if ( mysql_query(mysql, str->str) ) print_error(str->str);
		g_string_free ( str, TRUE );
		result = mysql_store_result ( mysql );
		mysql_free_result ( result );
		haseffi = TRUE;
//	}
	waiting = 0;
}


int hasfollow = FALSE;
void SaveFollows ( Follow *fols, gint len ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;

	/*
	if ( hasfollow ) {
		GString *str = g_string_new ( "update follow set value=case turbnum " );
		for ( int i=0; i<len; i++ ) {
			g_string_append_printf ( str, "when %d then %f ", i+1, fols[i].Value );
		}
		g_string_append_printf ( str, " else value end" );
		if ( mysql_query(mysql, str->str) ) print_error(str->str);
		g_string_free ( str, TRUE );
		MYSQL_RES *result = mysql_store_result ( mysql );
		mysql_free_result ( result );

		str = g_string_new ( "update follow set power=case turbnum " );
		for ( int i=0; i<len; i++ ) {
			g_string_append_printf ( str, "when %d then %f ", i+1, fols[i].Power );
		}
		g_string_append_printf ( str, " else value end" );
		if ( mysql_query(mysql, str->str) ) print_error(str->str);
		g_string_free ( str, TRUE );
		result = mysql_store_result ( mysql );
		mysql_free_result ( result );


		str = g_string_new ( "update follow set var=case turbnum " );
		for ( int i=0; i<len; i++ ) {
			g_string_append_printf ( str, "when %d then %f ", i+1, fols[i].Var );
		}
		g_string_append_printf ( str, " else value end" );
		if ( mysql_query(mysql, str->str) ) print_error(str->str);
		g_string_free ( str, TRUE );
		result = mysql_store_result ( mysql );
		mysql_free_result ( result );

	} else {
		*/
		GString *str = g_string_new ( "insert into follow (id,version,turbnum,power,var,value) values" );
		for ( int i=0; i<len; i++ ) {
			if ( i==0 ) g_string_append_printf ( str, "(null,0,%d,%f,%f,%f)", fols[i].turbnum, fols[i].Power, fols[i].Var, fols[i].Value );
			else g_string_append_printf ( str, ",(null,0,%d,%f,%f,%f)", fols[i].turbnum, fols[i].Power, fols[i].Var, fols[i].Value );
//			printf("%d|%f=", i, fols[i].Value);
		}
		if ( mysql_query(mysql, "delete from follow") ) print_error("delete follow");
//		MYSQL_RES *result = mysql_store_result ( mysql );
//		mysql_free_result ( result );
		if ( mysql_query(mysql, str->str) ) print_error(str->str);
		g_string_free ( str, TRUE );
		MYSQL_RES *result = mysql_store_result ( mysql );
		mysql_free_result ( result );
		hasfollow = TRUE;
	//}

	waiting = 0;
}

gint 
HasPitchData ( int tentime ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;

	gchar *sql = g_strdup_printf ( "select * from pitch where savetime=%d;", tentime );
	if ( mysql_query(mysql, sql) ) print_error(sql);
	g_free ( sql );

	MYSQL_RES *result = mysql_store_result ( mysql );
	unsigned long lengths = mysql_num_rows ( result );
	mysql_free_result ( result );

	waiting = 0;
	return lengths;
}


void SavePitchs ( gfloat *gains, gint savetime ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;
	GString *str = g_string_new ( "insert into pitch (id,version,turbnum,savetime,optgain) values" );
	for ( int i=0; i<66; i++ ) {
		if ( i==0 ) g_string_append_printf ( str, "(null,0,%d,%d,%f)", i+1, savetime, gains[i] );
		else g_string_append_printf ( str, ",(null,0,%d,%d,%f)", i+1, savetime, gains[i] );
	}
	if ( mysql_query(mysql, str->str) ) print_error(str->str);
	g_string_free ( str, TRUE );
	MYSQL_RES *result = mysql_store_result ( mysql );
	mysql_free_result ( result );

	waiting = 0;
}

void SaveWind ( gint *state, gfloat *beta, gfloat *alpha, gint len ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;

	GString *str = g_string_new ( "insert into wind (id,version,turbnum,state,beta,alpha) values" );
	for ( int i=0; i<len; i++ ) {
		if ( i==0 ) g_string_append_printf ( str, "(null,0,%d,%d,%f,%f)", i+1, state[i], beta[i], alpha[i] );
		else g_string_append_printf ( str, ",(null,0,%d,%d,%f,%f)", i+1, state[i], beta[i], alpha[i] );
	}
	if ( mysql_query(mysql, "delete from wind") ) print_error("delete wind");
	MYSQL_RES *result = mysql_store_result ( mysql );
	mysql_free_result ( result );
	if ( mysql_query(mysql, str->str) ) print_error(str->str);
	g_string_free ( str, TRUE );
	result = mysql_store_result ( mysql );
	mysql_free_result ( result );

	waiting = 0;
}

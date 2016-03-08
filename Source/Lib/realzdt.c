#include "persist.h"

static gint waiting = 0;

static MYSQL *mysql = NULL;
static void print_error ( gchar *sql ) {
	printf ( "sql:%s\nerror:%s\n", sql, mysql_error(mysql) ); fflush ( stdout );
	mysql_close ( mysql );
	exit ( 1 );
}

void OpenMysqlReal() {
	if ( mysql ) return;
	mysql = g_slice_new0 ( MYSQL );
	mysql_init( mysql );
	mysql_real_connect( mysql,
			"localhost",
			"root", "cai",
			"realzdt", 3306,
			NULL, 0 );
	printf ( "Open Mysql\n" ); fflush ( stdout );
}

void CloseMysqlReal() {
	if ( mysql ) {
		mysql_close( mysql );
		mysql = NULL;
		printf ( "Close Mysql\n" ); fflush ( stdout );
	}
}

double GetRealAvgValue( gint turb, gchar* name, gint start, gint end ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;
	double res = 0.0;

	// 2016-01-20号以后数据向前30天
	if( start > 1453219200 ) {
		start -= 86400*60;
		end -= 86400*60;
	}
	gchar *sql = g_strdup_printf( "select avg(%s) from realval%d where savetime>%d and savetime<%d;",
			name, turb, start, end );
	if ( mysql_query(mysql, sql) ) print_error(sql);
	g_free( sql );

	MYSQL_RES *result = mysql_store_result ( mysql );
	MYSQL_ROW row;
	while ( row = mysql_fetch_row(result) ) {
		if( row[0] != NULL )res = atof( row[0] );
	}
	mysql_free_result ( result );

	waiting = 0;
	return res;
}

gint GetRealRawValue( gint turb, gchar* name, gint start, gint end, double *values ) {
	while ( waiting ) g_usleep ( 1 );
	waiting = 1;
	gint res = 0;

	// 2016-01-20号以后数据向前30天
	if( start > 1453219200 ) {
		start -= 86400*60;
		end -= 86400*60;
	}
	gchar *sql = g_strdup_printf( "select %s from realval%d where savetime>%d and savetime<%d;",
			name, turb, start, end );
	if ( mysql_query(mysql, sql) ) print_error(sql);
	g_free( sql );

	MYSQL_RES *result = mysql_store_result ( mysql );
	MYSQL_ROW row;
	while ( row = mysql_fetch_row(result) ) {
		if( res < 600 ) values[res++] = atof( row[0] );
	}
	mysql_free_result ( result );

	waiting = 0;
	return res;
}

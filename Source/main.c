#include "Lib/common.h"
#include "Lib/persist.h"
#include "Lib/capdata.h"
#include "Calc/optimize.h"
#include "main.h"

int stop = 0;

void 
MainLoop ( void ) {
	int readneed = 1;
	gint histtime = time(NULL);
	histtime = (histtime / CALCSPAN) * CALCSPAN;
	OpenMysql ();
#ifdef linux
	OpenMysqlReal ();
#endif
	gint num =  HasTenData(histtime);
	while ( !stop ) {

		int curtime = time(NULL);

		gchar *dstr = getIntDate2Str ( histtime );
		// History Ten Data
		gint num =  HasTenData(histtime);
		if ( num == 0 ) {
			printf ( "History TenTime Data:%s\n", dstr ), fflush ( stdout );
			CapTenData ( histtime );
		} else if ( num != 67 ) {
			printf ( "History TenTime Bad Data:%s\n", dstr ), fflush ( stdout );
			DelTenData ( histtime );
			printf ( "Delete TenTime Bad Data:%s\n", dstr ), fflush ( stdout );
			CapTenData ( histtime );
			printf ( "Cap TenTime Bad Data:%s\n", dstr ), fflush ( stdout );
		}
		g_free ( dstr );
		if ( histtime > 1446307200 ) histtime -= 600;

		// Current Ten Data
		int tentime = (curtime / CALCSPAN) * CALCSPAN;
		if ( curtime - tentime > 0 && curtime - tentime < CALCSPAN/3 && readneed && tentime != (histtime+600) ) {
			if ( HasTenData(tentime) ) readneed = 0;
			else {
				gchar *dstr = getIntDate2Str ( curtime );
				printf ( "Cur TenTime Data:%s\n", dstr ), fflush ( stdout );
				g_free ( dstr );
				CapTenData ( tentime );
			}
		} else if ( curtime - tentime > CALCSPAN/3 ) {
			readneed = 1;
		}

		// Optimize
		CalcOptimize ( curtime );

//		g_usleep ( SECOND*100 );
		g_usleep ( SECOND );
	}
	CloseMysql ();
#ifdef linux
	CloseMysqlReal ();
#endif
}

int
main () {

    GThread *mainloop = g_thread_new ( "MainLoop", (GThreadFunc)MainLoop, NULL );
	while ( getchar() != 'a' ) g_usleep ( SECOND );
	stop = 1;
	g_thread_join ( mainloop );

	return 0;
}

#include "Lib/persist.h"
#include "main.h"

int stop = 0;

void 
MainLoop ( void ) {
	char szTime[32];
	int readneed = 1;
	gint histtime = time(NULL);
	histtime = (histtime / CALCSPAN) * CALCSPAN;
	OpenMysql ();
	while ( !stop ) {

		int curtime = time(NULL);
//		UCTToStringTime ( histtime, szTime, 32 );

		/*
		// History Ten Data
		gint num =  HasTenData(histtime);
		if ( num == 0 ) {
			printf ( "History TenTime Data:%s\n", szTime ), fflush ( stdout );
			CapTenData ( histtime );
		} else if ( num != 67 ) {
			printf ( "History TenTime Bad Data:%s\n", szTime ), fflush ( stdout );
			DelTenData ( histtime );
			CapTenData ( histtime );
		//} else {
			//printf ( "History TenTime Ready %d, Data:%d\n", num, histtime ), fflush ( stdout );
		}
		if ( histtime >= 1437147000 ) histtime -= 600;

		// Current Ten Data
		int tentime = (curtime / CALCSPAN) * CALCSPAN;
		if ( curtime - tentime > 0 && curtime - tentime < CALCSPAN/3 && readneed && tentime != (histtime+600) ) {
			UCTToStringTime ( tentime, szTime, 32 );
			printf ( "Cur TenTime Data:%s\n", szTime ), fflush ( stdout );
			readneed = 0;
			CapTenData ( tentime );
		} else if ( curtime - tentime > CALCSPAN/3 ) {
			readneed = 1;
		}

		// Optimize
		CalcOptimize ( curtime );
		*/

//		g_usleep ( SECOND*100 );
		g_usleep ( SECOND );
	}
	CloseMysql ();
}

int
main () {

    GThread *mainloop = g_thread_new ( "MainLoop", (GThreadFunc)MainLoop, NULL );
	while ( getchar() != 'a' ) g_usleep ( SECOND );
	stop = 1;
	g_thread_join ( mainloop );

	return 0;
}

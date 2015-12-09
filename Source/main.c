#include "Lib/common.h"
#include "Lib/persist.h"
#include "Lib/capdata.h"
#include "main.h"

#ifdef linux
#else
#include <windows.h>
#include "ezdnaapi.h"
#endif

int stop = 0;

void 
MainLoop ( void ) {
	int readneed = 1;
	gint histtime = time(NULL);
	histtime = (histtime / CALCSPAN) * CALCSPAN;
	OpenMysql ();
	gint num =  HasTenData(histtime);
	while ( !stop ) {

		int curtime = time(NULL);
		gchar *dstr = getIntDate2Str ( histtime );
//		printf ( "==:%s\n", dstr );
//		fflush ( stdout );


		// History Ten Data
		gint num =  HasTenData(histtime);
		if ( num == 0 ) {
			printf ( "History TenTime Data:%s\n", dstr ), fflush ( stdout );
			CapTenData ( histtime );
		} else if ( num != 67 ) {
			printf ( "History TenTime Bad Data:%s\n", dstr ), fflush ( stdout );
			DelTenData ( histtime );
			CapTenData ( histtime );
		//} else {
			//printf ( "History TenTime Ready %d, Data:%d\n", num, histtime ), fflush ( stdout );
		}
		if ( histtime >= 1437147000 ) histtime -= 600;
		/*

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
		g_free ( dstr );
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

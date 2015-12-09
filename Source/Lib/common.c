#include "common.h"

gchar*
getIntDate2Str ( gint64 datetime ) {
	GDateTime *dt = g_date_time_new_from_unix_local ( datetime );
	gchar *res = g_date_time_format ( dt, "%Y-%m-%d %H:%M:%S" );
	g_date_time_unref ( dt );
	return res;
}


//--------------------------------------------------------------------
//
// 函数功能  ：对日期字符串转换为从1970-01-01 00:00:00 UTC开始的秒数。
//             支持格式为：
//				  1. YYYY-mm-DD HH:MM:SS；
//				  2. YYYY-mm-DD HH:MM；
//				  3. YYYY/mm/DD HH:MM:SS；
//				  4. YYYY/mm/DD HH:MM；
//
// 函数参数  ：gchar* strdate : 日期字符串。
//
// 返回值    ：gint64 : 对应秒数或0(非法日期格式)。
//
// 用法示例  ：getStrDate2Int ( "2014-01-01 00:00:00" );
//             返回 : 1388505600
//
// 修改记录  ：
//
//--------------------------------------------------------------------
gint64
getStrDate2Int ( gchar *strdate ) {
	gchar **datetime = g_strsplit ( strdate, " ", 0 );
	GDateTime *dt = NULL;
	if ( g_strv_length(datetime) == 2 ) {
		gint year, month, day, hour, minute, second;
		gchar **dates2 = g_strsplit ( datetime[0], "/", 0 );
		if ( g_strv_length(dates2) == 3 ) {
			year = 2000 + atoi ( dates2[2] );
			month = atoi ( dates2[0] ) + 1;
			day = atoi ( dates2[1] );
		} else {
			g_strfreev ( dates2 );
			g_strfreev ( datetime );
			return 0;
		} 
		gchar **times = g_strsplit ( datetime[1], ":", 0 );
		if ( g_strv_length(times) == 3 ) {
			hour = atoi ( times[0] );
			minute = atoi ( times[1] );
			second = atoi ( times[2] );
		} else if ( g_strv_length(times) == 2 ) {
			hour = atoi ( times[0] );
			minute = atoi ( times[1] );
			second = 0;
		} else {
			g_strfreev ( times );
			g_strfreev ( dates2 );
			g_strfreev ( datetime );
			return 0;
		}
		dt = g_date_time_new_local ( year, month, day, hour, minute, second );
		g_strfreev ( dates2 );
		g_strfreev ( times );
		g_strfreev ( datetime );
		gint64 res = g_date_time_to_unix ( dt );
		g_date_time_unref ( dt );
		return res;
	}
	g_strfreev ( datetime );
	return 0;
}



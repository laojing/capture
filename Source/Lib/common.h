#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

gint64 getStrDate2Int ( gchar *strdate );
gchar* getIntDate2Str ( gint64 datetime );

#endif

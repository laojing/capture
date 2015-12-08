#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "../main.h"

void CalcOptimize ( gint curtime );
void OptPower ( TenData **tens, gint len );
void OptFollow ( TenData **tens, gint len );
void OptPitch ( TenData **tens, gint len, gint savetime );
void OptWind ( TenData **tens, gint len );


#endif

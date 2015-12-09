#ifndef PERSIST_H
#define PERSIST_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#ifdef linux
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

void OpenMysql();
void CloseMysql();

gint HasTenData ( int tentime );
void DelTenData ( int tentime );
void GetLocalTenData ( int turb, int tentime, int num, float *f1Temp );
void InsertTenData ( gint turb, gint tentime, gint num, gfloat *temp );

#endif

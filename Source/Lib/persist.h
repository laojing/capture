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


#endif

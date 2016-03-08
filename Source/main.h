#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <time.h>

// 测试标志
//#define SUTTEST

#define SECOND            1000000
// 统计数据周期10分钟
#define CALCSPAN          600


typedef struct _TenData TenData;
struct _TenData {
	int savetime;
	int turbnum;
	float Wind;
	float Power;
	float Var;
	float Voltagea;
	float Voltageb;
	float Voltagec;
	float Currenta;
	float Currentb;
	float Currentc;
	float Speed;
	float Pitch1;
	float Pitch2;
	float Pitch3;
	float Direct;
	float Position;
};

typedef struct _PowerBin PowerBin;
struct _PowerBin {
	int num;
	float w;
	float p;
};

typedef struct _PowerTurb PowerTurb;
struct _PowerTurb {
	float effi;
	PowerBin bins[50];
};

typedef struct _Follow Follow;
struct _Follow {
	int turbnum;
	float Power;
	float Var;
	float Value;
};

#endif

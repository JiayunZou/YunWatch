#include "common.h"
#include "altitude_display.h"
#include "bme280.h"

HistoryData historydat[12];

extern bool keep_on;

static bool btnExit()
{
	animation_start(display_load, ANIM_MOVE_OFF);
	keep_on=false;  //如果在draw里一直刷新变量，变量在这里关闭不了，因为会draw线程会再次覆盖
	return true;
}

extern float P_standard;
extern BME280_Data  bme280_data;
static bool btnup()
{
	P_standard = bme280_data.P;
	return false;
}

static bool btndown()
{
	return false;
}

static display_t draw()
{
	display_altitude();
	return DISPLAY_BUSY;
}

bool page_flag=0;

bool altitude_open(void)
{
	display_setDrawFunc(draw);
	buttons_setFuncs(btnup, btnExit, btndown);
	beginAnimation2(NULL);
	keep_on = true;
	page_flag = 0;
	return true;
}

extern u8 HistoryCount;


static display_t history_draw()
{
	char p[4],i,j,itemcount;
	draw_string("Time Temp Humi Alt",NOINVERT,0,0);
	draw_string("      'C    %   m ",NOINVERT,0,8);
	if(page_flag==0)
		itemcount=0;
	else
		itemcount=6;
	
	for(i=itemcount;i<itemcount+6;i++)
	{
		if(HistoryCount<i+1)
			j = HistoryCount+11-i;
		else 
			j = HistoryCount-i-1;
		sprintf((char *)p,"%d:%d", historydat[j].hour,historydat[j].min);//time
		draw_string(p,NOINVERT,0,16+(i-itemcount)*8);
		sprintf((char *)p,"%.1f", historydat[j].temp);
		draw_string(p,NOINVERT,40,16+(i-itemcount)*8);
		sprintf((char *)p,"%d", historydat[j].shidu);
		draw_string(p,NOINVERT,78,16+(i-itemcount)*8);
		sprintf((char *)p,"%d", historydat[j].height);
		draw_string(p,NOINVERT,100,16+(i-itemcount)*8);
	}
		
	return DISPLAY_BUSY;
}

static bool btnup1()
{
	page_flag=0;
	return true;
}
static bool btndown1()
{
	page_flag=1;
	return true;
}

void history_display()
{
	display_setDrawFunc(history_draw);
	buttons_setFuncs(btndown1, btnExit, btnup1);
	beginAnimation2(NULL);
	keep_on = true;
}

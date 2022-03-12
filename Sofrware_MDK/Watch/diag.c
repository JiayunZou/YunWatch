/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#define OPTION_COUNT		5

static prev_menu_s prevMenuData;

static void mSelect(void);
static void itemLoader(byte);
static void Change_BmeState(void);
static void Change_BmeTime(void);
//static void updateFPS(void);
//static void setShowFPS(void);
static void MeasureTimeUpdate(void);

extern bool bme_enable;
extern u8 bme_time;

void mDiagOpen()
{
	//rtc_tempUpdate();//读取内部温度传感器
//	battery_update();

	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_DIAGNOSTICSMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
  
	setPrevMenuOpen(&prevMenuData, mDiagOpen);
	
	beginAnimation2(NULL);
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
		appconfig_save();
	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());
	
}

static void itemLoader(byte num)
{
	switch(num)
	{
		case 1:
			Change_BmeState();
			return;
		case 2:
			Change_BmeTime();
			return;
	}

	setMenuOption_P(3, PSTR("FW    " FW_VERSION), NULL, NULL);
	setMenuOption_P(4, PSTR("User   " USER_NAME), NULL, NULL);
	addBackOption();
}

static void Change_State()
{
	bme_enable = !bme_enable;
}

static void Change_BmeState()
{

	char buff[24];
	if(bme_enable)
		//sprintf_P(buff, PSTR("EnvirMeasure  ON"));
		sprintf((char *)buff,"EnvirMeasure  ON");
	else
		sprintf((char *)buff,"EnvirMeasure  OFF");

	setMenuOption(1, buff, NULL, Change_State);
}



static void Change_BmeTime()
{
	char buff[24];
	sprintf_P((char *)buff, PSTR("MeasureTime   %ds"), bme_time);
	setMenuOption(2, buff, NULL, MeasureTimeUpdate);
}

static void MeasureTimeUpdate()
{
//	battery_updateNow();
	bme_time+=5;
	if(bme_time>50)
		bme_time=5;
}


//static void updateFPS()
//{
//	char buff[20];
//	char c =  appConfig.showFPS ? CHAR_YES : CHAR_NO;
//	sprintf_P(buff, PSTR(STR_SHOWFPS), c);
//	setMenuOption(2, buff, NULL, setShowFPS);
//}

//static void setShowFPS()
//{
//	appConfig.showFPS = !appConfig.showFPS;
//}


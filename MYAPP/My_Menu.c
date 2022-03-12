#include "common.h"
#include "delay.h"
#include "oled_driver.h"
#include "NVIC.h"
#include "bme280.h"
#include "mpu_task.h"
#include "altitude_display.h"

#define STR_WIFICMDMENU  "< My Menu >"
#define CMD1_NAME "Shut Down"
#define CMD2_NAME "Deep Sleep"
#define CMD3_NAME "MPU_Display"
#define CMD4_NAME "History_Data"
#define CMD5_NAME "Back"

#define OPTION_COUNT 5


static void mSelect()
{
	doAction(false);  //执行指令
	//menuData.isOpen = false;  //关闭菜单
}


void ShutDown(void)
{
	//display_startCRTAnim(CRTANIM_CLOSE);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

extern bool DeepSleepFlag;
void cmd2(void)
{
	DeepSleepFlag = 1;
	OLED_OFF();
	menuData.isOpen = false;  //关闭菜单
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);//进入停机模式
}

void cmd3(void)
{
	animation_start(display_load, ANIM_MOVE_OFF);
	menuData.isOpen = false;  //关闭菜单
}
extern u8 log_time;

static void LogTimeUpdate()
{
//	battery_updateNow();
	log_time+=2;
	if(log_time>15)
		log_time=1;
}

static void itemLoader(byte num)
{
	char buff[20];
	num = 0;

	setMenuOption_P(num++, PSTR(CMD1_NAME), NULL, ShutDown);

	setMenuOption_P(num++, PSTR(CMD2_NAME), NULL, cmd2);
	
	setMenuOption_P(num++, PSTR(CMD3_NAME), NULL, mpu_open);
	
	setMenuOption_P(num++, PSTR(CMD4_NAME), NULL, history_display);
	
	sprintf_P((char *)buff, PSTR("Log Time  %d min"), log_time);
	setMenuOption_P(num++, buff, NULL, LogTimeUpdate);

	setMenuOption_P(num++, PSTR(CMD5_NAME), NULL, cmd3);
}

bool my_menu_open(void)
{
	menuData.isOpen = true;  //打开菜单
	
	display_setDrawFunc(menu_draw);  //绑定绘制函数为menu_draw

	buttons_setFuncs(menu_up, menu_down ,menu_select);  //绑定按键功能函数

	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_WIFICMDMENU));   //获取当前菜单信息（选项个数，菜单类型是文字还是图标）
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //绑定菜单的函数,如前进后退选择确认
	beginAnimation2(NULL);   
	return true;
}




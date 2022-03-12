#include "common.h"
#include "NVIC.h"
#include "DS3231.h"
#include "oled_driver.h"

bool DeepSleepFlag = 0;

void KEY_INT_INIT (void){	 //按键中断初始化
	NVIC_InitTypeDef  NVIC_InitStruct;	//定义结构体变量
	EXTI_InitTypeDef  EXTI_InitStruct;

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //启动GPIO时钟 （需要与复用时钟一同启动）     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);//配置端口中断需要启用复用时钟

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);  //定义 GPIO  中断
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line5;  //定义中断线
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;              //中断使能
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;     //中断模式为 中断
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;   //下降沿触发
	
	EXTI_Init(& EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;   //中断线     
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;  //使能中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  //抢占优先级 2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;     //子优先级  2
	NVIC_Init(& NVIC_InitStruct);
}


//void DS3231_INT_INIT (void){	 //按键中断初始化
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	EXTI_InitTypeDef  EXTI_InitStruct;
//	NVIC_InitTypeDef  NVIC_InitStruct;	//定义结构体变量
//	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //启动GPIO时钟 （需要与复用时钟一同启动）  
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2
// 	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU; 		 //内部上拉输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
//	
//  	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);//配置端口中断需要启用复用时钟

//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);  //定义 GPIO  中断
//	
//	EXTI_InitStruct.EXTI_Line=EXTI_Line2;  //定义中断线
//	EXTI_InitStruct.EXTI_LineCmd=ENABLE;              //中断使能
//	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;     //中断模式为 中断
//	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;   //下降沿触发
//	
//	EXTI_Init(& EXTI_InitStruct);
//	
//	NVIC_InitStruct.NVIC_IRQChannel=EXTI2_IRQn ;   //中断线     
//	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;  //使能中断
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  //抢占优先级 2
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;     //子优先级  2
//	NVIC_Init(& NVIC_InitStruct);

//}
//外部中断9~5处理程序
//IO引脚 PB5 按键SW3
void  EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
	{
		if(KEY1==0&&DeepSleepFlag==1)
		{
			delay_ms(80);
			if(KEY1==0)
			{//判断某个线上的中断是否发生 
				RCC_Configuration();
				DeepSleepFlag=0;
				OLED_ON();
				animation_start(display_load, ANIM_MOVE_OFF);
				EXTI_ClearITPendingBit(EXTI_Line5);   //清除 LINE 上的中断标志位
			}  
		}		
		else
			EXTI_ClearITPendingBit(EXTI_Line5);   //清除 LINE 上的中断标志位
	}
			
}
//void  EXTI2_IRQHandler(void){
//	
//	 
//	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
//	{//判断某个线上的中断是否发生
//    		
//		INT_MARK=1;//标志位置1，表示有按键中断
//		 RCC_Configuration();
//		 power_off=0;
//		OLED_Display_On();
//		EXTI_ClearITPendingBit(EXTI_Line2);   //清除 LINE 上的中断标志位
//    
//	 
//	}     
//}

void RCC_Configuration(void)
	{ //RCC时钟的设置  
	ErrorStatus HSEStartUpStatus;   
	RCC_DeInit();              /* RCC system reset(for debug purpose) RCC寄存器恢复初始化值*/   
	RCC_HSEConfig(RCC_HSE_ON); /* Enable HSE 使能外部高速晶振*/   
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready 等待外部高速晶振使能完成*/   
	if(HSEStartUpStatus == SUCCESS){   
		/*设置PLL时钟源及倍频系数*/   
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //RCC_PLLMul_x（枚举2~16）是倍频值。当HSE=8MHZ,RCC_PLLMul_9时PLLCLK=72MHZ   
		/*设置AHB时钟（HCLK）*/   
		RCC_HCLKConfig(RCC_SYSCLK_Div1); //RCC_SYSCLK_Div1——AHB时钟 = 系统时钟(SYSCLK) = 72MHZ（外部晶振8HMZ）   
		/*注意此处的设置，如果使用SYSTICK做延时程序，此时SYSTICK(Cortex System timer)=HCLK/8=9MHZ*/   
		RCC_PCLK1Config(RCC_HCLK_Div2); //设置低速AHB时钟（PCLK1）,RCC_HCLK_Div2——APB1时钟 = HCLK/2 = 36MHZ（外部晶振8HMZ）   
		RCC_PCLK2Config(RCC_HCLK_Div1); //设置高速AHB时钟（PCLK2）,RCC_HCLK_Div1——APB2时钟 = HCLK = 72MHZ（外部晶振8HMZ）   
		/*注：AHB主要负责外部存储器时钟。APB2负责AD，I/O，高级TIM，串口1。APB1负责DA，USB，SPI，I2C，CAN，串口2，3，4，5，普通TIM */  
		//FLASH_SetLatency(FLASH_Latency_2); //设置FLASH存储器延时时钟周期数   
		/*FLASH时序延迟几个周期，等待总线同步操作。   
		推荐按照单片机系统运行频率：
		0—24MHz时，取Latency_0；   
		24—48MHz时，取Latency_1；   
		48~72MHz时，取Latency_2*/   
		//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //选择FLASH预取指缓存的模式，预取指缓存使能   
		RCC_PLLCmd(ENABLE);	//使能PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //等待PLL输出稳定   
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //选择SYSCLK时钟源为PLL
		while(RCC_GetSYSCLKSource() != 0x08); //等待PLL成为SYSCLK时钟源   
	}  
	/*开始使能程序中需要使用的外设时钟*/   
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1, ENABLE); //APB2外设时钟使能        	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    
} 






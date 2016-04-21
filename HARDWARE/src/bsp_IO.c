#include "bsp.h"
static uint8_t ucKeyDownTimeCnt1=0;//按键去抖动延迟计数器
static uint8_t ucKeyDownTimeCnt2=0;//按键去抖动延迟计数器
static uint8_t ucKeyUpTimeCnt1=0;//按键去抖动延迟计数器
static uint8_t ucKeyUpTimeCnt2=0;//按键去抖动延迟计数器
AppFlagStruct App_Flag; //应用程序标志位
static uint8_t TIMER0flg=0;    //定时器中断标志
static uint8_t ErrID=0;        //错误代码

void bsp_InitGPIO(void)  //按键端口初始化
{
	GPIO_SetMode(PA, BIT1, GPIO_PMD_OUTPUT);  //Configure PA.1 as Output mode
	GPIO_SetMode(PA, BIT2, GPIO_PMD_OUTPUT);  //Configure PA.2 as Output mode
	GPIO_SetMode(PA, BIT3, GPIO_PMD_OUTPUT);  //Configure PA.3 as Output mode
	GPIO_SetMode(PA, BIT4, GPIO_PMD_OUTPUT);  //Configure PA.4 as Output mode
	GPIO_SetMode(PA, BIT8, GPIO_PMD_OUTPUT);  //Configure PA.8 as Output mode
	GPIO_SetMode(PA, BIT9, GPIO_PMD_OUTPUT);  //Configure PA.9 as Output mode
	GPIO_SetMode(PA, BIT10, GPIO_PMD_OUTPUT);  //Configure PA.10 as Output mode
	GPIO_SetMode(PA, BIT11, GPIO_PMD_OUTPUT);  //Configure PA.11 as Output mode
	GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);  //Configure PA.12 as Output mode
	GPIO_SetMode(PB, BIT4, GPIO_PMD_OUTPUT);  //Configure PB.4 as Output mode
	
	GPIO_SetMode(PA, BIT13, GPIO_PMD_INPUT);  //Configure PA.13 as Input mode
	GPIO_SetMode(PA, BIT14, GPIO_PMD_INPUT);  //Configure PA.14 as Input mode
	GPIO_SetMode(PA, BIT15, GPIO_PMD_INPUT);	//Configure PA.15 as Input mode
	
	GPIO_SET_OUT_DATA(PA,0X0000);  //输出全部关闭
	PB4 = 0;
}

void InputOutput_TEST(void)  //输入输出检测
{
	uint16_t GPIOA_VALUE;	
	GPIOA_VALUE = GPIO_GET_IN_DATA(PA);
	GPIOA_VALUE &=  0xE000;
	if(GPIOA_VALUE != 0xE000){
			GPIO_TOGGLE(PA1);
			GPIO_TOGGLE(PA2);
			GPIO_TOGGLE(PA3);			
			GPIO_TOGGLE(PA4);
			GPIO_TOGGLE(PA8);
			GPIO_TOGGLE(PA9);			
			GPIO_TOGGLE(PA10);
			GPIO_TOGGLE(PA11);
			GPIO_TOGGLE(PA12);			
			GPIO_TOGGLE(PB4);		
		  CLK_SysTickDelay(10000000);
	}
}

/*******************************************************************************
* 函数名	: void Input_scan(void) 
* 功能描述	: 输入扫描函数 
* 输入参数  :无
* 返回参数  : 错误代码   车辆超长或人车误入
********************************************************************************/
uint8_t Input_scan(void) //输入扫描函数
{
	static	uint8_t ReturnValue = 0;	
	//////////前超长输入信号检测////////////////////////////////////////////////////
   if(FRONT_IN == 1)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
   {
	  ucKeyDownTimeCnt1=0;//按键去抖动延时计数器清零  
	  ucKeyUpTimeCnt1 ++;
	  if(ucKeyUpTimeCnt1>const_key_time1)
	  {
		ucKeyUpTimeCnt1 = 0;
		ReturnValue &=~ERR_OVERLENGTH_FRONT;//清除对应的错误信息
	  }
		
   }
   else 
   {
   	  ucKeyUpTimeCnt1 = 0;
      ucKeyDownTimeCnt1++; //累加定时中断次数
      if(ucKeyDownTimeCnt1>const_key_time1)
      {
         ucKeyDownTimeCnt1=0; 
		 ReturnValue |= ERR_OVERLENGTH_FRONT;//前超长输入信号检测
      }
   }
	//////////后超长输入信号检测////////////////////////////////////////////////////
   if(REAR_IN == 1)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
   {
	  ucKeyDownTimeCnt2=0;//按键去抖动延时计数器清零  
	  ucKeyUpTimeCnt2 ++;
	  if(ucKeyUpTimeCnt2>const_key_time2)
	  {
		ucKeyUpTimeCnt2 = 0;
		ReturnValue &=~ERR_OVERLENGTH_REAR;//清除对应的错误信息
	  }
		
   }
   else 
   {
   	  ucKeyUpTimeCnt2 = 0;
      ucKeyDownTimeCnt2++; //累加定时中断次数
      if(ucKeyDownTimeCnt2>const_key_time2)
      {
        ucKeyDownTimeCnt2=0; 
		ReturnValue |= ERR_OVERLENGTH_REAR;//后超长输入信号检测
      }
   } 
	return ReturnValue;
}

/*******************************************************************************
* 函数名	: void ControlAction(uint8_t command) 
* 功能描述	: 控制动作 
* 输入参数 1  : 控制指令
* 返回参数     : 接收到的指令是关闭指令还是打开指令
********************************************************************************/
uint8_t ControlAction(uint8_t command) 
{
	static	uint8_t ReturnValue;
	if(command & EXE_FORWARD_ON)//正向接触器打开
	{
		RUNLED_ON;     //运行指示灯打开
		INVERSION_OFF; //反向接触器关闭
		FORWARD_ON;		//正向接触器打开
		ReturnValue = COMMAND_RELAY_ON;
	}
	else if(command & EXE_FORWARD_OFF)//正向接触器关闭 
	{
		RUNLED_OFF;     //运行指示灯关闭
		INVERSION_OFF;  //反向接触器关闭
		FORWARD_OFF;	//正向接触器关闭
		ReturnValue = COMMAND_RELAY_OFF;
	}
	else if(command & EXE_INVERSION_ON)//反向接触器打开
	{
		RUNLED_ON;     //运行指示灯打开
		INVERSION_ON; //反向接触器打开
		FORWARD_OFF;	//正向接触器关闭
		ReturnValue = COMMAND_RELAY_ON;
	}
	else if(command & EXE_INVERSION_OFF)//反向接触器关闭 
	{
		RUNLED_OFF;     //运行指示灯关闭
		INVERSION_OFF;  //反向接触器关闭
		FORWARD_OFF;	//正向接触器关闭
		ReturnValue = COMMAND_RELAY_OFF;
	}	
	else if(command & EXE_ALARM_ON)//报警器打开
	{
		ALARM_ON;       //报警器打开
		RUNLED_OFF;     //运行指示灯关闭
		INVERSION_OFF;  //反向接触器关闭
		FORWARD_OFF;	//正向接触器关闭
		ReturnValue = COMMAND_RELAY_OFF;
		App_Flag.Alarm_PLC = 1;//接收到来自PLC的报警器打开指令
	}
	else if(command & EXE_ALARM_OFF)//报警器关闭 
	{
		ALARM_OFF;		//报警器关闭 
		ReturnValue = COMMAND_RELAY_OFF;
		App_Flag.Alarm_PLC = 0;//接收到来自PLC的报警器关闭指令
	}
	return ReturnValue;
}

/*******************************************************************************
* 函数名	: void Output(void) 
* 功能描述:  按逻辑输出
* 输入参数:无
* 返回参数: 无
********************************************************************************/
void Output(void) 
{
	static STR_RMSG_BSP StrExecuteMsg;//需要执行的信息
	static STR_CANMSG_BSP  StrMsgToSend;//需要发送的信息
	static STR_CANMSG_BSP  StrMsgToSendTmp;//需要发送的信息
	static STR_CANMSG_BSP  StrMsgReceived;//接收到的信息处理
	static uint8_t ErrIDtmp=0;        //错误代码暂存值
	static uint8_t ErrAlarmFlag=0;        //检测到错误 打开报警器标志
	static uint8_t AlarmTimeCnt=0;       //检测到错误 打开报警器标志 
	static uint8_t SendIntervalTimeCnt=0;       //检测到错误 打开报警器标志
	
	if (CBReadOne(&CANCircularBuf_RX,&StrMsgReceived) == eErrorBufferOK)//成功读取出数据
	{
		StrExecuteMsg = CANExecuteMsg(&StrMsgReceived);//需要执行的信息	
	}
	
	if (StrExecuteMsg.motion == 1)//动作执行指令  不为0  手动模式或自动模式
	{
		if (ControlAction(StrExecuteMsg.exeID) == COMMAND_RELAY_OFF) //按指令执行动作  判断是不是继电器关闭指令
		{
			StrExecuteMsg.motion = 0;//清除动作执行指令
		}
	}
	else  //动作执行指令为0  关闭所有输出
	{
		RUNLED_OFF;     //运行指示灯关闭
		INVERSION_OFF;  //反向接触器关闭
		FORWARD_OFF;	//正向接触器关闭		
	}		
	
	if (ErrID != 0)//检测到超长错误
	{	
		App_Flag.ErrScanlock = 1;//加锁，防止冲突，定时器中断函数改写ErrID的值，防止在读取处理过程中，ErrID的值改变
		if (ErrID != 0)//再次判断 否则ErrID的值还是会改变
		{
			AlarmTimeCnt = 0;//清定时器计数器
			ErrAlarmFlag = 1;//置检测到错误 打开报警器标志
			if (ErrID != ErrIDtmp)//有新的错误产生  发送一次
			{
				ErrIDtmp = ErrID;//保存当前错误代码
				StrMsgToSend.FuncID = FUNC_ERR; //反馈给PLC错误帧  错误功能码
				StrMsgToSend.ack = ACK;//不发送应答帧
				if (ErrID == 0x03)//前后超长同时存在，只发一个错误代码
				{
					StrMsgToSend.byte = ERR_OVERLENGTH_FRONT;//只发送前超长错误代码
				}
				else  //前超长存在就发送前超长   后超长存在就发送后超长
				{
					StrMsgToSend.byte = ErrID;//发送错误码	
				}							
				CBWriteOne(&CANCircularBuf_TX,StrMsgToSend);//向发送缓存填写数据	
				#ifdef BSP_DEBUG_MODE
				  printf("ErrID = 0x%x\r\n", ErrID);
				#endif 			
			}
		}
		App_Flag.ErrScanlock = 0;//解锁
	}
	else 
	{
		ErrIDtmp = 0;//清除错误码暂存值
	}
	
	if (StrExecuteMsg.ack == 1)////接收到的数据帧需要应答  将接收到的数据信息原样返回
	{
		StrExecuteMsg.ack = 0;//清除应答标识位
		StrMsgToSend.FuncID = StrMsgReceived.FuncID; //反馈给PLC接收到的功能码
		StrMsgToSend.ack = 0;//发送应答帧
		StrMsgToSend.byte = StrMsgReceived.byte;//返回接收的参数			
		CBWriteOne(&CANCircularBuf_TX,StrMsgToSend);//向发送缓存填写数据
		
	}

	if (App_Flag.connected == 1)//建立连接之后才能向CAN总线发送数据
	{
		if (App_Flag.SendInterval == 0)//发送的任意两帧数据间隔至少10ms
		{			
			if (CBReadOne(&CANCircularBuf_TX,&StrMsgToSendTmp) == eErrorBufferOK)//成功读取出数据
			{	
				FrameToSend(&StrMsgToSendTmp);//发送信息
				App_Flag.SendInterval = 1;//两帧数据间隔标志
				SendIntervalTimeCnt = 0;
			}				
		}
	}
	
	if (TIMER0flg == 1)//定时时间到
	{
		TIMER0flg = 0;//清定时器标志
		WDT_RESET_COUNTER ( );//Reset WDT Counter
		if (ErrAlarmFlag == 1)//置检测到错误 打开报警器标志
		{
			AlarmTimeCnt ++;//报警器报警延时计数器累加
			ALARM_ON;//报警器打开
			if (AlarmTimeCnt > ALARM_DELAY_TIME)
			{
				ErrAlarmFlag = 0;//清检测到错误 打开报警器标志
				if (App_Flag.Alarm_PLC == 0)//接收到来自PLC的报警器关闭指令
					ALARM_OFF;//报警器关闭
			}	
		}
		if (App_Flag.SendInterval == 1)//发送的任意两帧数据间隔至少10ms
		{
			SendIntervalTimeCnt ++;
			if (SendIntervalTimeCnt > SEND_INTERVAL_TIME)
			{
				App_Flag.SendInterval = 0;
			}
		}
		if (StrMsgReceived.FuncID == FUNC_HAND)//手动模式	500ms没有接收到数据就停止
		{
			HandleTimeCnt ++;// //手动时间计数器
			if (HandleTimeCnt > HANDLE_TIME_REACH)
			{
				StrExecuteMsg.motion = 0;//清动作执行标志
				#ifdef BSP_DEBUG_MODE
					//	printf("//////////////////////\r\n");
				#endif  
			}
			#ifdef BSP_DEBUG_MODE
				//	printf("HandleTimeCnt =%d\r\n",HandleTimeCnt);
			#endif  
		}		
	}
}

/*******************************************************************************
* 函数名  : void TMR0_IRQHandler(void)
* 功能描述: 定时器中断函数 ，输入扫描，每5ms进入一次中断
* 输入参数:无
* 返回参数: 无
********************************************************************************/
void TMR0_IRQHandler(void)
{
	if (TIMER_GetIntFlag(TIMER0) == 1)
	{
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
		TIMER0flg = 1;//置定时器标志
		if(App_Flag.ErrScanlock == 0)//其他线程已经结束对ErrID的访问
		{
			ErrID = Input_scan();//读取错误代码 
		}			 
    }
}

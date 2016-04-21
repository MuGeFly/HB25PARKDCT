#include "bsp.h"
static uint8_t ucKeyDownTimeCnt1=0;//����ȥ�����ӳټ�����
static uint8_t ucKeyDownTimeCnt2=0;//����ȥ�����ӳټ�����
static uint8_t ucKeyUpTimeCnt1=0;//����ȥ�����ӳټ�����
static uint8_t ucKeyUpTimeCnt2=0;//����ȥ�����ӳټ�����
AppFlagStruct App_Flag; //Ӧ�ó����־λ
static uint8_t TIMER0flg=0;    //��ʱ���жϱ�־
static uint8_t ErrID=0;        //�������

void bsp_InitGPIO(void)  //�����˿ڳ�ʼ��
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
	
	GPIO_SET_OUT_DATA(PA,0X0000);  //���ȫ���ر�
	PB4 = 0;
}

void InputOutput_TEST(void)  //����������
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
* ������	: void Input_scan(void) 
* ��������	: ����ɨ�躯�� 
* �������  :��
* ���ز���  : �������   �����������˳�����
********************************************************************************/
uint8_t Input_scan(void) //����ɨ�躯��
{
	static	uint8_t ReturnValue = 0;	
	//////////ǰ���������źż��////////////////////////////////////////////////////
   if(FRONT_IN == 1)//IO�Ǹߵ�ƽ��˵������û�б����£���ʱҪ��ʱ����һЩ��־λ
   {
	  ucKeyDownTimeCnt1=0;//����ȥ������ʱ����������  
	  ucKeyUpTimeCnt1 ++;
	  if(ucKeyUpTimeCnt1>const_key_time1)
	  {
		ucKeyUpTimeCnt1 = 0;
		ReturnValue &=~ERR_OVERLENGTH_FRONT;//�����Ӧ�Ĵ�����Ϣ
	  }
		
   }
   else 
   {
   	  ucKeyUpTimeCnt1 = 0;
      ucKeyDownTimeCnt1++; //�ۼӶ�ʱ�жϴ���
      if(ucKeyDownTimeCnt1>const_key_time1)
      {
         ucKeyDownTimeCnt1=0; 
		 ReturnValue |= ERR_OVERLENGTH_FRONT;//ǰ���������źż��
      }
   }
	//////////�󳬳������źż��////////////////////////////////////////////////////
   if(REAR_IN == 1)//IO�Ǹߵ�ƽ��˵������û�б����£���ʱҪ��ʱ����һЩ��־λ
   {
	  ucKeyDownTimeCnt2=0;//����ȥ������ʱ����������  
	  ucKeyUpTimeCnt2 ++;
	  if(ucKeyUpTimeCnt2>const_key_time2)
	  {
		ucKeyUpTimeCnt2 = 0;
		ReturnValue &=~ERR_OVERLENGTH_REAR;//�����Ӧ�Ĵ�����Ϣ
	  }
		
   }
   else 
   {
   	  ucKeyUpTimeCnt2 = 0;
      ucKeyDownTimeCnt2++; //�ۼӶ�ʱ�жϴ���
      if(ucKeyDownTimeCnt2>const_key_time2)
      {
        ucKeyDownTimeCnt2=0; 
		ReturnValue |= ERR_OVERLENGTH_REAR;//�󳬳������źż��
      }
   } 
	return ReturnValue;
}

/*******************************************************************************
* ������	: void ControlAction(uint8_t command) 
* ��������	: ���ƶ��� 
* ������� 1  : ����ָ��
* ���ز���     : ���յ���ָ���ǹر�ָ��Ǵ�ָ��
********************************************************************************/
uint8_t ControlAction(uint8_t command) 
{
	static	uint8_t ReturnValue;
	if(command & EXE_FORWARD_ON)//����Ӵ�����
	{
		RUNLED_ON;     //����ָʾ�ƴ�
		INVERSION_OFF; //����Ӵ����ر�
		FORWARD_ON;		//����Ӵ�����
		ReturnValue = COMMAND_RELAY_ON;
	}
	else if(command & EXE_FORWARD_OFF)//����Ӵ����ر� 
	{
		RUNLED_OFF;     //����ָʾ�ƹر�
		INVERSION_OFF;  //����Ӵ����ر�
		FORWARD_OFF;	//����Ӵ����ر�
		ReturnValue = COMMAND_RELAY_OFF;
	}
	else if(command & EXE_INVERSION_ON)//����Ӵ�����
	{
		RUNLED_ON;     //����ָʾ�ƴ�
		INVERSION_ON; //����Ӵ�����
		FORWARD_OFF;	//����Ӵ����ر�
		ReturnValue = COMMAND_RELAY_ON;
	}
	else if(command & EXE_INVERSION_OFF)//����Ӵ����ر� 
	{
		RUNLED_OFF;     //����ָʾ�ƹر�
		INVERSION_OFF;  //����Ӵ����ر�
		FORWARD_OFF;	//����Ӵ����ر�
		ReturnValue = COMMAND_RELAY_OFF;
	}	
	else if(command & EXE_ALARM_ON)//��������
	{
		ALARM_ON;       //��������
		RUNLED_OFF;     //����ָʾ�ƹر�
		INVERSION_OFF;  //����Ӵ����ر�
		FORWARD_OFF;	//����Ӵ����ر�
		ReturnValue = COMMAND_RELAY_OFF;
		App_Flag.Alarm_PLC = 1;//���յ�����PLC�ı�������ָ��
	}
	else if(command & EXE_ALARM_OFF)//�������ر� 
	{
		ALARM_OFF;		//�������ر� 
		ReturnValue = COMMAND_RELAY_OFF;
		App_Flag.Alarm_PLC = 0;//���յ�����PLC�ı������ر�ָ��
	}
	return ReturnValue;
}

/*******************************************************************************
* ������	: void Output(void) 
* ��������:  ���߼����
* �������:��
* ���ز���: ��
********************************************************************************/
void Output(void) 
{
	static STR_RMSG_BSP StrExecuteMsg;//��Ҫִ�е���Ϣ
	static STR_CANMSG_BSP  StrMsgToSend;//��Ҫ���͵���Ϣ
	static STR_CANMSG_BSP  StrMsgToSendTmp;//��Ҫ���͵���Ϣ
	static STR_CANMSG_BSP  StrMsgReceived;//���յ�����Ϣ����
	static uint8_t ErrIDtmp=0;        //��������ݴ�ֵ
	static uint8_t ErrAlarmFlag=0;        //��⵽���� �򿪱�������־
	static uint8_t AlarmTimeCnt=0;       //��⵽���� �򿪱�������־ 
	static uint8_t SendIntervalTimeCnt=0;       //��⵽���� �򿪱�������־
	
	if (CBReadOne(&CANCircularBuf_RX,&StrMsgReceived) == eErrorBufferOK)//�ɹ���ȡ������
	{
		StrExecuteMsg = CANExecuteMsg(&StrMsgReceived);//��Ҫִ�е���Ϣ	
	}
	
	if (StrExecuteMsg.motion == 1)//����ִ��ָ��  ��Ϊ0  �ֶ�ģʽ���Զ�ģʽ
	{
		if (ControlAction(StrExecuteMsg.exeID) == COMMAND_RELAY_OFF) //��ָ��ִ�ж���  �ж��ǲ��Ǽ̵����ر�ָ��
		{
			StrExecuteMsg.motion = 0;//�������ִ��ָ��
		}
	}
	else  //����ִ��ָ��Ϊ0  �ر��������
	{
		RUNLED_OFF;     //����ָʾ�ƹر�
		INVERSION_OFF;  //����Ӵ����ر�
		FORWARD_OFF;	//����Ӵ����ر�		
	}		
	
	if (ErrID != 0)//��⵽��������
	{	
		App_Flag.ErrScanlock = 1;//��������ֹ��ͻ����ʱ���жϺ�����дErrID��ֵ����ֹ�ڶ�ȡ��������У�ErrID��ֵ�ı�
		if (ErrID != 0)//�ٴ��ж� ����ErrID��ֵ���ǻ�ı�
		{
			AlarmTimeCnt = 0;//�嶨ʱ��������
			ErrAlarmFlag = 1;//�ü�⵽���� �򿪱�������־
			if (ErrID != ErrIDtmp)//���µĴ������  ����һ��
			{
				ErrIDtmp = ErrID;//���浱ǰ�������
				StrMsgToSend.FuncID = FUNC_ERR; //������PLC����֡  ��������
				StrMsgToSend.ack = ACK;//������Ӧ��֡
				if (ErrID == 0x03)//ǰ�󳬳�ͬʱ���ڣ�ֻ��һ���������
				{
					StrMsgToSend.byte = ERR_OVERLENGTH_FRONT;//ֻ����ǰ�����������
				}
				else  //ǰ�������ھͷ���ǰ����   �󳬳����ھͷ��ͺ󳬳�
				{
					StrMsgToSend.byte = ErrID;//���ʹ�����	
				}							
				CBWriteOne(&CANCircularBuf_TX,StrMsgToSend);//���ͻ�����д����	
				#ifdef BSP_DEBUG_MODE
				  printf("ErrID = 0x%x\r\n", ErrID);
				#endif 			
			}
		}
		App_Flag.ErrScanlock = 0;//����
	}
	else 
	{
		ErrIDtmp = 0;//����������ݴ�ֵ
	}
	
	if (StrExecuteMsg.ack == 1)////���յ�������֡��ҪӦ��  �����յ���������Ϣԭ������
	{
		StrExecuteMsg.ack = 0;//���Ӧ���ʶλ
		StrMsgToSend.FuncID = StrMsgReceived.FuncID; //������PLC���յ��Ĺ�����
		StrMsgToSend.ack = 0;//����Ӧ��֡
		StrMsgToSend.byte = StrMsgReceived.byte;//���ؽ��յĲ���			
		CBWriteOne(&CANCircularBuf_TX,StrMsgToSend);//���ͻ�����д����
		
	}

	if (App_Flag.connected == 1)//��������֮�������CAN���߷�������
	{
		if (App_Flag.SendInterval == 0)//���͵�������֡���ݼ������10ms
		{			
			if (CBReadOne(&CANCircularBuf_TX,&StrMsgToSendTmp) == eErrorBufferOK)//�ɹ���ȡ������
			{	
				FrameToSend(&StrMsgToSendTmp);//������Ϣ
				App_Flag.SendInterval = 1;//��֡���ݼ����־
				SendIntervalTimeCnt = 0;
			}				
		}
	}
	
	if (TIMER0flg == 1)//��ʱʱ�䵽
	{
		TIMER0flg = 0;//�嶨ʱ����־
		WDT_RESET_COUNTER ( );//Reset WDT Counter
		if (ErrAlarmFlag == 1)//�ü�⵽���� �򿪱�������־
		{
			AlarmTimeCnt ++;//������������ʱ�������ۼ�
			ALARM_ON;//��������
			if (AlarmTimeCnt > ALARM_DELAY_TIME)
			{
				ErrAlarmFlag = 0;//���⵽���� �򿪱�������־
				if (App_Flag.Alarm_PLC == 0)//���յ�����PLC�ı������ر�ָ��
					ALARM_OFF;//�������ر�
			}	
		}
		if (App_Flag.SendInterval == 1)//���͵�������֡���ݼ������10ms
		{
			SendIntervalTimeCnt ++;
			if (SendIntervalTimeCnt > SEND_INTERVAL_TIME)
			{
				App_Flag.SendInterval = 0;
			}
		}
		if (StrMsgReceived.FuncID == FUNC_HAND)//�ֶ�ģʽ	500msû�н��յ����ݾ�ֹͣ
		{
			HandleTimeCnt ++;// //�ֶ�ʱ�������
			if (HandleTimeCnt > HANDLE_TIME_REACH)
			{
				StrExecuteMsg.motion = 0;//�嶯��ִ�б�־
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
* ������  : void TMR0_IRQHandler(void)
* ��������: ��ʱ���жϺ��� ������ɨ�裬ÿ5ms����һ���ж�
* �������:��
* ���ز���: ��
********************************************************************************/
void TMR0_IRQHandler(void)
{
	if (TIMER_GetIntFlag(TIMER0) == 1)
	{
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
		TIMER0flg = 1;//�ö�ʱ����־
		if(App_Flag.ErrScanlock == 0)//�����߳��Ѿ�������ErrID�ķ���
		{
			ErrID = Input_scan();//��ȡ������� 
		}			 
    }
}

#include "bsp.h"

/*******************************************************************************
* ������	: bsp_DelayMS
* ��������	:
* ������� 1  : ������ʱ����
* ������� 2  : ��
* ������� 3  : ��
* ���ز���     : ��
********************************************************************************
*�޸ļ�¼:
*�޸�����             �޸���          �޸ı�ע
*2015/10/16             Whatbook               ��ʼ�汾
*******************************************************************************/
void  bsp_DelayMS(u32 delayTime)
{
	//vTaskDelay(delayTime);
	u16 i=0;
	while(delayTime--)
	{
		//i=12000;  //�Լ�����
		i=3000;  //�Լ�����
		while(i--) ;
	}
}

/*******************************************************************************
* ������	: bsp_DelayUs
* ��������	:
* ������� 1  : ��ʱ΢��
* ������� 2  : ��
* ������� 3  : ��
* ���ز���     : ��
********************************************************************************
*�޸ļ�¼:
*�޸�����             �޸���          �޸ı�ע
*2015/10/16             Whatbook               ��ʼ�汾
*******************************************************************************/
void  bsp_DelayUs(u32 delayTime)
{
	//vTaskDelay(delayTime);
	u16 i=0;
	while(delayTime--)
	{
		//i=10;  //�Լ�����
		i=3;  //�Լ�����
		while(i--) ;
	}
}

//////////////////////////////////////////////////////////////////////////
/*
*********************************************************************************************************
*	�� �� ��: CircularBufferInit
*	����˵��: ��ʼ�����������ݽṹ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void CircularBufferInit(sCircularBuffer *cb,tElement *buf,u8 buflen)
 {
 	cb->DataBuf = buf;
 	cb->ReadIndex = 0;
 	cb->WriteIndex = 0;
 	cb->BufLen = buflen;
 }

 /*
  *********************************************************************************************************
  *	�� �� ��: CBFreeDataLenght
  *	����˵��: ���㻺�����������ݳ��ȣ�
  *	��    �Σ����������ݽṹָ��
  *	�� �� ֵ: �ջ���������==0������������ֵ==(cb->BufLen - 1)
  *********************************************************************************************************
  */
 u8 CBFreeDataLenght(sCircularBuffer *cb)
 {
 	return ((cb->WriteIndex - cb->ReadIndex) & (cb->BufLen - 1));
 }

 /*
  *********************************************************************************************************
  *	�� �� ��: CBWriteOne
  *	����˵��: дһ�����ݵ�������
  *	��    �Σ����������ݽṹָ��   ====  Ҫд������
  *	�� �� ֵ: �ɹ�==eErrorBufferOK   ʧ��==eErrorBufferFull
  *********************************************************************************************************
  */
 u8 CBWriteOne(sCircularBuffer *cb,tElement writedata)
 {
 	if(CBFreeDataLenght(cb) == ((cb->BufLen - 1)))
 	{
 		return eErrorBufferFull;
 	}
 	cb->DataBuf[cb->WriteIndex] = writedata;
 	cb->WriteIndex = (cb->WriteIndex+1) & (cb->BufLen - 1);//������ԭ�Ӳ���
 	return eErrorBufferOK;
 }

 /*
  *********************************************************************************************************
  *	�� �� ��: CBReadOne
  *	����˵��:��һ����������ȡһ������
  *	��    �Σ����������ݽṹָ��   ====  ��ȡ���ݴ�ŵ�ָ��
  *	�� �� ֵ: �ɹ�==eErrorBufferOK   ʧ��==eErrorBufferEmpty
  *********************************************************************************************************
  */
 u8 CBReadOne(sCircularBuffer *cb,tElement *Readdata)
 {
 	if(CBFreeDataLenght(cb) == 0)
 	{
 		return eErrorBufferEmpty;
 	}
 	*Readdata = cb->DataBuf[cb->ReadIndex];
 	cb->ReadIndex = (cb->ReadIndex + 1) & (cb->BufLen - 1);//������ԭ�Ӳ���
 	return eErrorBufferOK;
 }
 
/*
  *********************************************************************************************************
  *	�� �� ��: CBFreeData
  *	����˵��: ǿ�ƻ�������дһ��
  *	��    �Σ����������ݽṹָ��
  *	�� �� ֵ:
  *********************************************************************************************************
  */
 void CBFreeData(sCircularBuffer *cb)
 {
 	cb->ReadIndex = cb->WriteIndex;
 }
 
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
   // CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
  //  CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);
  
    /* Enable external XTAL 12MHz clock and internal 10 kHz */
    CLK_EnableXtalRC((CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC10K_EN_Msk));

    /* Waiting for clock ready */
    CLK_WaitClockReady((CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_OSC10K_STB_Msk));	

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
		
	 /* Enable CAN module clock */
    CLK_EnableModuleClock(CAN0_MODULE);
		
		/* Enable Timer 0  module clock */
    CLK_EnableModuleClock(TMR0_MODULE); 
	
    /* Enable WDT module clock */
    CLK_EnableModuleClock(WDT_MODULE);    
        
    /* Select WDT module clock source */
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDT_S_LIRC, NULL);	
		
    /* Select Timer 0 module clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, NULL);		

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);
		
	  /* Set PD multi-function pins for CANTX0, CANRX0 */
    SYS->GPD_MFP &= ~(SYS_GPD_MFP_PD6_Msk | SYS_GPD_MFP_PD7_Msk);
    SYS->GPD_MFP = SYS_GPD_MFP_PD6_CAN0_RXD | SYS_GPD_MFP_PD7_CAN0_TXD;

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void TIMER0_Init()
{
	/* Open Timer0 in periodic mode, enable interrupt and 200 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 200);
    TIMER_EnableInt(TIMER0);
	 /* Enable Timer0 NVIC */
    NVIC_EnableIRQ(TMR0_IRQn);
	 /* Start Timer0  counting */	
}

void WDT_Init()
{
	WDT_RESET_COUNTER (  );//Reset WDT Counter
    /* Enable WDT time-out reset function and select time-out interval to 2^14 * WDT clock then start WDT counting */	
	WDT_Open(WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_1026CLK, TRUE, FALSE);
}
/*******************************************************************************
* ������	: bsp_Init
* ��������	:
* ������� 1  : ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ��
* ������� 2  : ��
* ������� 3  : ��
* ���ز���     : ��
********************************************************************************
*�޸ļ�¼:
*�޸�����             �޸���          �޸ı�ע
*2015/10/16              Whatbook               ��ʼ�汾
*******************************************************************************/
void bsp_Init(void)
{
   /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
	
	bsp_InitGPIO();  //��������˿ڳ�ʼ��

    UART0_Init();
	
	bsp_CANInit();
	
	TIMER0_Init();
	
    /* Because of all bits can be written in WDT Control Register are write-protected;
       To program it needs to disable register protection first. */
    SYS_UnlockReg();
	
	WDT_Init();
	
    TIMER_Start(TIMER0);
    printf("Simple Demo Code\r\n");
}

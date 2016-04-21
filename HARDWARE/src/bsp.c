#include "bsp.h"

/*******************************************************************************
* 函数名	: bsp_DelayMS
* 功能描述	:
* 输入参数 1  : 定义延时毫秒
* 输入参数 2  : 无
* 输入参数 3  : 无
* 返回参数     : 无
********************************************************************************
*修改记录:
*修改日期             修改人          修改备注
*2015/10/16             Whatbook               初始版本
*******************************************************************************/
void  bsp_DelayMS(u32 delayTime)
{
	//vTaskDelay(delayTime);
	u16 i=0;
	while(delayTime--)
	{
		//i=12000;  //自己定义
		i=3000;  //自己定义
		while(i--) ;
	}
}

/*******************************************************************************
* 函数名	: bsp_DelayUs
* 功能描述	:
* 输入参数 1  : 延时微秒
* 输入参数 2  : 无
* 输入参数 3  : 无
* 返回参数     : 无
********************************************************************************
*修改记录:
*修改日期             修改人          修改备注
*2015/10/16             Whatbook               初始版本
*******************************************************************************/
void  bsp_DelayUs(u32 delayTime)
{
	//vTaskDelay(delayTime);
	u16 i=0;
	while(delayTime--)
	{
		//i=10;  //自己定义
		i=3;  //自己定义
		while(i--) ;
	}
}

//////////////////////////////////////////////////////////////////////////
/*
*********************************************************************************************************
*	函 数 名: CircularBufferInit
*	功能说明: 初始化缓存区数据结构
*	形    参：无
*	返 回 值: 无
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
  *	函 数 名: CBFreeDataLenght
  *	功能说明: 计算缓冲区可用数据长度，
  *	形    参：缓冲区数据结构指针
  *	返 回 值: 空缓冲区返回==0；数据满返回值==(cb->BufLen - 1)
  *********************************************************************************************************
  */
 u8 CBFreeDataLenght(sCircularBuffer *cb)
 {
 	return ((cb->WriteIndex - cb->ReadIndex) & (cb->BufLen - 1));
 }

 /*
  *********************************************************************************************************
  *	函 数 名: CBWriteOne
  *	功能说明: 写一个数据到缓冲区
  *	形    参：缓冲区数据结构指针   ====  要写的数据
  *	返 回 值: 成功==eErrorBufferOK   失败==eErrorBufferFull
  *********************************************************************************************************
  */
 u8 CBWriteOne(sCircularBuffer *cb,tElement writedata)
 {
 	if(CBFreeDataLenght(cb) == ((cb->BufLen - 1)))
 	{
 		return eErrorBufferFull;
 	}
 	cb->DataBuf[cb->WriteIndex] = writedata;
 	cb->WriteIndex = (cb->WriteIndex+1) & (cb->BufLen - 1);//必须是原子操作
 	return eErrorBufferOK;
 }

 /*
  *********************************************************************************************************
  *	函 数 名: CBReadOne
  *	功能说明:从一个缓冲区读取一个数据
  *	形    参：缓冲区数据结构指针   ====  读取数据存放的指针
  *	返 回 值: 成功==eErrorBufferOK   失败==eErrorBufferEmpty
  *********************************************************************************************************
  */
 u8 CBReadOne(sCircularBuffer *cb,tElement *Readdata)
 {
 	if(CBFreeDataLenght(cb) == 0)
 	{
 		return eErrorBufferEmpty;
 	}
 	*Readdata = cb->DataBuf[cb->ReadIndex];
 	cb->ReadIndex = (cb->ReadIndex + 1) & (cb->BufLen - 1);//必须是原子操作
 	return eErrorBufferOK;
 }
 
/*
  *********************************************************************************************************
  *	函 数 名: CBFreeData
  *	功能说明: 强制缓冲区读写一致
  *	形    参：缓冲区数据结构指针
  *	返 回 值:
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
* 函数名	: bsp_Init
* 功能描述	:
* 输入参数 1  : 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
* 输入参数 2  : 无
* 输入参数 3  : 无
* 返回参数     : 无
********************************************************************************
*修改记录:
*修改日期             修改人          修改备注
*2015/10/16              Whatbook               初始版本
*******************************************************************************/
void bsp_Init(void)
{
   /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
	
	bsp_InitGPIO();  //输入输出端口初始化

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

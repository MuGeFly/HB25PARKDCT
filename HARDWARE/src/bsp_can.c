#include "bsp.h"

/* Declare a CAN message structure */
STR_CANMSG_T rrMsg;

STR_CANMSG_BSP CAN_RxBuf[CAN_RX_BUFFER_SIZE];//接收缓存数组
STR_CANMSG_BSP CAN_TxBuf[CAN_TX_BUFFER_SIZE];//发送缓存数组
sCircularBuffer CANCircularBuf_RX;//接收环形缓冲区数据结构
sCircularBuffer CANCircularBuf_TX;//发送环形缓冲区数据结构
uint8_t HandleTimeCnt = 0;// //手动时间计数器
static STR_CANMSG_BSP CANReceivedProtocol(STR_CANMSG_T* Msg); //协议解析

/*---------------------------------------------------------------------------*/
/*  Show Message Function                                                    */
/*---------------------------------------------------------------------------*/
void CAN_ShowMsg(STR_CANMSG_T* Msg)
{
    uint8_t i;

    /* Show the message information */
    printf("Read ID=0x%X, Type=%s, DLC=%d, Data=", Msg->Id, Msg->IdType ? "EXT" : "STD", Msg->DLC);
    for(i = 0; i < Msg->DLC; i++)
        printf("%X,", Msg->Data[i]);
    printf("\r\n");
}


/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle CAN interrupt event                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_MsgInterrupt(CAN_T *tCAN, uint32_t u32IIDR)
{
	STR_CANMSG_BSP StrMsgToWrite;
    if(u32IIDR == 1) {        
        CAN_Receive(tCAN, 0, &rrMsg);
		StrMsgToWrite = CANReceivedProtocol(&rrMsg);//读取解析后的数据
		CBWriteOne(&CANCircularBuf_RX,StrMsgToWrite);//向接收缓存数组填写数据
		#ifdef BSP_DEBUG_MODE
			printf("Msg-0 INT and Callback\r\n");
			CAN_ShowMsg(&rrMsg);
		#endif        
    }
    if(u32IIDR == 1 + 1) {       
        CAN_Receive(tCAN, 1, &rrMsg);
		StrMsgToWrite = CANReceivedProtocol(&rrMsg);//读取解析后的数据
		CBWriteOne(&CANCircularBuf_RX,StrMsgToWrite);//向接收缓存数组填写数据		
		#ifdef BSP_DEBUG_MODE
			printf("Msg-1 INT and Callback\r\n");
			CAN_ShowMsg(&rrMsg);
		#endif         
    }	
    if(u32IIDR == 2 + 1) {       
        CAN_Receive(tCAN, 2, &rrMsg);
		StrMsgToWrite = CANReceivedProtocol(&rrMsg);//读取解析后的数据
		CBWriteOne(&CANCircularBuf_RX,StrMsgToWrite);//向接收缓存数组填写数据		
		#ifdef BSP_DEBUG_MODE
			printf("Msg-2 INT and Callback\r\n");
			CAN_ShowMsg(&rrMsg);
		#endif         
    }		
//    if(u32IIDR == 5 + 1) {
//        printf("Msg-5 INT and Callback \n");
//        CAN_Receive(tCAN, 5, &rrMsg);
//        CAN_ShowMsg(&rrMsg);
//    }
//    if(u32IIDR == 31 + 1) {
//        printf("Msg-31 INT and Callback \n");
//        CAN_Receive(tCAN, 31, &rrMsg);
//        CAN_ShowMsg(&rrMsg);
//    }
}
/*---------------------------------------------------------------------------------------------------------*/
/* CAN0 interrupt handler                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CAN0_IRQHandler(void)
{
    uint32_t u8IIDRstatus;

    u8IIDRstatus = CAN0->IIDR;

    if(u8IIDRstatus == 0x00008000) {      /* Check Status Interrupt Flag (Error status Int and Status change Int) */
        /**************************/
        /* Status Change interrupt*/
        /**************************/
        if(CAN0->STATUS & CAN_STATUS_RXOK_Msk) {
            CAN0->STATUS &= ~CAN_STATUS_RXOK_Msk;   /* Clear RxOK status*/

            printf("RxOK INT\r\n") ;
        }

        if(CAN0->STATUS & CAN_STATUS_TXOK_Msk) {
            CAN0->STATUS &= ~CAN_STATUS_TXOK_Msk;    /* Clear TxOK status*/

            printf("TxOK INT\r\n") ;
        }

        /**************************/
        /* Error Status interrupt */
        /**************************/
        if(CAN0->STATUS & CAN_STATUS_EWARN_Msk) {
            printf("EWARN INT\r\n") ;
        }

        if(CAN0->STATUS & CAN_STATUS_BOFF_Msk) {
            printf("BOFF INT\r\n") ;
        }
    } else if(u8IIDRstatus != 0) {
        printf("=> Interrupt Pointer = %d\r\n", CAN0->IIDR - 1);

        CAN_MsgInterrupt(CAN0, u8IIDRstatus);

        CAN_CLR_INT_PENDING_BIT(CAN0, ((CAN0->IIDR) - 1)); /* Clear Interrupt Pending */
    } else if(CAN0->WU_STATUS == 1) {
        printf("Wake up\r\n");

        CAN0->WU_STATUS = 0;    /* Write '0' to clear */
    }

}

/*----------------------------------------------------------------------------*/
/*  Send Rx Msg by Normal Mode Function (With Message RAM)                    */
/*----------------------------------------------------------------------------*/
void Test_NormalMode_Rx(CAN_T *tCAN)
{	
   if(CAN_SetRxMsg(tCAN, MSG(0), CAN_STD_ID, 0x002) == FALSE) {//接收停止帧、错误帧
       printf("Set Rx Msg Object failed\r\n");
       return;
   }
   if(CAN_SetRxMsg(tCAN, MSG(1), CAN_STD_ID, 0x402) == FALSE) {//接收其他帧
       printf("Set Rx Msg Object failed\r\n");
       return;
   }
   if(CAN_SetRxMsg(tCAN, MSG(2), CAN_STD_ID, 0X7FF) == FALSE) {//接收广播帧
       printf("Set Rx Msg Object failed\r\n");
       return;
   }   
    /* INT Mode */
    CAN_EnableInt(tCAN, CAN_CON_IE_Msk);     /* Enable CAN interrupt and corresponding NVIC of CAN */
    NVIC_SetPriority(CAN0_IRQn, (1 << __NVIC_PRIO_BITS) - 2);   /* Install CAN call back functions */
    NVIC_EnableIRQ(CAN0_IRQn);
}



/*----------------------------------------------------------------------------*/
/*  Send Tx Msg by Normal Mode Function (With Message RAM)                    */
/*----------------------------------------------------------------------------*/
void Test_NormalMode_Tx(CAN_T *tCAN)
{
    STR_CANMSG_T tMsg;

    /* Send a 11-bit Standard Identifier message */
    tMsg.FrameType = CAN_DATA_FRAME;
    tMsg.IdType   = CAN_STD_ID;
    tMsg.Id       = 0x7FF;
    tMsg.DLC      = 2;
    tMsg.Data[0]  = 7;
    tMsg.Data[1]  = 0xFF;

    if(CAN_Transmit(tCAN, MSG(10), &tMsg) == FALSE) { // Configure Msg RAM and send the Msg in the RAM
//        printf("Set Tx Msg Object failed\r\n");
        return;
    }
//		printf("MSG(1).Send STD_ID:0x7FF, Data[07,FF]done\r\n");
}
/*******************************************************************************
* 函数名	: STR_CANMSG_BSP CANReceivedProtocol(STR_CANMSG_T* Msg)
* 功能描述	: CAN接收到的信息解析
* 输入参数  : CAN接收到的数据信息
* 返回参数  : 按照协议解析之后的数据
********************************************************************************/
STR_CANMSG_BSP CANReceivedProtocol(STR_CANMSG_T* Msg) //协议解析
{
	uint16_t uitemp = 0;
	STR_CANMSG_BSP StructsReturn;
		
	StructsReturn.ErrFrame = (Msg->Id & ERRFRAME)>>10;//错误帧标识符       0：错误帧，以及停止命令   1：其它帧
	StructsReturn.DestMACID   =  Msg->Id & DESTMACID;  //提取目标节点ID
	StructsReturn.FuncID = (Msg->Data[0] & FUNCID) >> 5;// 提取功能码
	StructsReturn.ack = Msg->Data[0] & ACK;//提取响应标识位	
	uitemp = (Msg->Data[0] & SRCMACID) << 8 ;
	StructsReturn.SrcMACID = uitemp + Msg->Data[1];//提取源节点ID
	StructsReturn.byte = Msg->Data[2];//提取控制指令、状态信息、错误代码
	return StructsReturn;
}
/*******************************************************************************
* 函数名	:STR_RMSG_BSP CANReceivedProtocol(STR_CANMSG_BSP* Msg)
* 功能描述	: 对接收到的经过协议处理之后的信息，进一步提取有效信息
* 输入参数  : 协议解析之后的数据
* 返回参数  : OUTPUT需要判断执行的信息
********************************************************************************/
STR_RMSG_BSP CANExecuteMsg(STR_CANMSG_BSP* Msg)  //协议解析
{	
	STR_RMSG_BSP StructsReturn;//返回需要判断执行的信息
	if(Msg->SrcMACID == UNIT_PLC)//是PLC发送出来的吗
	{
		if(Msg->ErrFrame ==0){//接收错误帧或停止帧时关闭所有输出
			INVERSION_OFF;//反向接触器关闭
			FORWARD_OFF;//正向接触器关闭 
		}
	 
		if(Msg->ack & ACK)//响应标识位
		{
			StructsReturn.ack = 0;//不需要应答 
			#ifdef BSP_DEBUG_MODE
				printf("This frame don't need Response.\r\n");
			#endif 	
		}
		else
		{
		   StructsReturn.ack = 1;//需要应答 
		    #ifdef BSP_DEBUG_MODE
				printf("This frame Request Response.\r\n");
		    #endif 	    		
		}
	    if(Msg->FuncID == FUNC_ERR)//出错响应
		{
			INVERSION_OFF;//反向接触器关闭
			FORWARD_OFF;//正向接触器关闭 
			StructsReturn.motion = 0;//清动作执行标志
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is error\r\n");
			#endif 			
		}
		else if(Msg->FuncID == FUNC_HAND)//手动模式
		{
			StructsReturn.motion = 1;//置动作执行标志	
			StructsReturn.exeID = Msg->byte;
			HandleTimeCnt = 0;//手动时间计数器清零
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is hand movement\r\n");
			#endif 	
		}	
		else if(Msg->FuncID == FUNC_AUTO)//自动模式
		{
			StructsReturn.motion = 1;//置动作执行标志
			StructsReturn.exeID = Msg->byte;
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is Automatic\r\n");
			#endif 			 
		}	
		else if(Msg->FuncID == FUNC_CONNECT)//建立连接
		{
			App_Flag.connected = 1;//建立连接
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is connect\r\n");
			#endif 
		}
		
/* 		if(Msg->DestMACID == UNIT_BROADCAST)//是广播帧吗？
		{
			if(Msg->byte == EXE_STOP_ALL)//当 “控制指令”时“EXE_STOP_ALL”时， 每个接收到都全部停止
			{
				INVERSION_OFF;//反向接触器关闭
				FORWARD_OFF;//正向接触器关闭 
				StructsReturn.motion = 0;//清动作执行标志
				#ifdef BSP_DEBUG_MODE
				  	printf("Stop all\r\n");
				#endif 								
			}				
		} */
	}
	return StructsReturn;	
}
/*******************************************************************************
* 函数名	: void FrameToSend(STR_CANMSG_T* Msg) 
* 功能描述	:向CAN总线发送帧
* 输入参数 1  :发送帧的信息
* 返回参数     : 无
********************************************************************************/
void FrameToSend(STR_CANMSG_BSP* MsgToSend) 
{
	STR_CANMSG_T tMsg;
	uint16_t uitemp = 0;
    uint8_t  uctemp = 0;
    static uint8_t SendMSGNum=10;//	发送邮箱编号
	#ifdef BSP_DEBUG_MODE
	//	printf("MsgToSend->FrameType=0x%X\r\n",MsgToSend->FrameType);
	#endif  
	if(MsgToSend->FuncID == FUNC_ERR)	//反馈给PLC错误帧
	{			
		uitemp  = 0;//错误帧最高位是0  
		tMsg.Id = uitemp | UNIT_PLC;//发送给目标地址
		uctemp = FUNC_ERR << 5;//错误功能码
		//uitemp = UNIT_DIR >> 8;
		//tMsg.Data[0]  = uctemp | uitemp | ACK;//发送功能码 应答标识位  源节点高两位
		tMsg.Data[0]  = uctemp | ACK;//发送功能码 应答标识位  源节点高两位
		tMsg.Data[1]  = UNIT_DIR;	//发送源节点低八位
		tMsg.Data[2]  = MsgToSend->byte;
		/* Send a 11-bit Standard Identifier message */
		tMsg.FrameType = CAN_DATA_FRAME;
		tMsg.IdType   = CAN_STD_ID;
	   // tMsg.Id       = 0x7FF;
		tMsg.DLC      = 3;
		//tMsg.Data[0]  = 7;
		//tMsg.Data[1]  = 0xFF;
		  //tMsg.Data[2]  = 0xFF;
		  
	    if(CAN_Transmit(CAN0, MSG(SendMSGNum), &tMsg) == FALSE) { // Configure Msg RAM and send the Msg in the RAM
			#ifdef BSP_DEBUG_MODE
				printf("Set Tx Msg Object failed\r\n");
			#endif  
			return;
	    }
		#ifdef BSP_DEBUG_MODE
			printf("MSG(%d) Send done\r\n",SendMSGNum);				 
		#endif 								 
	}
	else if(MsgToSend->ack == 0)//反馈给PLC应答帧
	{
		uitemp  = 0x400;//应答帧最高位是1  
		tMsg.Id = uitemp | UNIT_PLC;//发送给目标地址
		uctemp = MsgToSend->FuncID << 5;///
		//uitemp = unit >> 8;
		//tMsg.Data[0]  = uctemp | uitemp | ACK;//发送功能码 应答标识位  源节点高两位
		tMsg.Data[0]  = uctemp | ACK;//发送功能码 应答标识位  源节点高两位
		tMsg.Data[1]  = UNIT_DIR;	//发送源节点低八位
		tMsg.Data[2]  = MsgToSend->byte;
				
		/* Send a 11-bit Standard Identifier message */
		tMsg.FrameType = CAN_DATA_FRAME;
		tMsg.IdType   = CAN_STD_ID;
	     // tMsg.Id       = 0x7FF;
		tMsg.DLC      = 3;
		//tMsg.Data[0]  = 7;
		//tMsg.Data[1]  = 0xFF;
	    //tMsg.Data[2]  = 0xFF;
			  
	   if(CAN_Transmit(CAN0, MSG(SendMSGNum), &tMsg) == FALSE) 
	   { // Configure Msg RAM and send the Msg in the RAM
			#ifdef BSP_DEBUG_MODE
				printf("Set Tx Msg Object failed\r\n");
			#endif  
			return;
	    }
		#ifdef BSP_DEBUG_MODE
			 printf("MSG(%d) Send done\r\n",SendMSGNum);
		#endif 					  	  
	}
	  if(SendMSGNum > 30)	 //发送邮箱编号  10-31
	  {
		SendMSGNum = 10;
	  }
	  else
	  {
		SendMSGNum ++;
	  } 
}
void  bsp_CANInit(void)
{	
	CircularBufferInit(&CANCircularBuf_RX,CAN_RxBuf,CAN_RX_BUFFER_SIZE);//初始化接收环形队列
	CircularBufferInit(&CANCircularBuf_TX,CAN_TxBuf,CAN_TX_BUFFER_SIZE);//初始化发送环形队列
	CAN_Open(CAN0,20000,CAN_NORMAL_MODE);//Set the CAN speed  20K    
	
	Test_NormalMode_Rx(CAN0);	
	
	//CAN_EnterTestMode(CAN0, CAN_TEST_LBACK_Msk);		
}

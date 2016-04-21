#include "bsp.h"

/* Declare a CAN message structure */
STR_CANMSG_T rrMsg;

STR_CANMSG_BSP CAN_RxBuf[CAN_RX_BUFFER_SIZE];//���ջ�������
STR_CANMSG_BSP CAN_TxBuf[CAN_TX_BUFFER_SIZE];//���ͻ�������
sCircularBuffer CANCircularBuf_RX;//���ջ��λ��������ݽṹ
sCircularBuffer CANCircularBuf_TX;//���ͻ��λ��������ݽṹ
uint8_t HandleTimeCnt = 0;// //�ֶ�ʱ�������
static STR_CANMSG_BSP CANReceivedProtocol(STR_CANMSG_T* Msg); //Э�����

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
		StrMsgToWrite = CANReceivedProtocol(&rrMsg);//��ȡ�����������
		CBWriteOne(&CANCircularBuf_RX,StrMsgToWrite);//����ջ���������д����
		#ifdef BSP_DEBUG_MODE
			printf("Msg-0 INT and Callback\r\n");
			CAN_ShowMsg(&rrMsg);
		#endif        
    }
    if(u32IIDR == 1 + 1) {       
        CAN_Receive(tCAN, 1, &rrMsg);
		StrMsgToWrite = CANReceivedProtocol(&rrMsg);//��ȡ�����������
		CBWriteOne(&CANCircularBuf_RX,StrMsgToWrite);//����ջ���������д����		
		#ifdef BSP_DEBUG_MODE
			printf("Msg-1 INT and Callback\r\n");
			CAN_ShowMsg(&rrMsg);
		#endif         
    }	
    if(u32IIDR == 2 + 1) {       
        CAN_Receive(tCAN, 2, &rrMsg);
		StrMsgToWrite = CANReceivedProtocol(&rrMsg);//��ȡ�����������
		CBWriteOne(&CANCircularBuf_RX,StrMsgToWrite);//����ջ���������д����		
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
   if(CAN_SetRxMsg(tCAN, MSG(0), CAN_STD_ID, 0x002) == FALSE) {//����ֹͣ֡������֡
       printf("Set Rx Msg Object failed\r\n");
       return;
   }
   if(CAN_SetRxMsg(tCAN, MSG(1), CAN_STD_ID, 0x402) == FALSE) {//��������֡
       printf("Set Rx Msg Object failed\r\n");
       return;
   }
   if(CAN_SetRxMsg(tCAN, MSG(2), CAN_STD_ID, 0X7FF) == FALSE) {//���չ㲥֡
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
* ������	: STR_CANMSG_BSP CANReceivedProtocol(STR_CANMSG_T* Msg)
* ��������	: CAN���յ�����Ϣ����
* �������  : CAN���յ���������Ϣ
* ���ز���  : ����Э�����֮�������
********************************************************************************/
STR_CANMSG_BSP CANReceivedProtocol(STR_CANMSG_T* Msg) //Э�����
{
	uint16_t uitemp = 0;
	STR_CANMSG_BSP StructsReturn;
		
	StructsReturn.ErrFrame = (Msg->Id & ERRFRAME)>>10;//����֡��ʶ��       0������֡���Լ�ֹͣ����   1������֡
	StructsReturn.DestMACID   =  Msg->Id & DESTMACID;  //��ȡĿ��ڵ�ID
	StructsReturn.FuncID = (Msg->Data[0] & FUNCID) >> 5;// ��ȡ������
	StructsReturn.ack = Msg->Data[0] & ACK;//��ȡ��Ӧ��ʶλ	
	uitemp = (Msg->Data[0] & SRCMACID) << 8 ;
	StructsReturn.SrcMACID = uitemp + Msg->Data[1];//��ȡԴ�ڵ�ID
	StructsReturn.byte = Msg->Data[2];//��ȡ����ָ�״̬��Ϣ���������
	return StructsReturn;
}
/*******************************************************************************
* ������	:STR_RMSG_BSP CANReceivedProtocol(STR_CANMSG_BSP* Msg)
* ��������	: �Խ��յ��ľ���Э�鴦��֮�����Ϣ����һ����ȡ��Ч��Ϣ
* �������  : Э�����֮�������
* ���ز���  : OUTPUT��Ҫ�ж�ִ�е���Ϣ
********************************************************************************/
STR_RMSG_BSP CANExecuteMsg(STR_CANMSG_BSP* Msg)  //Э�����
{	
	STR_RMSG_BSP StructsReturn;//������Ҫ�ж�ִ�е���Ϣ
	if(Msg->SrcMACID == UNIT_PLC)//��PLC���ͳ�������
	{
		if(Msg->ErrFrame ==0){//���մ���֡��ֹͣ֡ʱ�ر��������
			INVERSION_OFF;//����Ӵ����ر�
			FORWARD_OFF;//����Ӵ����ر� 
		}
	 
		if(Msg->ack & ACK)//��Ӧ��ʶλ
		{
			StructsReturn.ack = 0;//����ҪӦ�� 
			#ifdef BSP_DEBUG_MODE
				printf("This frame don't need Response.\r\n");
			#endif 	
		}
		else
		{
		   StructsReturn.ack = 1;//��ҪӦ�� 
		    #ifdef BSP_DEBUG_MODE
				printf("This frame Request Response.\r\n");
		    #endif 	    		
		}
	    if(Msg->FuncID == FUNC_ERR)//������Ӧ
		{
			INVERSION_OFF;//����Ӵ����ر�
			FORWARD_OFF;//����Ӵ����ر� 
			StructsReturn.motion = 0;//�嶯��ִ�б�־
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is error\r\n");
			#endif 			
		}
		else if(Msg->FuncID == FUNC_HAND)//�ֶ�ģʽ
		{
			StructsReturn.motion = 1;//�ö���ִ�б�־	
			StructsReturn.exeID = Msg->byte;
			HandleTimeCnt = 0;//�ֶ�ʱ�����������
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is hand movement\r\n");
			#endif 	
		}	
		else if(Msg->FuncID == FUNC_AUTO)//�Զ�ģʽ
		{
			StructsReturn.motion = 1;//�ö���ִ�б�־
			StructsReturn.exeID = Msg->byte;
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is Automatic\r\n");
			#endif 			 
		}	
		else if(Msg->FuncID == FUNC_CONNECT)//��������
		{
			App_Flag.connected = 1;//��������
			#ifdef BSP_DEBUG_MODE
			  	printf("Received FuncID is connect\r\n");
			#endif 
		}
		
/* 		if(Msg->DestMACID == UNIT_BROADCAST)//�ǹ㲥֡��
		{
			if(Msg->byte == EXE_STOP_ALL)//�� ������ָ�ʱ��EXE_STOP_ALL��ʱ�� ÿ�����յ���ȫ��ֹͣ
			{
				INVERSION_OFF;//����Ӵ����ر�
				FORWARD_OFF;//����Ӵ����ر� 
				StructsReturn.motion = 0;//�嶯��ִ�б�־
				#ifdef BSP_DEBUG_MODE
				  	printf("Stop all\r\n");
				#endif 								
			}				
		} */
	}
	return StructsReturn;	
}
/*******************************************************************************
* ������	: void FrameToSend(STR_CANMSG_T* Msg) 
* ��������	:��CAN���߷���֡
* ������� 1  :����֡����Ϣ
* ���ز���     : ��
********************************************************************************/
void FrameToSend(STR_CANMSG_BSP* MsgToSend) 
{
	STR_CANMSG_T tMsg;
	uint16_t uitemp = 0;
    uint8_t  uctemp = 0;
    static uint8_t SendMSGNum=10;//	����������
	#ifdef BSP_DEBUG_MODE
	//	printf("MsgToSend->FrameType=0x%X\r\n",MsgToSend->FrameType);
	#endif  
	if(MsgToSend->FuncID == FUNC_ERR)	//������PLC����֡
	{			
		uitemp  = 0;//����֡���λ��0  
		tMsg.Id = uitemp | UNIT_PLC;//���͸�Ŀ���ַ
		uctemp = FUNC_ERR << 5;//��������
		//uitemp = UNIT_DIR >> 8;
		//tMsg.Data[0]  = uctemp | uitemp | ACK;//���͹����� Ӧ���ʶλ  Դ�ڵ����λ
		tMsg.Data[0]  = uctemp | ACK;//���͹����� Ӧ���ʶλ  Դ�ڵ����λ
		tMsg.Data[1]  = UNIT_DIR;	//����Դ�ڵ�Ͱ�λ
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
	else if(MsgToSend->ack == 0)//������PLCӦ��֡
	{
		uitemp  = 0x400;//Ӧ��֡���λ��1  
		tMsg.Id = uitemp | UNIT_PLC;//���͸�Ŀ���ַ
		uctemp = MsgToSend->FuncID << 5;///
		//uitemp = unit >> 8;
		//tMsg.Data[0]  = uctemp | uitemp | ACK;//���͹����� Ӧ���ʶλ  Դ�ڵ����λ
		tMsg.Data[0]  = uctemp | ACK;//���͹����� Ӧ���ʶλ  Դ�ڵ����λ
		tMsg.Data[1]  = UNIT_DIR;	//����Դ�ڵ�Ͱ�λ
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
	  if(SendMSGNum > 30)	 //����������  10-31
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
	CircularBufferInit(&CANCircularBuf_RX,CAN_RxBuf,CAN_RX_BUFFER_SIZE);//��ʼ�����ջ��ζ���
	CircularBufferInit(&CANCircularBuf_TX,CAN_TxBuf,CAN_TX_BUFFER_SIZE);//��ʼ�����ͻ��ζ���
	CAN_Open(CAN0,20000,CAN_NORMAL_MODE);//Set the CAN speed  20K    
	
	Test_NormalMode_Rx(CAN0);	
	
	//CAN_EnterTestMode(CAN0, CAN_TEST_LBACK_Msk);		
}

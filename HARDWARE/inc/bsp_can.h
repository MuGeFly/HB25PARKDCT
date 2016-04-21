#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

//CANͨѶ�ṹ�嶨��
#define ERRFRAME    0x400
#define DESTMACID   0x3FF
#define FUNCID      0xE0
#define ACK         0x10
#define SRCMACID	  0x03

//���峵��ڵ�ID����
#define UNIT_PLC	0x01  //PLC 
#define UNIT_DIR	0x02  //������ư�

//������
#define FUNC_ERR   	 0X00  //������Ӧ
#define FUNC_HAND  	 0X01 //�ֶ�ģʽ
#define FUNC_AUTO 	 0X02 //�Զ�ģʽ
#define FUNC_TRIGGER 0X03 //����˿��¼���������
#define FUNC_CONNECT 0X04 //��������

//����ָ��
#define EXE_FORWARD_ON	  0x01//����Ӵ�����
#define EXE_FORWARD_OFF	  0x02//����Ӵ����ر� 
#define EXE_INVERSION_ON  0x04 //����Ӵ�����
#define EXE_INVERSION_OFF 0x08//����Ӵ����ر�
#define EXE_ALARM_ON	  0x10//��������
#define EXE_ALARM_OFF	  0x20//�������ر�
//�������
#define	ERR_OVERLENGTH_FRONT	0x01  //ǰ����
#define	ERR_OVERLENGTH_REAR 	0x02  //�󳬳�
//��Ҫ����֡����  ����֡��Ӧ��֡
#define ERR_FRAME   	 0x01  //������PLC����֡
#define ACK_FRAME        0x02  //������PLCӦ��֡
//��������ĳ��������2���ݴη����Լ����������������������λ����
#define CAN_RX_BUFFER_SIZE  8
#define CAN_RX_BUFFER_SIZE_MASK  (CAN_RX_BUFFER_SIZE-1)

//��������ĳ��������2���ݴη����Լ����������������������λ����
#define CAN_TX_BUFFER_SIZE  8
#define CAN_TX_BUFFER_SIZE_MASK  (CAN_TX_BUFFER_SIZE-1)

/*---------------------------------------------------------------------------------------------------------*/
/* CAN message structure                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    unsigned  char  ErrFrame;	//0������֡���Լ�ֹͣ����   1������֡
    unsigned  int   DestMACID;	//Ŀ��ڵ�
    unsigned  char  FuncID;	 	//������
    unsigned  char  ack; 	    //��Ӧ��ʶλ
    unsigned  int   SrcMACID;	//Դ�ڵ�
	unsigned  char  byte; 	 	//����ָ�״̬��Ϣ���������
} STR_CANMSG_BSP;

/*---------------------------------------------------------------------------------------------------------*/
//���͸�OUTPUT����Ҫִ�е���Ϣ                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    unsigned  char  motion;	//����ָ�0��ֹͣ����   1��ִ�ж���
    unsigned  char  exeID;	 	//ִ��ָ��
    unsigned  char  ack; 	    //��Ӧ��ʶλ
} STR_RMSG_BSP;
extern uint8_t HandleTimeCnt;// //�ֶ�ʱ�������
void  bsp_CANInit(void);
STR_RMSG_BSP CANExecuteMsg(STR_CANMSG_BSP* Msg);  //Э�����
void FrameToSend(STR_CANMSG_BSP* MsgToSend);//��CAN���߷���֡
#endif

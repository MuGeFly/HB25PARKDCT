#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

//CAN通讯结构体定义
#define ERRFRAME    0x400
#define DESTMACID   0x3FF
#define FUNCID      0xE0
#define ACK         0x10
#define SRCMACID	  0x03

//立体车库节点ID定义
#define UNIT_PLC	0x01  //PLC 
#define UNIT_DIR	0x02  //方向控制板

//功能码
#define FUNC_ERR   	 0X00  //出错响应
#define FUNC_HAND  	 0X01 //手动模式
#define FUNC_AUTO 	 0X02 //自动模式
#define FUNC_TRIGGER 0X03 //输入端口事件触发传送
#define FUNC_CONNECT 0X04 //建立连接

//控制指令
#define EXE_FORWARD_ON	  0x01//正向接触器打开
#define EXE_FORWARD_OFF	  0x02//正向接触器关闭 
#define EXE_INVERSION_ON  0x04 //反向接触器打开
#define EXE_INVERSION_OFF 0x08//反向接触器关闭
#define EXE_ALARM_ON	  0x10//报警器打开
#define EXE_ALARM_OFF	  0x20//报警器关闭
//错误代码
#define	ERR_OVERLENGTH_FRONT	0x01  //前超长
#define	ERR_OVERLENGTH_REAR 	0x02  //后超长
//需要发送帧类型  错误帧或应答帧
#define ERR_FRAME   	 0x01  //反馈给PLC错误帧
#define ACK_FRAME        0x02  //反馈给PLC应答帧
//接受数组的长度最好是2的幂次方这个约束条件，这里巧妙运用了位运算
#define CAN_RX_BUFFER_SIZE  8
#define CAN_RX_BUFFER_SIZE_MASK  (CAN_RX_BUFFER_SIZE-1)

//发送数组的长度最好是2的幂次方这个约束条件，这里巧妙运用了位运算
#define CAN_TX_BUFFER_SIZE  8
#define CAN_TX_BUFFER_SIZE_MASK  (CAN_TX_BUFFER_SIZE-1)

/*---------------------------------------------------------------------------------------------------------*/
/* CAN message structure                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    unsigned  char  ErrFrame;	//0：错误帧，以及停止命令   1：其它帧
    unsigned  int   DestMACID;	//目标节点
    unsigned  char  FuncID;	 	//功能码
    unsigned  char  ack; 	    //响应标识位
    unsigned  int   SrcMACID;	//源节点
	unsigned  char  byte; 	 	//控制指令、状态信息、错误代码
} STR_CANMSG_BSP;

/*---------------------------------------------------------------------------------------------------------*/
//传送给OUTPUT，需要执行的信息                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    unsigned  char  motion;	//动作指令：0：停止动作   1：执行动作
    unsigned  char  exeID;	 	//执行指令
    unsigned  char  ack; 	    //响应标识位
} STR_RMSG_BSP;
extern uint8_t HandleTimeCnt;// //手动时间计数器
void  bsp_CANInit(void);
STR_RMSG_BSP CANExecuteMsg(STR_CANMSG_BSP* Msg);  //协议解析
void FrameToSend(STR_CANMSG_BSP* MsgToSend);//向CAN总线发送帧
#endif

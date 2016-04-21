/*
* Copyright (c) 2016,杭州灵易利自动化技术有限公司研发部
* All rights reserved.
*
* 文件名称： 方向控制板
* 文件标识： 见配置管理计划书
* 摘 要： 升降横移类立体车库
*        1、接收PLC发来的指令，打开和关闭对应的继电器
*        2、检测到超长故障信息，打开报警器；自动模式下发送给PLC超长故障；手动和静止时，不上传故障信息
*		 3、生产时请注释掉 BSP_DEBUG_MODE
* 当前版本： 1.0
* 作 者：    马 孝 志
* 完成日期： 2016年4月14日
*
* 取代版本： 1.0
* 原作者 ：  马 孝 志
* 完成日期： 2016年4月14日
*/
#ifndef _BSP_H_
#define _BSP_H

#include <stdio.h>
#include <string.h>
#include "mytypes.h"
#include "NUC230_240.h"
/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_IO.h"
#include "bsp_can.h"


#define PLL_CLOCK           48000000

#define BSP_DEBUG_MODE         //调试时请打开 生产时请注释  是否编译调试代码  例如print打印函数
#ifdef BSP_DEBUG_MODE
#endif 

typedef STR_CANMSG_BSP tElement;//缓冲区数据结构，方便日后缓冲区移植其他数据结构
//环形缓冲区数据结构
typedef struct CircularBuffer
{
	tElement *DataBuf;//缓冲区指向的位置指针
	u8		BufLen;//缓冲区长度,必须是2的幂次方
	u8		WriteIndex;//缓冲区写位置索引，生产者
	u8		ReadIndex;//缓存区读位置索引，消费者
}sCircularBuffer;
enum
{
	eErrorBufferOK,
	eErrorBufferFull,
	eErrorBufferEmpty,
};

void CircularBufferInit(sCircularBuffer *cb,tElement *buf,u8 buflen);
u8 CBFreeDataLenght(sCircularBuffer *cb);
void CBFreeData(sCircularBuffer *cb);
u8 CBReadOne(sCircularBuffer *cb,tElement *Readdata);
u8 CBWriteOne(sCircularBuffer *cb,tElement writedata);

extern STR_CANMSG_BSP CAN_RxBuf[CAN_RX_BUFFER_SIZE];//接收缓存数组
extern STR_CANMSG_BSP CAN_TxBuf[CAN_TX_BUFFER_SIZE];//发送缓存数组
extern sCircularBuffer CANCircularBuf_RX;//接收环形缓冲区数据结构
extern sCircularBuffer CANCircularBuf_TX;//发送环形缓冲区数据结构
/*-----------------------------------------------------------------*/
/*程序运行的各种状态标志位结构体                                                                                  */
/*-----------------------------------------------------------------*/
typedef struct APPFLAGSTRUCT//
{
	unsigned  char connected;//是否已经建立连接
	unsigned  char SendInterval;//发送间隔
	unsigned  char ErrScanlock;//互斥量，俗称原子锁 
	unsigned  char Alarm_PLC;//PLC通过指令让报警器打开，这个时候检测到车辆超长就不应该关闭报警器了
}AppFlagStruct;

extern AppFlagStruct App_Flag;//应用程序标志位
/* 提供给其他C文件调用的函数 */
void  bsp_Init(void);
void  bsp_DelayMS(u32 delayTime);
void  bsp_DelayUs(u32 delayTime);


#endif

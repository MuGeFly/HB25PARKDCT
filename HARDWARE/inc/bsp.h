/*
* Copyright (c) 2016,�����������Զ����������޹�˾�з���
* All rights reserved.
*
* �ļ����ƣ� ������ư�
* �ļ���ʶ�� �����ù���ƻ���
* ժ Ҫ�� �������������峵��
*        1������PLC������ָ��򿪺͹رն�Ӧ�ļ̵���
*        2����⵽����������Ϣ���򿪱��������Զ�ģʽ�·��͸�PLC�������ϣ��ֶ��;�ֹʱ�����ϴ�������Ϣ
*		 3������ʱ��ע�͵� BSP_DEBUG_MODE
* ��ǰ�汾�� 1.0
* �� �ߣ�    �� Т ־
* ������ڣ� 2016��4��14��
*
* ȡ���汾�� 1.0
* ԭ���� ��  �� Т ־
* ������ڣ� 2016��4��14��
*/
#ifndef _BSP_H_
#define _BSP_H

#include <stdio.h>
#include <string.h>
#include "mytypes.h"
#include "NUC230_240.h"
/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_IO.h"
#include "bsp_can.h"


#define PLL_CLOCK           48000000

#define BSP_DEBUG_MODE         //����ʱ��� ����ʱ��ע��  �Ƿ������Դ���  ����print��ӡ����
#ifdef BSP_DEBUG_MODE
#endif 

typedef STR_CANMSG_BSP tElement;//���������ݽṹ�������պ󻺳�����ֲ�������ݽṹ
//���λ��������ݽṹ
typedef struct CircularBuffer
{
	tElement *DataBuf;//������ָ���λ��ָ��
	u8		BufLen;//����������,������2���ݴη�
	u8		WriteIndex;//������дλ��������������
	u8		ReadIndex;//��������λ��������������
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

extern STR_CANMSG_BSP CAN_RxBuf[CAN_RX_BUFFER_SIZE];//���ջ�������
extern STR_CANMSG_BSP CAN_TxBuf[CAN_TX_BUFFER_SIZE];//���ͻ�������
extern sCircularBuffer CANCircularBuf_RX;//���ջ��λ��������ݽṹ
extern sCircularBuffer CANCircularBuf_TX;//���ͻ��λ��������ݽṹ
/*-----------------------------------------------------------------*/
/*�������еĸ���״̬��־λ�ṹ��                                                                                  */
/*-----------------------------------------------------------------*/
typedef struct APPFLAGSTRUCT//
{
	unsigned  char connected;//�Ƿ��Ѿ���������
	unsigned  char SendInterval;//���ͼ��
	unsigned  char ErrScanlock;//���������׳�ԭ���� 
	unsigned  char Alarm_PLC;//PLCͨ��ָ���ñ������򿪣����ʱ���⵽���������Ͳ�Ӧ�ùرձ�������
}AppFlagStruct;

extern AppFlagStruct App_Flag;//Ӧ�ó����־λ
/* �ṩ������C�ļ����õĺ��� */
void  bsp_Init(void);
void  bsp_DelayMS(u32 delayTime);
void  bsp_DelayUs(u32 delayTime);


#endif

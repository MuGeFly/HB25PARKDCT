#ifndef __BSP_INPUTOUTPUT_H
#define __BSP_INPUTOUTPUT_H
//IO���Ŷ���
#define FRONT_IN    PA13  //ǰ�����������Ŷ���
#define REAR_IN     PA14  //�󳬳��������Ŷ���

#define ALARM_ON    PA1=1  //��������
#define ALARM_OFF   PA1=0  //�������ر�
#define RUNLED_ON   PA12=1 //����ָʾ�ƴ�
#define RUNLED_OFF  PA12=0 //����ָʾ�ƹر�
#define INVERSION_ON   PA8=1 //����Ӵ�����
#define INVERSION_OFF  PA8=0 //����Ӵ����ر�
#define FORWARD_ON     PB4=1 //����Ӵ�����
#define FORWARD_OFF    PB4=0 //����Ӵ����ر� 

#define const_key_time1  2    //����ȥ�����ӳٵ�ʱ��
#define const_key_time2  2    //����ȥ�����ӳٵ�ʱ��

#define ALARM_DELAY_TIME  100    //�����ӳ�  500MS 
#define SEND_INTERVAL_TIME  6    //���ͼ��ʱ��30ms
#define HANDLE_TIME_REACH  100    //500msʱ�䵽  û�н��յ�����  ����Ϊ�ֶ�����

#define COMMAND_RELAY_ON    0X01    //�̵�����ָ��
#define COMMAND_RELAY_OFF   0X02    //�̵����ر�ָ��
void bsp_InitGPIO(void);  //��������˿ڳ�ʼ��
void InputOutput_TEST(void);  //����������
void Output(void) ;//���߼����
#endif

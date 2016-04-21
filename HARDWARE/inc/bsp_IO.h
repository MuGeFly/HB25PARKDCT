#ifndef __BSP_INPUTOUTPUT_H
#define __BSP_INPUTOUTPUT_H
//IO引脚定义
#define FRONT_IN    PA13  //前超长输入引脚定义
#define REAR_IN     PA14  //后超长输入引脚定义

#define ALARM_ON    PA1=1  //报警器打开
#define ALARM_OFF   PA1=0  //报警器关闭
#define RUNLED_ON   PA12=1 //运行指示灯打开
#define RUNLED_OFF  PA12=0 //运行指示灯关闭
#define INVERSION_ON   PA8=1 //反向接触器打开
#define INVERSION_OFF  PA8=0 //反向接触器关闭
#define FORWARD_ON     PB4=1 //正向接触器打开
#define FORWARD_OFF    PB4=0 //正向接触器关闭 

#define const_key_time1  2    //按键去抖动延迟的时间
#define const_key_time2  2    //按键去抖动延迟的时间

#define ALARM_DELAY_TIME  100    //报警延迟  500MS 
#define SEND_INTERVAL_TIME  6    //发送间隔时间30ms
#define HANDLE_TIME_REACH  100    //500ms时间到  没有接收到数据  就认为手动结束

#define COMMAND_RELAY_ON    0X01    //继电器打开指令
#define COMMAND_RELAY_OFF   0X02    //继电器关闭指令
void bsp_InitGPIO(void);  //输入输出端口初始化
void InputOutput_TEST(void);  //输入输出检测
void Output(void) ;//按逻辑输出
#endif

#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H

#include "stm32f10x.h"

#define CW  0
#define CCW 1


#define TRUE 1
#define FALSE 0


#define Pulse_width 2000

//系统状态
struct GLOBAL_FLAGS {
  //当步进电机正在运行时，值为1
  unsigned char running:1;
  //当串口接收到数据时，值为1
  unsigned char cmd:1;
  //当驱动器正常输出时,值为1
  unsigned char out_ena:1;
};
extern struct GLOBAL_FLAGS status;

// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 这里我们使用定时器TIM2
#define            MSD_PULSE_TIM                    TIM2
#define            MSD_PULSE_TIM_APBxClock_FUN      RCC_APB1PeriphClockCmd
#define            MSD_PULSE_TIM_CLK                RCC_APB1Periph_TIM2
// 定时器输出PWM通道，PA0是通道1
#define            MSD_PULSE_OCx_Init               TIM_OC2Init
#define            MSD_PULSE_OCx_PreloadConfig      TIM_OC2PreloadConfig
// 定时器中断
#define            MSD_PULSE_TIM_IRQ                TIM2_IRQn
#define            MSD_PULSE_TIM_IRQHandler         TIM2_IRQHandler

// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            MSD_PULSE_TIM_PERIOD             (10-1)
#define            MSD_PULSE_TIM_PSC                (72-1)


// 步进电机脉冲输出通道
#define            MSD_PULSE_GPIO_CLK               RCC_APB2Periph_GPIOA
#define            MSD_PULSE_PORT                   GPIOA
#define            MSD_PULSE_PIN                    GPIO_Pin_1

// 步进电机方向控制
#define            MSD_DIR_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define            MSD_DIR_PORT                     GPIOB
#define            MSD_DIR_PIN                      GPIO_Pin_14

// 步进电机输出使能引脚
#define            MSD_ENA_GPIO_CLK                 RCC_APB2Periph_GPIOC
#define            MSD_ENA_PORT                     GPIOC
#define            MSD_ENA_PIN                      GPIO_Pin_4


#define DIR(a)	if (a == CW)	\
					GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);\
					else		\
					GPIO_SetBits(MSD_DIR_PORT,MSD_DIR_PIN)


void MSD_Init(void);
extern int stepPosition;
void MoveStep(void);
void DisableMoveStep(void);


#endif


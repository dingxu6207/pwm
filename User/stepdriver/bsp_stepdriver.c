#include "bsp_stepdriver.h" 
#include <stdio.h>
#include <math.h>


//ϵͳ���������״̬
struct GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};



/**

  * @brief  ��ʱ���ж����ȼ�����

  * @param  ��

  * @retval ��

  */
static void TIM_NVIC_Config(void)

{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = MSD_PULSE_TIM_IRQ; 	
	// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/**

  * @brief  ��ʼ����������õ�������

  * @param  ��

  * @retval ��

  */
static void MSD_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //���������� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(MSD_PULSE_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_PULSE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_PULSE_PORT, &GPIO_InitStructure);
    
    //���������� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(MSD_DIR_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_DIR_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);
    
    //���ʹ����� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(MSD_ENA_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_ENA_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_ENA_PORT,MSD_ENA_PIN);
}

///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *  TIM_CounterMode		   TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

static void TIM_Mode_Config(void)
{
  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;   //ʱ���ṹ���ʼ�
	TIM_OCInitTypeDef  TIM_OCInitStructure;           //����ȽϽṹ���ʼ��
	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	MSD_PULSE_TIM_APBxClock_FUN(MSD_PULSE_TIM_CLK,ENABLE);

    /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1�����ں����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=MSD_PULSE_TIM_PERIOD;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= MSD_PULSE_TIM_PSC;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ�����ֵΪ255
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(MSD_PULSE_TIM, &TIM_TimeBaseStructure);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	//TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ����ΪPWMģʽ2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �����������
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; 
	// ����ռ�ձȴ�С
	TIM_OCInitStructure.TIM_Pulse = MSD_PULSE_TIM_PERIOD/2;
	// ���ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    
	MSD_PULSE_OCx_Init(MSD_PULSE_TIM, &TIM_OCInitStructure);
    //ʹ��TIM1_CH1Ԥװ�ؼĴ���
	MSD_PULSE_OCx_PreloadConfig(MSD_PULSE_TIM, TIM_OCPreload_Enable);
    //ʹ��TIM1Ԥװ�ؼĴ���
    TIM_ARRPreloadConfig(MSD_PULSE_TIM, ENABLE); 
    
    //�����ж�Դ��ֻ�����ʱ���ж�
    TIM_UpdateRequestConfig(MSD_PULSE_TIM,TIM_UpdateSource_Regular);
	// ����жϱ�־λ
	TIM_ClearITPendingBit(MSD_PULSE_TIM, TIM_IT_Update);
    // ʹ���ж�
    TIM_ITConfig(MSD_PULSE_TIM, TIM_IT_Update, ENABLE);
	// ʹ�ܼ�����
	TIM_Cmd(MSD_PULSE_TIM, DISABLE);

	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
    //TIM_CtrlPWMOutputs(MSD_PULSE_TIM, ENABLE);
}



void MSD_Init(void)
{
    MSD_GPIO_Config();
    
    TIM_NVIC_Config();

    TIM_Mode_Config();    
}


/**

  * @brief  �������嶨ʱ�����ж���Ӧ����ÿ��һ����������˶�״̬

  * @param  ��

  * @retval ��

  */
int stepPosition = 0;
void MSD_PULSE_TIM_IRQHandler(void)
{
	if (TIM_GetITStatus(MSD_PULSE_TIM, TIM_IT_Update) != RESET)
	{
		stepPosition++;
		/* Clear MSD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    	TIM_ClearITPendingBit(MSD_PULSE_TIM, TIM_IT_Update);
	}
}

void MoveStep(void)
{
	//���ö�ʱ����װֵ	
    TIM_SetAutoreload(MSD_PULSE_TIM, Pulse_width);
    //����ռ�ձ�Ϊ50%	
    TIM_SetCompare2(MSD_PULSE_TIM, Pulse_width>>1);
    //ʹ�ܶ�ʱ��	      
    TIM_Cmd(MSD_PULSE_TIM, ENABLE);

		MSD_PULSE_TIM->CCER |= 1<<4; //ʹ�����
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
    //TIM_CtrlPWMOutputs(MSD_PULSE_TIM, ENABLE);

}

void DisableMoveStep(void)
{
	//ʹ�ܶ�ʱ��	      
    TIM_Cmd(MSD_PULSE_TIM, DISABLE);

	MSD_PULSE_TIM->CCER &= ~(1<<4); //��ֹ���
	//TIM_CtrlPWMOutputs(MSD_PULSE_TIM, DISABLE);
}



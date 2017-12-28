#include "bsp_stepdriver.h" 
#include <stdio.h>
#include <math.h>


//ÏµÍ³µç»ú¡¢´®¿Ú×´Ì¬
struct GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};



/**

  * @brief  ¶¨Ê±Æ÷ÖÐ¶ÏÓÅÏÈ¼¶ÅäÖÃ

  * @param  ÎÞ

  * @retval ÎÞ

  */
static void TIM_NVIC_Config(void)

{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // ÉèÖÃÖÐ¶Ï×éÎª0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// ÉèÖÃÖÐ¶ÏÀ´Ô´
    NVIC_InitStructure.NVIC_IRQChannel = MSD_PULSE_TIM_IRQ; 	
	// ÉèÖÃÇÀÕ¼ÓÅÏÈ¼¶
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ÉèÖÃ×ÓÓÅÏÈ¼¶
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/**

  * @brief  ³õÊ¼»¯µç»úÇý¶¯ÓÃµ½µÄÒý½Å

  * @param  ÎÞ

  * @retval ÎÞ

  */
static void MSD_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //µç»úÂö³åÊä³ö GPIO ³õÊ¼»¯
    RCC_APB2PeriphClockCmd(MSD_PULSE_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_PULSE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_PULSE_PORT, &GPIO_InitStructure);
    
    //µç»ú·½ÏòÊä³ö GPIO ³õÊ¼»¯
    RCC_APB2PeriphClockCmd(MSD_DIR_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_DIR_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);
    
    //µç»úÊ¹ÄÜÊä³ö GPIO ³õÊ¼»¯
    RCC_APB2PeriphClockCmd(MSD_ENA_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_ENA_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_ENA_PORT,MSD_ENA_PIN);
}

///*
// * ×¢Òâ£ºTIM_TimeBaseInitTypeDef½á¹¹ÌåÀïÃæÓÐ5¸ö³ÉÔ±£¬TIM6ºÍTIM7µÄ¼Ä´æÆ÷ÀïÃæÖ»ÓÐ
// * TIM_PrescalerºÍTIM_Period£¬ËùÒÔÊ¹ÓÃTIM6ºÍTIM7µÄÊ±ºòÖ»Ðè³õÊ¼»¯ÕâÁ½¸ö³ÉÔ±¼´¿É£¬
// * ÁíÍâÈý¸ö³ÉÔ±ÊÇÍ¨ÓÃ¶¨Ê±Æ÷ºÍ¸ß¼¶¶¨Ê±Æ÷²ÅÓÐ.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ¶¼ÓÐ
// *  TIM_CounterMode		   TIMx,x[6,7]Ã»ÓÐ£¬ÆäËû¶¼ÓÐ
// *  TIM_Period               ¶¼ÓÐ
// *  TIM_ClockDivision        TIMx,x[6,7]Ã»ÓÐ£¬ÆäËû¶¼ÓÐ
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]²ÅÓÐ
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWMÐÅºÅ ÖÜÆÚºÍÕ¼¿Õ±ÈµÄ¼ÆËã--------------- */
// ARR £º×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷µÄÖµ
// CLK_cnt£º¼ÆÊýÆ÷µÄÊ±ÖÓ£¬µÈÓÚ Fck_int / (psc+1) = 72M/(psc+1)
// PWM ÐÅºÅµÄÖÜÆÚ T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// Õ¼¿Õ±ÈP=CCR/(ARR+1)

static void TIM_Mode_Config(void)
{
  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;   //Ê±»ù½á¹¹Ìå³õÊ¼»
	TIM_OCInitTypeDef  TIM_OCInitStructure;           //Êä³ö±È½Ï½á¹¹Ìå³õÊ¼»¯
	
	// ¿ªÆô¶¨Ê±Æ÷Ê±ÖÓ,¼´ÄÚ²¿Ê±ÖÓCK_INT=72M
	MSD_PULSE_TIM_APBxClock_FUN(MSD_PULSE_TIM_CLK,ENABLE);

    /*--------------------Ê±»ù½á¹¹Ìå³õÊ¼»¯-------------------------*/
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    // ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷µÄÖµ£¬ÀÛ¼ÆTIM_Period+1¸öÖÜÆÚºó²úÉúÒ»¸ö¸üÐÂ»òÕßÖÐ¶Ï
	TIM_TimeBaseStructure.TIM_Period=MSD_PULSE_TIM_PERIOD;	
	// Çý¶¯CNT¼ÆÊýÆ÷µÄÊ±ÖÓ = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= MSD_PULSE_TIM_PSC;	
	// Ê±ÖÓ·ÖÆµÒò×Ó £¬ÅäÖÃËÀÇøÊ±¼äÊ±ÐèÒªÓÃµ½
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ¼ÆÊýÆ÷¼ÆÊýÄ£Ê½£¬ÉèÖÃÎªÏòÉÏ¼ÆÊý
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// ÖØ¸´¼ÆÊýÆ÷µÄÖµ£¬×î´óÖµÎª255
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ³õÊ¼»¯¶¨Ê±Æ÷
	TIM_TimeBaseInit(MSD_PULSE_TIM, &TIM_TimeBaseStructure);

	/*--------------------Êä³ö±È½Ï½á¹¹Ìå³õÊ¼»¯-------------------*/		
	//TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ÅäÖÃÎªPWMÄ£Ê½2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	// Êä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// »¥²¹Êä³ö½ûÄÜ
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; 
	// ÉèÖÃÕ¼¿Õ±È´óÐ¡
	TIM_OCInitStructure.TIM_Pulse = MSD_PULSE_TIM_PERIOD/2;
	// Êä³öÍ¨µÀµçÆ½¼«ÐÔÅäÖÃ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	// Êä³öÍ¨µÀ¿ÕÏÐµçÆ½¼«ÐÔÅäÖÃ
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    
	MSD_PULSE_OCx_Init(MSD_PULSE_TIM, &TIM_OCInitStructure);
    //Ê¹ÄÜTIM1_CH1Ô¤×°ÔØ¼Ä´æÆ÷
	MSD_PULSE_OCx_PreloadConfig(MSD_PULSE_TIM, TIM_OCPreload_Enable);
    //Ê¹ÄÜTIM1Ô¤×°ÔØ¼Ä´æÆ÷
    TIM_ARRPreloadConfig(MSD_PULSE_TIM, ENABLE); 
    
    //ÉèÖÃÖÐ¶ÏÔ´£¬Ö»ÓÐÒç³öÊ±²ÅÖÐ¶Ï
    TIM_UpdateRequestConfig(MSD_PULSE_TIM,TIM_UpdateSource_Regular);
	// Çå³ýÖÐ¶Ï±êÖ¾Î»
	TIM_ClearITPendingBit(MSD_PULSE_TIM, TIM_IT_Update);
    // Ê¹ÄÜÖÐ¶Ï
    TIM_ITConfig(MSD_PULSE_TIM, TIM_IT_Update, ENABLE);
	// Ê¹ÄÜ¼ÆÊýÆ÷
	TIM_Cmd(MSD_PULSE_TIM, DISABLE);

	// Ö÷Êä³öÊ¹ÄÜ£¬µ±Ê¹ÓÃµÄÊÇÍ¨ÓÃ¶¨Ê±Æ÷Ê±£¬Õâ¾ä²»ÐèÒª
    //TIM_CtrlPWMOutputs(MSD_PULSE_TIM, ENABLE);
}



void MSD_Init(void)
{
    MSD_GPIO_Config();
    
    TIM_NVIC_Config();

    TIM_Mode_Config();    
}


/**

  * @brief  ²úÉúÂö³å¶¨Ê±Æ÷µÄÖÐ¶ÏÏìÓ¦³ÌÐò£¬Ã¿×ßÒ»²½¶¼»á¼ÆËãÔË¶¯×´Ì¬

  * @param  ÎÞ

  * @retval ÎÞ

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
	//ÉèÖÃ¶¨Ê±Æ÷ÖØ×°Öµ	
    TIM_SetAutoreload(MSD_PULSE_TIM, Pulse_width);
    //ÉèÖÃÕ¼¿Õ±ÈÎª50%	
    TIM_SetCompare2(MSD_PULSE_TIM, Pulse_width>>1);
    //Ê¹ÄÜ¶¨Ê±Æ÷	      
    TIM_Cmd(MSD_PULSE_TIM, ENABLE);

		MSD_PULSE_TIM->CCER |= 1<<4; //Ê¹ÄÜÊä³ö
	// Ö÷Êä³öÊ¹ÄÜ£¬µ±Ê¹ÓÃµÄÊÇÍ¨ÓÃ¶¨Ê±Æ÷Ê±£¬Õâ¾ä²»ÐèÒª
    //TIM_CtrlPWMOutputs(MSD_PULSE_TIM, ENABLE);

}

void DisableMoveStep(void)
{
	//Ê¹ÄÜ¶¨Ê±Æ÷	      
    TIM_Cmd(MSD_PULSE_TIM, DISABLE);

	MSD_PULSE_TIM->CCER &= ~(1<<4); //½ûÖ¹Êä³ö
	//TIM_CtrlPWMOutputs(MSD_PULSE_TIM, DISABLE);
}



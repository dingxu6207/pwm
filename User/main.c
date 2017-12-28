
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_stepdriver.h"


int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();

	/* ����SysTick Ϊ10us�ж�һ�� */
	SysTick_Init();

	USART_Config();

	MSD_Init();

	while(1)
	{
		//����Ƿ���յ�ָ��
    	if (status.cmd == TRUE)
    	{    
                printf("%s\n", UART_RxBuffer);
				
				if (UART_RxBuffer[0] == ':')
				{
					if (UART_RxBuffer[1] == 'F')
					{
						if (UART_RxBuffer[2] == '?')
						{
							printf("it is ok!\n");
							printf("%d\n", stepPosition);
						    MoveStep();
						}

						if (UART_RxBuffer[2] == 'd')
						{
							DisableMoveStep();
							printf("%d\n", stepPosition);

						}
					}
				}
				
				status.cmd = FALSE;
			
			  uart_FlushRxBuffer();
			}
			
			
	}
}

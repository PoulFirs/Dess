#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"

#include "stm32f10x_spi.h"




void SetSysClockTo72(void)
{
    ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* ��������� RESET RCC (������ �� �����������, �� ������� �� ����� �������) */
	RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
    	 /* ��������� ��� ������� �������� ������������� ������ � FLASH.
    	    ���� �� �� ����������� ������������ � ����� ��������� ������� ������ � Flash,
    	    FLASH_PrefetchBufferCmd( ) �� FLASH_SetLatency( ) ����� ���������������� */
    	/* Enable Prefetch Buffer */
        //FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

    	/* FLASH Latency.
		������������� �������������:FLASH_Latency_0 - 0 < SYSCLK≤ 24 MHz
		FLASH_Latency_1 - 24 MHz < SYSCLK ≤ 48 MHz
		FLASH_Latency_2 - 48 MHz < SYSCLK ≤ 72 MHz
		FLASH_SetLatency( FLASH_Latency_2);*/

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */ /* ������ �� ����� APB1 Prescaler. ������� ������� �� 2 (RCC_HCLK_Div2)
		������ ��� �� ������ APB1 ������ ���� �� ����� 36��� (������ �����) */
	    RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        /* ��������� PLL �� ���� ����� ������� (RCC_PLLSource_HSE_Div1) � �� ������� �� �������� (RCC_PLLMul_9) */
        /* PLL ����� ����� ������� � ������ ��� ���� (RCC_PLLSource_HSE_Div1) ��� ���������� �� 2 (RCC_PLLSource_HSE_Div2). ������ ����� */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */

        /* Go to infinite loop */
        while (1)
        {
        }
    }
}

void ports_init(void) {
	GPIO_InitTypeDef port;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_StructInit(&port);

	/* Configure Pins (PA.5,6,7) as output for indicate */
	port.GPIO_Speed = GPIO_Speed_2MHz;
	port.GPIO_Mode = GPIO_Mode_Out_PP;
	port.GPIO_Pin = 0b111;
	GPIO_Init(GPIOA, &port);

	/* Configure Pin (PB.12) as CS for SPI */
	port.GPIO_Speed = GPIO_Speed_2MHz;
	port.GPIO_Mode = GPIO_Mode_Out_PP;
	port.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &port);

	/* Configure Pin (PB.6,7) as SCK and SDA for I2C */
	port.GPIO_Speed = GPIO_Speed_2MHz;
	port.GPIO_Mode = GPIO_Mode_AF_OD;
	port.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &port);

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &port);

	/* Configure USART1 Rx (PA.10) as input floating */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	port.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &port);


	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &port);

	/* Configure USART3 Rx (PB.11) as input floating */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;//����������� � ������� ���, ���� � ���� �������� TX (��� �� ����� 5�!!!)
	port.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &port);


//���������� �� PORTB_0
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	//���������� - ��� �������������� ������� �����
	//������� ���� ���������� ��� Alternate function I/O clock enable
	//� �������� RCC_APB2ENR
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE);
	//���� ������ �������� � �������� EXTICR[0] ����� ���������� ���
	// 0000 0000 0001 0000
	// �� ��������� ��� ����, ������� ��������� ������ 1 ���
	AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI0_PB;
	//���������� �� 13 ���� ���������
	EXTI->IMR|=(EXTI_IMR_MR0);
	//���������� �� ����� ����� �� ������������ ������
	EXTI->FTSR|=(EXTI_RTSR_TR0);
	//��������� ��� ����������
	NVIC_EnableIRQ(EXTI0_IRQn);


}



void usart1_init(void)
{
    /* Enable USART1 and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



    /* Configure the USART1 */
    USART_InitTypeDef USART_InitStructure;

    /* USART1 configuration ------------------------------------------------------*/
    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
            the SCLK pin
     */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);

    /* Enable the USART1 Receive interrupt: this interrupt is generated when the
        USART1 receive data register is not empty */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void usart2_init(void)
{
    /* Enable USART1 and GPIOA clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



    /* Configure the USART1 */
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);

    /* Enable USART1 */
    USART_Cmd(USART3, ENABLE);

    /* Enable the USART1 Receive interrupt: this interrupt is generated when the
        USART1 receive data register is not empty */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}


void timer_init(void) {

	TIM_TimeBaseInitTypeDef timer;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 7200;
    timer.TIM_Period = 50000;
	TIM_TimeBaseInit(TIM3, &timer);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM3, ENABLE);		//Enabling in main func

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
	    /* Enable the TIM3_IRQn Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void servo_init(void) {

	GPIO_InitTypeDef port;
	TIM_TimeBaseInitTypeDef timer;
	TIM_OCInitTypeDef timerPWM;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_AF_PP;
    port.GPIO_Pin = GPIO_Pin_9;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &port);

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 72;
    timer.TIM_Period = 1000000 / 50;
    timer.TIM_ClockDivision = 0;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &timer);

    TIM_OCStructInit(&timerPWM);
    timerPWM.TIM_Pulse = 1000;
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM4, &timerPWM);// ��� � ���� �����!!!

    TIM_Cmd(TIM4, ENABLE);


    //TIM4->CCR4 = RX_BUF[1]*20;//Turn
}




/*******************************************************************/
void I2C1_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_OwnAddress1 = 1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void SPI2_init(void)
{

	//�������� ������������ ����� B � �������������� �������
		RCC->APB2ENR  |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

	//13(SCK) � 15(MOSI) ����� - �������������� �������  push pull, 14(MISO) ����� - Input floating, 10(CS) ����� - �����, push-pull
		GPIOB->CRH &= ~(GPIO_CRH_CNF13_0 | GPIO_CRH_CNF15_0 | GPIO_CRH_CNF10_0);
		GPIOB->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF15_1;
		GPIOB->CRH |= GPIO_CRH_MODE10_0 | GPIO_CRH_MODE13_1 |	GPIO_CRH_MODE15_1;

	//�������� ������������ SPI2
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	        SPI2->CR1 |= SPI_CR1_BR;                //Baud rate = Fpclk/256
	        SPI2->CR1 &= ~SPI_CR1_CPOL;             //Polarity cls signal CPOL = 0;
	        SPI2->CR1 &= ~SPI_CR1_CPHA;             //Phase cls signal    CPHA = 0;
	        SPI2->CR1 |= SPI_CR1_DFF;               //16 bit data
	        SPI2->CR1 &= ~SPI_CR1_LSBFIRST;         //MSB will be first
	        SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;  //Software slave management & Internal slave select

	        SPI2->CR1 |= SPI_CR1_MSTR;              //Mode Master
	        SPI2->CR1 |= SPI_CR1_SPE;                //Enable SPI2



	SPI2->CR2 |= SPI_CR2_RXNEIE;       //��������� ����������, ���� ������ ���� ������
	NVIC_EnableIRQ (SPI2_IRQn);
}

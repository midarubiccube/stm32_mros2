#include "app.hpp"

#include "adc.h"
#include "fdcan.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "CANFD.hpp"

Application app;

int32_t get_encoder1( void )
{
	  uint16_t enc_buff = TIM2->CNT;
	  TIM2->CNT = 0;
	  if (enc_buff > 32767)
	  {
	    return (int16_t)enc_buff * -1;
	  }
	  else
	  {
	    return (int16_t)enc_buff;
	  }
}

int32_t get_encoder2( void )
{
	  uint16_t enc_buff = TIM5->CNT;
	  TIM5->CNT = 0;
	  if (enc_buff > 32767)
	  {
	    return (int16_t)enc_buff * -1;
	  }
	  else
	  {
	    return (int16_t)enc_buff;
	  }
}

int32_t get_encoder3( void )
{
	  uint16_t enc_buff = TIM4->CNT;
	  TIM4->CNT = 0;
	  if (enc_buff > 32767)
	  {
	    return (int16_t)enc_buff * -1;
	  }
	  else
	  {
	    return (int16_t)enc_buff;
	  }
}

int32_t get_encoder4( void )
{
	  uint16_t enc_buff = TIM8->CNT;
	  TIM8->CNT = 0;
	  if (enc_buff > 32767)
	  {
	    return (int16_t)enc_buff * -1;
	  }
	  else
	  {
	    return (int16_t)enc_buff;
	  }
}

int Application::Initilized()
{
	canfd = new CANFD(&hfdcan1);
	canfd->init();

	CANFD_Frame test;
	test.id=10;
	memset(test.data, 0, 64);
	test.size = 32;
	canfd->tx(test);

	pid1.set_limit(10, 900);
	pid1.set_gain(5,3,0.2);
	//pid1.set_gain(0.3,2,0.2);
	pid2.set_limit(10, 900);
	//pid2.set_gain(5,3,0.2);
	pid2.set_gain(0.3,2,0.2);

	pid3.set_limit(10, 900);
	 //pid1.set_gain(5,3,0.2);
	pid3.set_gain(0.3,2,0.2);
	pid4.set_limit(10, 900);
	//pid2.set_gain(5,3,0.2);
	pid4.set_gain(0.3,2,0.2);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);


	if (HAL_ADC_Init(&hadc3) != HAL_OK)
	{
		Error_Handler();
	}
	/* Run the ADC calibration in single-ended mode */
	/* Run the ADC calibration in single-ended mode */
	if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED) != HAL_OK)
	{
	 /* Calibration Error */
		Error_Handler();
	}

	/*##-3- Start the conversion process #######################################*/
	if (HAL_ADC_Start(&hadc3) != HAL_OK)
	{
		/* Start Conversation Error */
	     Error_Handler();
	}
	return 1;
}

int Application::loop()
{
	//__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 100);
	char usr_buf[256];
	  __IO uint32_t ad;
	while(1) {
		 int32_t encoder = get_encoder1();
		 int pwm = (int)pid1.calc(100.0, (float)encoder);
		 __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 100);

		 int32_t encoder2 = get_encoder2();
		 pwm = (int)pid2.calc(100.0, (float)encoder2);
		 __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm);

		 int32_t encoder3 = get_encoder3();
		 pwm = (int)pid3.calc(100.0, (float)encoder3);
		 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pwm);

		 int32_t encoder4 = get_encoder4();
		 pwm = (int)pid4.calc(100.0, (float)encoder4);
		 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);

		 sprintf(usr_buf, "%d %d %d %d\n\r", encoder, encoder2, encoder3, encoder4);
		 HAL_UART_Transmit(&huart1, (uint8_t *)usr_buf, strlen(usr_buf), 0xffff);
		 HAL_Delay(10);
	}
}



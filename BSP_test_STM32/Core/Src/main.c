#include "main.h"

#include "generated_bsp.h"

void SystemClock_Config(void);

/**
 * @brief Sends the current PWM duty cycle value via UART.
 * 
 * @param duty The PWM duty cycle value to send.
 */
static void sendDuty(uint16_t duty){
  BSP_UART_USED_TransmitMessage("\r\nPWM duty = ");
  char buffer[20];
  int buf_written_chars = 0;

  if(duty == 0)
	buffer[buf_written_chars++] = '0';
  else{
    char reverse_string[8];
    int  rev_written_chars = 0;
    while((duty > 0) && (rev_written_chars < (int)sizeof(reverse_string))){
      reverse_string[rev_written_chars++] = (char)('0' + (duty % 10));
      duty /= 10;
    }
    // Write reverse_sting in correct order in buffer
    while(rev_written_chars > 0)
      buffer[buf_written_chars++] = reverse_string[--rev_written_chars];
  }

  buffer[buf_written_chars++] = '\r';
  buffer[buf_written_chars++] = '\n';
  buffer[buf_written_chars] = '\0';

  BSP_UART_USED_TransmitMessage(buffer);
}

/**
 * @brief Main program entry point
 * 
 * @return int
 * 
 * This program initializes the BSP and PWM, then enters an infinite loop where it:
 * 1) Monitors a button press to toggle a red LED.
 * 2) Listens for '+' and '-' commands via UART to adjust the PWM duty cycle, controlling brightness.
 *    Feedback is provided via UART messages.
 */
int main(void){
  HAL_Init();
  SystemClock_Config();

  BSP_Init();
  BSP_PWM_SIGNAL_1_Start();

  BSP_UART_USED_TransmitMessage("\r\n\n\nStart program\r\n");
  BSP_UART_USED_TransmitMessage("Button controls RED LED\r\n");
  BSP_UART_USED_TransmitMessage("Send '+' to increase PWM brightness\r\n");
  BSP_UART_USED_TransmitMessage("Send '-' to decrease PWM brightness\r\n");

  const uint16_t step = 200; // 20% per keypress
  bool last_button, button = false;
  while(1){
    /* --- 1) Button pressed -> Toggle red LED --- */
	last_button = button;
    button = BSP_BUTTON_BOARD_IsActive();
    if(button && !last_button)
      BSP_LED_RED_Toggle();


    /* --- 2) UART +/- -> PWM duty --- */
    uint8_t ch;

    //If byte could not be read => continue
    if(!BSP_UART_USED_TryReceiveChar(&ch)){
      HAL_Delay(20);
      continue;
    }

    // Byte was successfully read

    // Echo received byte back for debugging
    BSP_UART_USED_TransmitChar(ch);

    if(ch == '+'){
      uint16_t duty = BSP_PWM_SIGNAL_1_GetDuty();
      duty = duty + step;
      BSP_PWM_SIGNAL_1_SetDuty(duty);
      duty = BSP_PWM_SIGNAL_1_GetDuty();

      sendDuty(duty);
    }
    else if(ch == '-'){
      uint16_t duty = BSP_PWM_SIGNAL_1_GetDuty();
      if(duty > step)
    	  duty = duty - step;
      else
    	  duty = 0;
      BSP_PWM_SIGNAL_1_SetDuty(duty);
      duty = BSP_PWM_SIGNAL_1_GetDuty();

      sendDuty(duty);
    }
    else if(ch == '\r' || ch == '\n' || ch == ' '){
      // Ignore whitespace
    } else
      BSP_UART_USED_TransmitMessage("\r\nUnknown command (use '+' or '-')\r\n");

    // Small delay to reduce CPU load
    HAL_Delay(20);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

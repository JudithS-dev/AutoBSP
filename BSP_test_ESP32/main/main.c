#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "generated_bsp.h"

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

void app_main(void){
  // Initialize BSP
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
      vTaskDelay(pdMS_TO_TICKS(20));
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
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

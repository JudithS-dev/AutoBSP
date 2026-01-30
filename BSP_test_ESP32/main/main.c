#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "generated_bsp.h"

void app_main(void){
  // Initialize BSP
  BSP_Init();
  
  bool last_button, button = false;
  
  uint16_t pwm_duty = BSP_PWM_SIGNAL_1_GetDuty();
  bool pwm_increasing = true;
  const uint16_t step = 50; // 5% per step
  BSP_PWM_SIGNAL_1_Start();

  while(1){
    /* --- 1) Button pressed -> Toggle red LED --- */
    last_button = button;
    button = BSP_BUTTON_BOARD_IsActive();
    if(button && !last_button)
      BSP_LED_RED_Toggle();
    
    /* --- 2) Adjust PWM duty cycle --- */
    if(pwm_increasing){
      if(pwm_duty + step <= 1000)
        pwm_duty += step;
      else{
        pwm_duty = 1000;
        pwm_increasing = false;
      }
    } else{
      if(pwm_duty >= step)
        pwm_duty -= step;
      else{
        pwm_duty = 0;
        pwm_increasing = true;
      }
    }
    BSP_PWM_SIGNAL_1_SetDuty(pwm_duty);
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

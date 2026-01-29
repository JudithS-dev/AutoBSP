#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "generated_bsp.h"

void app_main(void){
  // Initialize BSP
    BSP_Init();
  
  bool last_button, button = false;
  while(1){
    /* --- Button pressed -> Toggle red LED --- */
    last_button = button;
    button = BSP_BUTTON_BOARD_IsActive();
    if(button && !last_button)
      BSP_LED_RED_Toggle();
    
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

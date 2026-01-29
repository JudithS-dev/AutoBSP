#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "generated_bsp.h"

void app_main(void){
  // Initialize BSP
    BSP_Init();
  
  while(1){
    // Turn LED red ON, LED green OFF
    BSP_LED_RED_On();
    BSP_LED_GREEN_Off();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Toggle both LEDs
    BSP_LED_RED_Toggle();
    BSP_LED_GREEN_Toggle();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Explicit set using logical state
    BSP_LED_RED_Set(false);
    BSP_LED_GREEN_Set(true);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Query state
    if(BSP_LED_GREEN_IsOn())
        BSP_LED_RED_On();
    else
        BSP_LED_RED_Off();
    
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

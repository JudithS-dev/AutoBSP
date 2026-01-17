#ifndef __STM32F446RE_H__
#define __STM32F446RE_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct{
  char    port;      // 'A','B','C'
  uint8_t num;       // 0...15
} pin_key_t;

typedef struct{
  uint8_t tim;       // 1...14 (z.B. 3 für TIM3)
  uint8_t ch;        // 1...4
  uint8_t af;        // AF number (z.B. 2)
} pwm_opt_t;

#define MAX_PWM_OPT 4

typedef struct{
  pin_key_t pin;
  
  bool can_gpio_in;
  bool can_gpio_out;
  
  pwm_opt_t  pwm[MAX_PWM_OPT];
  uint8_t    pwm_count;
  
  uint8_t cost;      // 0=best, higher=try to avoid (e.g. HSE pin)
  bool not_usable;   // if pin is not usable at all (e.g. PB11)
} pin_cap_t;

static const pin_cap_t PINCAP_STM32F446RE[] = {
  /* PA0: // works
   * AF1: TIM2_CH1
   * AF2: TIM5_CH1
   */
  { .pin={'A',0},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=1, .af=1 },  // TIM2_CH1 @ AF1
      { .tim=5, .ch=1, .af=2 },  // TIM5_CH1 @ AF2
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PA1: // works
   * AF1: TIM2_CH2
   * AF2: TIM5_CH2
   */
  { .pin={'A',1},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=2, .af=1 },  // TIM2_CH2 @ AF1
      { .tim=5, .ch=2, .af=2 },  // TIM5_CH2 @ AF2
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PA2: // doesn't work (used for ST-Link)
   * AF1: TIM2_CH3
   * AF2: TIM5_CH3
   * AF3: TIM9_CH1
   */
  { .pin={'A',2},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=3, .af=1 },  // TIM2_CH3 @ AF1
      { .tim=5, .ch=3, .af=2 },  // TIM5_CH3 @ AF2
      { .tim=9, .ch=1, .af=3 },  // TIM9_CH1 @ AF3
    },
    .pwm_count = 3,
    .cost=0, .not_usable=true
  },
  
  /* PA3: // doesn't work (used for ST-Link)
   * AF1: TIM2_CH4
   * AF2: TIM5_CH4
   * AF3: TIM9_CH2
   */
  { .pin={'A',3},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=4, .af=1 },  // TIM2_CH4 @ AF1
      { .tim=5, .ch=4, .af=2 },  // TIM5_CH4 @ AF2
      { .tim=9, .ch=2, .af=3 },  // TIM9_CH2 @ AF3
    },
    .pwm_count = 3,
    .cost=0, .not_usable=true
  },
  
  /* PA4: // works
   * (no PWM)
   */
  { .pin={'A',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA5: // works
   * AF1: TIM2_CH1
   */
  { .pin={'A',5},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=1, .af=1 },  // TIM2_CH1 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA6: // works
   * AF2: TIM3_CH1
   * AF9: TIM13_CH1
   */
  { .pin={'A',6},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=1, .af=2 },   // TIM3_CH1 @ AF2
      { .tim=13, .ch=1, .af=9 },  // TIM13_CH1 @ AF9
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PA7: // works
   * AF2: TIM3_CH2
   * AF9: TIM14_CH1
   */
  { .pin={'A',7},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=2, .af=2 },   // TIM3_CH2 @ AF2
      { .tim=14, .ch=1, .af=9 },  // TIM14_CH1 @ AF9
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PA8: // works
   * AF1: TIM1_CH1
   */
  { .pin={'A',8},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=1, .af=1 },   // TIM1_CH1 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA9: // works
   * AF1: TIM1_CH2 
   */
  { .pin={'A',9},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=2, .af=1 },   // TIM1_CH2 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA10: // works
   * AF1: TIM1_CH3 
   */
  { .pin={'A',10},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=3, .af=1 },   // TIM1_CH3 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA11: // works
   * AF1: TIM1_CH4 
   */
  { .pin={'A',11},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=4, .af=1 },   // TIM1_CH4 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA12: // works
   * (no PWM)
   */
  { .pin={'A',12},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA13: // works
   * (no PWM) 
   */
  { .pin={'A',13},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA14: // works
   * (no PWM) 
   */
  { .pin={'A',14},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA15: // works
   * AF1: TIM2_CH1 
   */
  { .pin={'A',15},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=1, .af=1 },   // TIM2_CH1 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB0: // works
   * AF2: TIM3_CH3
   */
  { .pin={'B',0},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=3, .af=2 },   // TIM3_CH3 @ AF2
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB1: // works
   * AF2: TIM3_CH4
   */
  { .pin={'B',1},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=4, .af=2 },   // TIM3_CH4 @ AF2
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB2: // works
   * AF1: TIM2_CH4
   */
  { .pin={'B',2},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=4, .af=1 },   // TIM2_CH4 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB3: // works
   * AF1: TIM2_CH2
   */
  { .pin={'B',3},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=2, .af=1 },   // TIM2_CH2 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB4: // works
   * AF2: TIM3_CH1
   */
  { .pin={'B',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=1, .af=2 },   // TIM3_CH1 @ AF2
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB5: // works
   * AF2: TIM3_CH2
   */
  { .pin={'B',5},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=2, .af=2 },   // TIM3_CH2 @ AF2
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB6: // works
   * AF2: TIM4_CH1
   */
  { .pin={'B',6},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=4, .ch=1, .af=2 },   // TIM4_CH1 @ AF2
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB7: // works
   * AF2: TIM4_CH2
   */
  { .pin={'B',7},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=4, .ch=2, .af=2 },   // TIM4_CH2 @ AF2
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB8: // works
   * AF1: TIM2_CH1
   * AF2: TIM4_CH3
   * AF3: TIM10_CH1
   */
  { .pin={'B',8},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=1, .af=1 },   // TIM2_CH1 @ AF1
      { .tim=4,  .ch=3, .af=2 },   // TIM4_CH3 @ AF2
      { .tim=10, .ch=1, .af=3 },   // TIM10_CH1 @ AF3
    },
    .pwm_count = 3,
    .cost=0, .not_usable=false
  },
  
  /* PB9: // works
   * AF1: TIM2_CH2
   * AF2: TIM4_CH4
   * AF3: TIM11_CH1
   */
  { .pin={'B',9},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=2, .af=1 },   // TIM2_CH2 @ AF1
      { .tim=4,  .ch=4, .af=2 },   // TIM4_CH4 @ AF2
      { .tim=11, .ch=1, .af=3 },   // TIM11_CH1 @ AF3
    },
    .pwm_count = 3,
    .cost=0, .not_usable=false
  },
  
  /* PB10: // works
   * AF1: TIM2_CH3
   */
  { .pin={'B',10},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=3, .af=1 },   // TIM2_CH3 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB11: // doesn't work (does not exist on STM32F446RE)
   * AF1: TIM2_CH4
   */
  { .pin={'B',11},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=4, .af=1 },   // TIM2_CH4 @ AF1
    },
    .pwm_count = 1,
    .cost=0, .not_usable=true
  },
  
  /* PB12: // works
   * (no PWM)
   */
  { .pin={'B',12},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB13: // works
   * (no PWM)
   */
  { .pin={'B',13},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB14: // works
   * AF9: TIM12_CH1
   */
  { .pin={'B',14},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=12,  .ch=1, .af=9 },   // TIM12_CH1 @ AF9
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB15: // works
   * AF9: TIM12_CH2
   */
  { .pin={'B',15},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=12,  .ch=2, .af=9 },   // TIM12_CH2 @ AF9
    },
    .pwm_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PC0: // works
   * (no PWM)
   */
  { .pin={'C',0},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC1: // works
   * (no PWM)
   */
  { .pin={'C',1},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC2: // works
   * (no PWM)
   */
  { .pin={'C',2},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC3: // works
   * (no PWM)
   */
  { .pin={'C',3},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC4: // works
   * (no PWM)
   */
  { .pin={'C',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC5: // works
   * (no PWM)
   */
  { .pin={'C',5},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC6: // works
   * AF2: TIM3_CH1
   * AF3: TIM8_CH1
   */
  { .pin={'C',6},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=1, .af=2 },   // TIM3_CH1 @ AF2
      { .tim=8, .ch=1, .af=3 },   // TIM8_CH1 @ AF3
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PC7: // works
   * AF2: TIM3_CH2
   * AF3: TIM8_CH2
   */
  { .pin={'C',7},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=2, .af=2 },   // TIM3_CH2 @ AF2
      { .tim=8, .ch=2, .af=3 },   // TIM8_CH2 @ AF3
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PC8: // works
   * AF2: TIM3_CH3
   * AF3: TIM8_CH3
   */
  { .pin={'C',8},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=3, .af=2 },   // TIM3_CH3 @ AF2
      { .tim=8, .ch=3, .af=3 },   // TIM8_CH3 @ AF3
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PC9: // works
   * AF2: TIM3_CH4
   * AF3: TIM8_CH4
   */
  { .pin={'C',9},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=4, .af=2 },   // TIM3_CH4 @ AF2
      { .tim=8, .ch=4, .af=3 },   // TIM8_CH4 @ AF3
    },
    .pwm_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PC10: // works
   * (no PWM)
   */
  { .pin={'C',10},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC11: // works
   * (no PWM)
   */
  { .pin={'C',11},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC12: // works
   * (no PWM)
   */
  { .pin={'C',12},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC13: // works
   * (no PWM)
   */
  { .pin={'C',13},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC14: // doesn't work (LSE crystal pin)
   * (no PWM)
   */
  { .pin={'C',14},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=true
  },
  
  /* PC15: // doesn't work (LSE crystal pin)
   * (no PWM)
   */
  { .pin={'C',15},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=true
  }
};

static const uint16_t PINCAP_STM32F446RE_LEN = sizeof(PINCAP_STM32F446RE) / sizeof(PINCAP_STM32F446RE[0]);

#endif // __STM32F446RE_H__
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
  /* PA0:
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
  
  /* PA1:
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
  
  /* PA2:
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
    .cost=0, .not_usable=false
  },
  
  /* PA3:
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
    .cost=0, .not_usable=false
  },
  
  /* PA4:
   */
  { .pin={'A',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA5:
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
  
  /* PA6:
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
  }

  // TODO: complete for all pins
};

static const uint16_t PINCAP_STM32F446RE_LEN = sizeof(PINCAP_STM32F446RE) / sizeof(PINCAP_STM32F446RE[0]);

#endif // __STM32F446RE_H__
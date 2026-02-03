#ifndef __STM32F446RE_H__
#define __STM32F446RE_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Data structure representing a STM32 microcontroller pin.
 */
typedef struct{
  char    port;      // 'A','B','C'
  uint8_t num;       // 0...15
} pin_key_t;

/**
 * @brief Data structure representing PWM capabilities of a pin.
 */
typedef struct{
  uint8_t tim;       // 1...14 (z.B. 3 für TIM3)
  uint8_t ch;        // 1...4
  uint8_t af;        // AF number (z.B. 2)
} pwm_opt_t;

/**
 * @brief Maximum number of PWM options per pin.
 */
#define MAX_PWM_OPT 4

/**
 * @brief Data structure representing UART capabilities of a pin.
 */
typedef struct{
  uint8_t usart;     // 1...5
  bool is_uart;      // true=UART4/5, false=USART1/2/3/6
  bool is_tx;        // true=TX pin, false=RX pin
  uint8_t af;        // AF number (e.g., 7)
} uart_opt_t;

/**
 * @brief Maximum number of UART options per pin.
 */
#define MAX_UART_OPT 2

/**
 * @brief Data structure representing the capabilities of a pin on the STM32F446RE.
 */
typedef struct{
  pin_key_t pin;
  
  bool can_gpio_in;
  bool can_gpio_out;
  
  pwm_opt_t  pwm[MAX_PWM_OPT];
  uint8_t    pwm_count;
  
  uart_opt_t uart[MAX_UART_OPT];
  uint8_t    uart_count;
  
  uint8_t cost;      // 0=best, higher=try to avoid (e.g. HSE pin)
  bool not_usable;   // If pin is not usable at all (e.g. PB11)
} pin_cap_t;

/**
 * @brief Array of pin capabilities for the STM32F446RE microcontroller.
 * 
 * Each entry defines the capabilities of a specific pin, including GPIO, PWM, and UART functionalities.
 */
static const pin_cap_t PINCAP_STM32F446RE[] = {
  /* PA0: // input works, output works, pwm works
   * AF1: TIM2_CH1
   * AF2: TIM5_CH1
   * AF8: UART4_TX
   */
  { .pin={'A',0},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=1, .af=1 },  // TIM2_CH1 @ AF1
      { .tim=5, .ch=1, .af=2 },  // TIM5_CH1 @ AF2
    },
    .pwm_count = 2,
    .uart = {
      { .usart=4, .is_uart=true, .is_tx=true, .af=8 }, // UART4_TX @ AF8
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA1: // input works, output works, pwm works
   * AF1: TIM2_CH2
   * AF2: TIM5_CH2
   * AF8: UART4_RX
   */
  { .pin={'A',1},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=2, .af=1 },  // TIM2_CH2 @ AF1
      { .tim=5, .ch=2, .af=2 },  // TIM5_CH2 @ AF2
    },
    .pwm_count = 2,
    .uart = {
      { .usart=4, .is_uart=true, .is_tx=false, .af=8 }, // UART4_RX @ AF8
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA2: // doesn't work (used for ST-Link)
   * AF1: TIM2_CH3
   * AF2: TIM5_CH3
   * AF3: TIM9_CH1
   * AF7: USART2_TX
   */
  { .pin={'A',2},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=3, .af=1 },  // TIM2_CH3 @ AF1
      { .tim=5, .ch=3, .af=2 },  // TIM5_CH3 @ AF2
      { .tim=9, .ch=1, .af=3 },  // TIM9_CH1 @ AF3
    },
    .pwm_count = 3,
    .uart = {
      { .usart=2, .is_uart=false, .is_tx=true, .af=7 }, // USART2_TX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=true
  },
  
  /* PA3: // doesn't work (used for ST-Link)
   * AF1: TIM2_CH4
   * AF2: TIM5_CH4
   * AF3: TIM9_CH2
   * AF7: USART2_RX
   */
  { .pin={'A',3},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=4, .af=1 },  // TIM2_CH4 @ AF1
      { .tim=5, .ch=4, .af=2 },  // TIM5_CH4 @ AF2
      { .tim=9, .ch=2, .af=3 },  // TIM9_CH2 @ AF3
    },
    .pwm_count = 3,
    .uart = {
      { .usart=2, .is_uart=false, .is_tx=false, .af=7 }, // USART2_RX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=true
  },
  
  /* PA4: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'A',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA5: // input works, output works, pwm works
   * AF1: TIM2_CH1
   * (no UART)
   */
  { .pin={'A',5},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=1, .af=1 },  // TIM2_CH1 @ AF1
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA6: // input works, output works, pwm works
   * AF2: TIM3_CH1
   * AF9: TIM13_CH1
   * (no UART)
   */
  { .pin={'A',6},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=1, .af=2 },   // TIM3_CH1 @ AF2
      { .tim=13, .ch=1, .af=9 },  // TIM13_CH1 @ AF9
    },
    .pwm_count = 2,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA7: // input works, output works, pwm works
   * AF2: TIM3_CH2
   * AF9: TIM14_CH1
   * (no UART)
   */
  { .pin={'A',7},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=2, .af=2 },   // TIM3_CH2 @ AF2
      { .tim=14, .ch=1, .af=9 },  // TIM14_CH1 @ AF9
    },
    .pwm_count = 2,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA8: // input works, output works, pwm works
   * AF1: TIM1_CH1
   * (no UART)
   */
  { .pin={'A',8},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=1, .af=1 },   // TIM1_CH1 @ AF1
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA9: // input works, output works, pwm works
   * AF1: TIM1_CH2
   * AF7: USART1_TX
   */
  { .pin={'A',9},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=2, .af=1 },   // TIM1_CH2 @ AF1
    },
    .pwm_count = 1,
    .uart = {
      { .usart=1, .is_uart=false, .is_tx=true, .af=7 }, // USART1_TX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA10: // input works, output works, pwm works
   * AF1: TIM1_CH3
   * AF7: USART1_RX
   */
  { .pin={'A',10},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=3, .af=1 },   // TIM1_CH3 @ AF1
    },
    .pwm_count = 1,
    .uart = {
      { .usart=1, .is_uart=false, .is_tx=false, .af=7 }, // USART1_RX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PA11: // input works, output works, pwm works
   * AF1: TIM1_CH4
   * (no UART)
   */
  { .pin={'A',11},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=1, .ch=4, .af=1 },   // TIM1_CH4 @ AF1
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA12: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'A',12},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PA13: // doesn't work (SWDIO)
   * (no PWM)
   * (no UART)
   */
  { .pin={'A',13},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=true
  },
  
  /* PA14: // doesn't work (SWCLK)
   * (no PWM)
   * (no UART)
   */
  { .pin={'A',14},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=true
  },
  
  /* PA15: // input works, output works, pwm works
   * AF1: TIM2_CH1
   * (no UART)
   */
  { .pin={'A',15},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=1, .af=1 },   // TIM2_CH1 @ AF1
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB0: // input works, output works, pwm works
   * AF2: TIM3_CH3
   * (no UART)
   */
  { .pin={'B',0},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=3, .af=2 },   // TIM3_CH3 @ AF2
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB1: // input works, output works, pwm works
   * AF2: TIM3_CH4
   * (no UART)
   */
  { .pin={'B',1},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=4, .af=2 },   // TIM3_CH4 @ AF2
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB2: // input works, output works, pwm works
   * AF1: TIM2_CH4
   * (no UART)
   */
  { .pin={'B',2},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=4, .af=1 },   // TIM2_CH4 @ AF1
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB3: // input works, output works, pwm works
   * AF1: TIM2_CH2
   * (no UART)
   */
  { .pin={'B',3},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2, .ch=2, .af=1 },   // TIM2_CH2 @ AF1
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB4: // input works, output works, pwm works
   * AF2: TIM3_CH1
   * (no UART)
   */
  { .pin={'B',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=1, .af=2 },   // TIM3_CH1 @ AF2
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB5: // input works, output works, pwm works
   * AF2: TIM3_CH2
   * (no UART)
   */
  { .pin={'B',5},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=2, .af=2 },   // TIM3_CH2 @ AF2
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB6: // input works, output works, pwm works
   * AF2: TIM4_CH1
   * AF7: USART1_TX
   */
  { .pin={'B',6},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=4, .ch=1, .af=2 },   // TIM4_CH1 @ AF2
    },
    .pwm_count = 1,
    .uart = {
      { .usart=1, .is_uart=false, .is_tx=true, .af=7 }, // USART1_TX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB7: // input works, output works, pwm works
   * AF2: TIM4_CH2
   * AF7: USART1_RX
   */
  { .pin={'B',7},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=4, .ch=2, .af=2 },   // TIM4_CH2 @ AF2
    },
    .pwm_count = 1,
    .uart = {
      { .usart=1, .is_uart=false, .is_tx=false, .af=7 }, // USART1_RX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB8: // input works, output works, pwm works
   * AF1: TIM2_CH1
   * AF2: TIM4_CH3
   * AF3: TIM10_CH1
   * (no UART)
   */
  { .pin={'B',8},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=1, .af=1 },   // TIM2_CH1 @ AF1
      { .tim=4,  .ch=3, .af=2 },   // TIM4_CH3 @ AF2
      { .tim=10, .ch=1, .af=3 },   // TIM10_CH1 @ AF3
    },
    .pwm_count = 3,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB9: // input works, output works, pwm works
   * AF1: TIM2_CH2
   * AF2: TIM4_CH4
   * AF3: TIM11_CH1
   * (no UART)
   */
  { .pin={'B',9},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=2, .af=1 },   // TIM2_CH2 @ AF1
      { .tim=4,  .ch=4, .af=2 },   // TIM4_CH4 @ AF2
      { .tim=11, .ch=1, .af=3 },   // TIM11_CH1 @ AF3
    },
    .pwm_count = 3,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB10: // input works, output works, pwm works
   * AF1: TIM2_CH3
   * AF7: USART3_TX
   */
  { .pin={'B',10},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=3, .af=1 },   // TIM2_CH3 @ AF1
    },
    .pwm_count = 1,
    .uart = {
      { .usart=3, .is_uart=false, .is_tx=true, .af=7 }, // USART3_TX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PB11: // doesn't work (does not exist on STM32F446RE)
   * AF1: TIM2_CH4
   * AF7: USART3_RX
   */
  { .pin={'B',11},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=2,  .ch=4, .af=1 },   // TIM2_CH4 @ AF1
    },
    .pwm_count = 1,
    .uart = {
      { .usart=3, .is_uart=false, .is_tx=false, .af=7 }, // USART3_RX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=true
  },
  
  /* PB12: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'B',12},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB13: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'B',13},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB14: // input works, output works, pwm works
   * AF9: TIM12_CH1
   * (no UART)
   */
  { .pin={'B',14},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=12,  .ch=1, .af=9 },   // TIM12_CH1 @ AF9
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PB15: // input works, output works, pwm works
   * AF9: TIM12_CH2
   * (no UART)
   */
  { .pin={'B',15},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=12,  .ch=2, .af=9 },   // TIM12_CH2 @ AF9
    },
    .pwm_count = 1,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC0: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',0},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC1: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',1},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC2: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',2},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC3: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',3},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC4: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',4},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC5: // input works, output works
   * (no PWM)
   * AF7: USART3_RX
   */
  { .pin={'C',5},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = {
      { .usart=3, .is_uart=false, .is_tx=false, .af=7 }, // USART3_RX @ AF7
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PC6: // input works, output works, pwm works
   * AF2: TIM3_CH1
   * AF3: TIM8_CH1
   * AF8: UART6_TX
   */
  { .pin={'C',6},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=1, .af=2 },   // TIM3_CH1 @ AF2
      { .tim=8, .ch=1, .af=3 },   // TIM8_CH1 @ AF3
    },
    .pwm_count = 2,
    .uart = {
      { .usart=6, .is_uart=true, .is_tx=true, .af=8 }, // UART6_TX @ AF8
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PC7: // input works, output works, pwm works
   * AF2: TIM3_CH2
   * AF3: TIM8_CH2
   * AF8: UART6_RX
   */
  { .pin={'C',7},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=2, .af=2 },   // TIM3_CH2 @ AF2
      { .tim=8, .ch=2, .af=3 },   // TIM8_CH2 @ AF3
    },
    .pwm_count = 2,
    .uart = {
      { .usart=6, .is_uart=true, .is_tx=false, .af=8 }, // UART6_RX @ AF8
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PC8: // input works, output works, pwm works
   * AF2: TIM3_CH3
   * AF3: TIM8_CH3
   * (no UART)
   */
  { .pin={'C',8},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=3, .af=2 },   // TIM3_CH3 @ AF2
      { .tim=8, .ch=3, .af=3 },   // TIM8_CH3 @ AF3
    },
    .pwm_count = 2,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC9: // input works, output works, pwm works
   * AF2: TIM3_CH4
   * AF3: TIM8_CH4
   * (no UART)
   */
  { .pin={'C',9},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = {
      { .tim=3, .ch=4, .af=2 },   // TIM3_CH4 @ AF2
      { .tim=8, .ch=4, .af=3 },   // TIM8_CH4 @ AF3
    },
    .pwm_count = 2,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC10: // input works, output works
   * (no PWM)
   * AF7: USART3_TX
   * AF8: UART4_TX
   */
  { .pin={'C',10},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = {
      { .usart=3, .is_uart=false, .is_tx=true, .af=7 }, // USART3_TX @ AF7
      { .usart=4, .is_uart=true,  .is_tx=true, .af=8 },  // UART4_TX @ AF8
    },
    .uart_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PC11: // input works, output works
   * (no PWM)
   * AF7: USART3_RX
   * AF8: UART4_RX
   */
  { .pin={'C',11},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = {
      { .usart=3, .is_uart=false, .is_tx=false, .af=7 }, // USART3_RX @ AF7
      { .usart=4, .is_uart=true,  .is_tx=false, .af=8 },  // UART4_RX @ AF8
    },
    .uart_count = 2,
    .cost=0, .not_usable=false
  },
  
  /* PC12: // input works, output works
   * (no PWM)
   * AF8: UART5_TX
   */
  { .pin={'C',12},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = {
      { .usart=5, .is_uart=true,  .is_tx=true, .af=8 },  // UART5_TX @ AF8
    },
    .uart_count = 1,
    .cost=0, .not_usable=false
  },
  
  /* PC13: // input works, output works
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',13},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=false
  },
  
  /* PC14: // doesn't work (LSE crystal pin)
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',14},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=true
  },
  
  /* PC15: // doesn't work (LSE crystal pin)
   * (no PWM)
   * (no UART)
   */
  { .pin={'C',15},
    .can_gpio_in=true, .can_gpio_out=true,
    .pwm = { },
    .pwm_count = 0,
    .uart = { },
    .uart_count = 0,
    .cost=0, .not_usable=true
  }
};

/**
 * @brief Length of the PINCAP_STM32F446RE array.
 */
static const uint16_t PINCAP_STM32F446RE_LEN = sizeof(PINCAP_STM32F446RE) / sizeof(PINCAP_STM32F446RE[0]);

#endif // __STM32F446RE_H__
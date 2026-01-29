#ifndef __MODULE_ENUMS_H__
#define __MODULE_ENUMS_H__

#include <stdbool.h>


/* -------- Supported microcontrollers -------- */

/**
 * @brief Supported microcontroller types
 * 
 * Values: STM32F446RE, ESP32
 */
typedef enum{
  STM32F446RE,
  ESP32
} controller_t;


/* ---------- General used data types --------- */

/**
 * @brief Pin identifier types to distinguish between different pin naming conventions
 * 
 * Values: GPIOn, PXn
 * 
 * X represents the port letter (A, B, C, etc.)
 * n represents the pin number (0, 1, 2, etc.)
 */
typedef enum{
  GPIOn,
  PXn
} pin_identifier_t;

/**
 * @brief Structure representing a microcontroller pin.
 * 
 * Consists of a identifier type, port character, and pin number.
 */
typedef struct{
  pin_identifier_t identifier;
  char port;
  unsigned int pin_number;
} pin_t;


/* ------ Multiple used parameter values ------ */
/**
 * @brief Logical levels
 * 
 * Values: LOW, HIGH
 */
typedef enum{
  LOW,
  HIGH
} level_t;


/* ------ GPIO specific parameter values ------ */

/**
 * @brief GPIO output types
 * 
 * Values: GPIO_TYPE_PUSHPULL, GPIO_TYPE_OPENDRAIN
 */
typedef enum{
  GPIO_TYPE_PUSHPULL,
  GPIO_TYPE_OPENDRAIN
} gpio_type_t;

/**
 * @brief GPIO pull-up/pull-down configurations
 * 
 * Values: GPIO_PULL_UP, GPIO_PULL_DOWN, GPIO_PULL_NONE
 */
typedef enum{
  GPIO_PULL_UP,
  GPIO_PULL_DOWN,
  GPIO_PULL_NONE
} gpio_pull_t;

/**
 * @brief GPIO speed configurations
 * 
 * Values: GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH, GPIO_SPEED_VERY_HIGH
 */
typedef enum{
  GPIO_SPEED_LOW,
  GPIO_SPEED_MEDIUM,
  GPIO_SPEED_HIGH,
  GPIO_SPEED_VERY_HIGH
} gpio_speed_t;

/**
 * @brief GPIO initialization states
 * 
 * Values: GPIO_INIT_ON, GPIO_INIT_OFF, GPIO_INIT_NONE
 */
typedef enum{
  GPIO_INIT_ON,
  GPIO_INIT_OFF,
  GPIO_INIT_NONE
} gpio_init_t;


/* ------ UART specific parameter values ------ */
/**
 * @brief UART parity configurations
 * 
 * Values: UART_PARITY_NONE, UART_PARITY_EVEN, UART_PARITY_ODD
 */
typedef enum{
  UART_PARITY_NONE,
  UART_PARITY_EVEN,
  UART_PARITY_ODD
} uart_parity_t;

#endif //__MODULE_ENUMS_H__
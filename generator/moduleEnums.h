#ifndef __MODULE_ENUMS_H__
#define __MODULE_ENUMS_H__

#include <stdbool.h>

/* -------- Supported microcontrollers -------- */

/**
 * @brief Supported microcontroller types
 * 
 * Values: STM32F446RE
 */
typedef enum{
  STM32F446RE
} controller_t;

/* ---------- General used data types --------- */

/**
 * @brief Structure representing a microcontroller pin.
 * 
 * Consists of a port (character) and a pin number (unsigned integer).
 */
typedef struct{
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

#endif //__MODULE_ENUMS_H__
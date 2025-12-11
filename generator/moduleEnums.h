#ifndef __MODULE_ENUMS_H__
#define __MODULE_ENUMS_H__

/* Supported microcontrollers */
typedef enum{
  STM32F446RE
} controller_t;

/* General used data types */
typedef struct{
  char port;
  unsigned int pin_number;
} pin_t;

/* Multiple used parameter values */
typedef enum{
    BOOL_FALSE = 0,
    BOOL_TRUE  = 1
} bool_t;

typedef enum{
  LOW,
  HIGH
} level_t;

/* GPIO specific parameter values */
typedef enum{
  GPIO_TYPE_PUSHPULL,
  GPIO_TYPE_OPENDRAIN
} gpio_type_t;

typedef enum{
  GPIO_PULL_UP,
  GPIO_PULL_DOWN,
  GPIO_PULL_NONE
} gpio_pull_t;

typedef enum{
  GPIO_SPEED_LOW,
  GPIO_SPEED_MEDIUM,
  GPIO_SPEED_HIGH,
  GPIO_SPEED_VERY_HIGH
} gpio_speed_t;

typedef enum{
  GPIO_INIT_ON,
  GPIO_INIT_OFF,
  GPIO_INIT_NONE
} gpio_init_t;

#endif //__MODULE_ENUMS_H__
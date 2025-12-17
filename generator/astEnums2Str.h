#ifndef __AST_ENUMS_2_STR_H__
#define __AST_ENUMS_2_STR_H__

#include "astEnums.h"

char* pin_to_string(pin_t pin);
const char* bool_to_string(bool value);
const char* controller_to_string(controller_t controller);
const char* level_to_string(level_t level);
const char* gpio_type_to_string(gpio_type_t type);
const char* gpio_pull_to_string(gpio_pull_t pull);
const char* gpio_speed_to_string(gpio_speed_t speed);
const char* gpio_init_to_string(gpio_init_t init);
const char* kind_to_string(module_kind_t kind);

#endif //__AST_ENUMS_2_STR_H__
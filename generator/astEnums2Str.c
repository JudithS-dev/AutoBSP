#include "astEnums2Str.h"

#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* -------- Module kind conversions -------- */
/**
 * @brief Converts module kind enum to string.
 * 
 * @param kind Module kind enum value.
 * @return Corresponding string representation.
 */
const char* kind_to_string(ast_module_kind_t kind){
  switch(kind){
    case MODULE_OUTPUT:     return "OUTPUT";
    case MODULE_INPUT:      return "INPUT";
    case MODULE_PWM_OUTPUT: return "PWM_OUTPUT";
    case MODULE_UART:       return "UART";
    default:  log_error("kind_to_string", 0, "Unknown module kind enum value '%d'", kind); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}


/* -------- General parameter conversions -------- */
/**
 * @brief Converts controller enum to string.
 * 
 * @param controller Controller enum value.
 * @return Corresponding string representation.
 */
const char* controller_to_string(controller_t controller){
  switch(controller){
    case STM32F446RE: return "STM32F446RE";
    default:  log_error("controller_to_string", 0, "Unknown controller enum value '%d'", controller); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts pin structure to string.
 * 
 * @param pin Pin structure.
 * @return String representation of the pin.
 * 
 * @note The returned string is dynamically allocated and should be freed by the caller.
 */
char* pin_to_string(pin_t pin){
  char buffer[20];
  char* result;
  switch(pin.identifier){
    case GPIOn: snprintf(buffer, sizeof(buffer), "GPIO%d", pin.pin_number);
                result = strdup(buffer);
                if(result == NULL)
                  log_error("pin_to_string", 0, "Memory allocation failed for pin string.");
                return result;
    case PXn:   snprintf(buffer, sizeof(buffer), "P%c%d", pin.port, pin.pin_number);
                result = strdup(buffer);
                if(result == NULL)
                  log_error("pin_to_string", 0, "Memory allocation failed for pin string.");
                return result;
    default:  log_error("pin_to_string", 0, "Unknown pin identifier enum value '%d'", pin.identifier);
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts boolean value to string.
 * 
 * @param value Boolean value.
 * @return Corresponding string representation ("true" or "false").
 */
const char* bool_to_string(bool value){
  if(value)
    return "true";
  else
    return "false";
}

/**
 * @brief Converts level enum to string.
 * 
 * @param level Level enum value.
 * @return Corresponding string representation.
 */
const char* level_to_string(level_t level){
  switch(level){
    case LOW:   return "LOW";
    case HIGH:  return "HIGH";
    default:  log_error("level_to_string", 0, "Unknown level enum value '%d'", level); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}


/* -------- GPIO specific conversions -------- */
/**
 * @brief Converts GPIO type enum to string.
 * 
 * @param type GPIO type enum value.
 * @return Corresponding string representation.
 */
const char* gpio_type_to_string(gpio_type_t type){
  switch(type){
    case GPIO_TYPE_PUSHPULL:    return "Push-Pull";
    case GPIO_TYPE_OPENDRAIN:   return "Open-Drain";
    default:  log_error("gpio_type_to_string", 0, "Unknown GPIO type enum value '%d'", type); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO pull enum to string.
 * 
 * @param pull GPIO pull enum value.
 * @return Corresponding string representation.
 */
const char* gpio_pull_to_string(gpio_pull_t pull){
  switch(pull){
    case GPIO_PULL_UP:      return "Pull-Up";
    case GPIO_PULL_DOWN:    return "Pull-Down";
    case GPIO_PULL_NONE:    return "No Pull";
    default:  log_error("gpio_pull_to_string", 0, "Unknown GPIO pull enum value '%d'", pull); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO speed enum to string.
 * 
 * @param speed GPIO speed enum value.
 * @return Corresponding string representation.
 */
const char* gpio_speed_to_string(gpio_speed_t speed){
  switch(speed){
    case GPIO_SPEED_LOW:        return "Low";
    case GPIO_SPEED_MEDIUM:     return "Medium";
    case GPIO_SPEED_HIGH:       return "High";
    case GPIO_SPEED_VERY_HIGH:  return "Very High";
    default:  log_error("gpio_speed_to_string", 0, "Unknown GPIO speed enum value '%d'", speed); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}

/**
 * @brief Converts GPIO init enum to string.
 * 
 * @param init GPIO init enum value.
 * @return Corresponding string representation.
 */
const char* gpio_init_to_string(gpio_init_t init){
  switch(init){
    case GPIO_INIT_ON:     return "ON";
    case GPIO_INIT_OFF:    return "OFF";
    case GPIO_INIT_NONE:   return "No Init";
    default:  log_error("gpio_init_to_string", 0, "Unknown GPIO init enum value '%d'", init); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}


/* -------- UART specific conversions -------- */
/**
 * @brief Converts UART parity enum to string.
 * 
 * @param parity UART parity enum value.
 * @return Corresponding string representation.
 */
const char* uart_parity_to_string(uart_parity_t parity){
  switch(parity){
    case UART_PARITY_NONE:   return "None";
    case UART_PARITY_EVEN:   return "Even";
    case UART_PARITY_ODD:    return "Odd";
    default:  log_error("uart_parity_to_string", 0, "Unknown UART parity enum value '%d'", parity); 
              return "UNKNOWN"; // This won't be reached due to log_error exiting
  }
}
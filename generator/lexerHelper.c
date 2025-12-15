#include "lexerHelper.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "logging.h"

/** 
 * @brief Extracts the name from a raw name string by removing surrounding quotes.
 * 
 * @param raw_name Raw name string with surrounding quotes.
 * @return char* Extracted name string without quotes. The caller is responsible for freeing the allocated memory.
 * 
 * @note Logs an error and exits if the raw_name is NULL or too short to extract a name.
*/
char* extract_name(const char* raw_name){
  if(raw_name == NULL)
    log_error("extract_name", 0, "Raw name is NULL.");
  
  size_t len = strlen(raw_name);
  if(len < 2)
    log_error("extract_name", 0, "Raw name '%s' is too short to extract name.", raw_name);
  
  // Allocate memory for the new string (excluding quotes)
  char* name = (char*)malloc(len - 1); // len - 2 for quotes + 1 for null terminator
  if(name == NULL)
    log_error("extract_name", 0, "Memory allocation failed for name extraction.");
  
  // Copy characters excluding the first and last quote
  strncpy(name, raw_name + 1, len - 2);
  name[len - 2] = '\0'; // Null-terminate the new string
  
  return name;
}

/**
 * @brief Get the port character from a pin text
 * 
 * @param pin_text pin text in format "PXn" or "GPIOn"
 * @return char port character (e.g. 'A' for "PA5"), or '\0' if no port
 * 
 * @note expects pin text in format "PXn" (e.g. "PA5") or "GPIOn" (e.g. "GPIO5"), if other format is given it could result in incorrect behavior
 */
char get_port(const char* pin_text){
  if(pin_text == NULL || pin_text[0] == '\0')
    log_error("get_port", 0, "Pin text is NULL or empty.");
  
  // Check in which format the pin text is given
  if(pin_text[0] == 'P'){ // "PXn" format
    if(pin_text[1] >= 'A' && pin_text[1] <= 'Z')
      return pin_text[1];
    else
      log_error("get_port", 0, "Invalid pin format: '%s'", pin_text);
  } else{ // "GPIOn" format
    if(isdigit(pin_text[4]))
      return '\0'; // No port found e.g. for microcontrollers without ports
    else
      log_error("get_port", 0, "Unsupported pin format: '%s'", pin_text);
  } 
  
  log_error("get_port", 0, "Unsupported pin format: '%s'", pin_text);
  return '\0'; // This won't be reached due to log_error exiting
}

/**
 * @brief Get the pin number from a pin text in "PXn" format
 * 
 * @param pin_text pin text in format "PXn", X is port letter (A, B, C, etc.), n is pin number 
 * @return unsigned int pin number
 * 
 * @note expects pin text in format "PXn" (e.g. "PA5"), if other format is given it could result in incorrect behavior
 */
unsigned int get_pin_number_PXn(const char* pin_text){
  if(pin_text == NULL || pin_text[0] == '\0')
    log_error("get_pin_number_PXn", 0, "Pin text is NULL or empty.");
  
  int pin_number;
  // Check in which format the pin text is given
  if(pin_text[0] == 'P'){ // "PXn" format
    if(!isdigit(pin_text[2]))
      log_error("get_pin_number_PXn", 0, "Invalid pin format: '%s'", pin_text);
    
    pin_number = atoi(&pin_text[2]);
    return (unsigned int)pin_number;
  } 
  
  log_error("get_pin_number_PXn", 0, "Unsupported pin format: '%s'", pin_text);
  return 0; // This won't be reached due to log_error exiting
}

/**
 * @brief Get the pin number from a pin text in "GPIOn" format
 * 
 * @param pin_text pin text in format "GPIOn", n is pin number 
 * @return unsigned int pin number
 * 
 * @note expects pin text in format "GPIOn" (e.g. "GPIO5"), if other format is given it could result in incorrect behavior
 */
unsigned int get_pin_number_GPIOn(const char* pin_text){
  if(pin_text == NULL || pin_text[0] == '\0')
    log_error("get_pin_number_GPIOn", 0, "Pin text is NULL or empty.");
  
  int pin_number;
  // Check in which format the pin text is given
  if(pin_text[0] == 'G'){ // "GPIOn" format
    if(!isdigit(pin_text[4]))
      log_error("get_pin_number_GPIOn", 0, "Invalid pin format: '%s'", pin_text);
    pin_number = atoi(&pin_text[4]);
    return (unsigned int)pin_number;
  } 
  
  log_error("get_pin_number_GPIOn", 0, "Unsupported pin format: '%s'", pin_text);
  return 0; // This won't be reached due to log_error exiting
}
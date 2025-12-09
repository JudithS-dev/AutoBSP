#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> // allows variable number of arguments in function call

// Logging file pointer: static = only accessible from logging.c
static FILE *log_file = NULL;

/**
 * @brief Initializes the logging system by opening the specified log file.
 * 
 * @param file_path Path to the log file.
 * 
 * @note If the log file cannot be opened, an error message is printed to stderr and the program exits. 
 */
void init_logging(const char *file_path){
  if(log_file != NULL)
    fclose(log_file); // Close any previously opened log file
  
  log_file = fopen(file_path, "w");
  if(log_file == NULL){
    fprintf(stderr, "\nERROR 'init_logging': Could not open log file '%s'\n", file_path);
    exit(1);
  }
}

/**
 * @brief Closes the logging system by closing the log file if it is open.
 * 
 * @note If the log file is not open, this function does nothing.
 */
void close_logging(){
  if(log_file != NULL){
    fclose(log_file);
    log_file = NULL;
  }
}

/**
 * @brief Internal function to log an error message using a va_list of arguments.
 * 
 * @param function_name Name of the function where the error occurred.
 * @param line_nr Line number where the error occurred.
 * @param message Format string for the error message.
 * @param args va_list of additional arguments for the format string.
 * 
 * @note If the log file is not initialized, an error message is printed to stderr and the program exits.
 * @note If the function_name is an empty string, it is omitted from the error message.
 * @note If the line_nr is less than or equal to 0, it is omitted from the error message.
 */
static void log_error_v_internal(const char *function_name, int line_nr, const char *message, va_list args){
  if(log_file == NULL){
    fprintf(stderr, "\nERROR 'log_error': Logging file not initialized.\n");
    exit(1);
  }
  
  va_list args_copy;
  
  // Log to stderr
  va_copy(args_copy, args);
  fprintf(stderr, "\nERROR");
  if(strcmp(function_name, "") != 0)
    fprintf(stderr, " '%s'", function_name);
  if(line_nr > 0)
    fprintf(stderr, " Line %d", line_nr);
  fprintf(stderr, ": ");
  vfprintf(stderr, message, args_copy);
  fprintf(stderr, "\n");
  va_end(args_copy);

  // Log to log_file
  va_copy(args_copy, args);
  fprintf(log_file, "\nERROR");
  if(strcmp(function_name, "") != 0)
    fprintf(log_file, " '%s'", function_name);
  if(line_nr > 0)
    fprintf(log_file, " Line %d", line_nr);
  fprintf(log_file, ": ");
  vfprintf(log_file, message, args_copy);
  fprintf(log_file, "\n");
  fflush(log_file);
  va_end(args_copy);
}

/**
 * @brief Logs an error message to the log file and exits the program.
 * 
 * @param function_name Name of the function where the error occurred.
 * @param line_nr Line number where the error occurred.
 * @param message Format string for the error message.
 * @param ... Additional arguments for the format string.
 * 
 * @note If the log file is not initialized, an error message is printed to stderr and the program exits.
 * @note If the function_name is an empty string, it is omitted from the error message.
 * @note If the line_nr is less than or equal to 0, it is omitted from the error message.
 */
void log_error(const char *function_name, int line_nr, const char *message, ...){
  va_list args;
  va_start(args, message);
  // delegate to va_list-based internal implementation
  log_error_v_internal(function_name, line_nr, message, args);
  va_end(args);
  
  // Close the log file before exiting
  if(log_file != NULL){
    fclose(log_file);
    log_file = NULL;
  }
  exit(1);
}

/**
 * @brief Logs an error message to the log file without exiting the program.
 * 
 * @param function_name Name of the function where the error occurred.
 * @param line_nr Line number where the error occurred.
 * @param message Format string for the error message.
 * @param ... Additional arguments for the format string.
 * 
 * @note If the log file is not initialized, an error message is printed to stderr and the program exits.
 * @note If the function_name is an empty string, it is omitted from the error message.
 * @note If the line_nr is less than or equal to 0, it is omitted from the error message.
 */
void log_error_no_exit(const char *function_name, int line_nr, const char *message, ...){
  va_list args;
  va_start(args, message);
  log_error_v_internal(function_name, line_nr, message, args);
  va_end(args);
}

/**
 * @brief Logs an informational message to the log file if the specified log category is active.
 * 
 * @param function_name Name of the function where the log is generated.
 * @param log_category Category of the log message.
 * @param line_nr Line number where the log is generated.
 * @param message Format string for the log message.
 * @param ... Additional arguments for the format string.
 * 
 * @note If the log file is not initialized, an error message is printed to stderr and the program exits.
 * @note If the log category is not recognized, an error is logged using log_error.
 */
void log_info(const char *function_name, int log_category, int line_nr, const char *message, ...){
  if(log_file == NULL){
    fprintf(stderr, "\nERROR 'log_info': Logging file not initialized.\n");
    exit(1);
  }

  if((log_category) & (ACTIVE_LOGS)){
    const char *category_str =  (log_category == LOG_LEXER_CONDITION)  ? "LEX_CONDI" :
                                (log_category == LOG_LEXER_TOKEN)      ? "LEX_TOKEN" :
                                (log_category == LOG_OTHER)            ? "__OTHER__" :
                                "UNKNOWN";
    
    if(strcmp(category_str, "UNKNOWN") == 0)
      log_error("log_info", __LINE__, "Unknown log category: %d", log_category);
    
    int padding = 20 - (int)strlen(function_name); 
    if(padding < 0)
      padding = 0;
        
    va_list args;
    va_start(args, message);
    fprintf(log_file, "INFO [%-8s] '%s'%*s Line %3d: ", category_str, function_name, padding, " ", line_nr);
    vfprintf(log_file, message, args);
    fprintf(log_file, "\n");
    va_end(args);
  }
}

/**
 * @brief Writes a raw message to the log file (without any formatting).
 * 
 * @param message Format string for the log message.
 * @param ... Additional arguments for the format string.
 * 
 * @note If the log file is not initialized, an error message is printed to stderr and the program exits.
 */
void log_write(const char *message, ...){
  if(log_file == NULL){
    fprintf(stderr, "\nERROR 'log_info': Logging file not initialized.\n");
    exit(1);
  }
  
  va_list args;
  va_start(args, message);
  vfprintf(log_file, message, args);
  fprintf(log_file, "\n");
  fflush(log_file);
  va_end(args);
}

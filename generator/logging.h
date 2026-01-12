#ifndef __LOGGING_H__
#define __LOGGING_H__

// Logging categories
#define LOG_LEXER_CONDITION   1  // shows if lexer starts or leaves a comment
#define LOG_LEXER_TOKEN       2  // shows the token the lexer has found
#define LOG_LEXER_CONVERSION  4  // shows the conversion of a pin text to a pin or port
#define LOG_PARSER_FOUND      8 // shows when the parser has found a specific rule
#define LOG_OTHER            16  // shows all other information

// Active logs are specified in the logging.c file

void init_logging(const char *output_path);
void close_logging();

void log_error(const char *function_name, int line_nr, const char *message, ...);
void log_error_no_exit(const char *function_name, int line_nr, const char *message, ...);

void log_info(const char *function_name, int log_category, int line_nr, const char *message, ...);

void log_write(const char *message, ...);

#endif //__LOGGING_H__

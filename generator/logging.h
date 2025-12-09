#ifndef __LOGGING_H__
#define __LOGGING_H__

// Logging categories
#define LOG_LEXER_CONDITION  1  // shows if lexer starts or leaves a comment
#define LOG_LEXER_TOKEN      2  // shows the token the lexer has found
#define LOG_OTHER            4  // shows all other information

// Active logging categories (bitwise OR of the above)
#define ACTIVE_LOGS (LOG_LEXER_CONDITION | LOG_LEXER_TOKEN | LOG_OTHER)

void init_logging(const char *file_path);
void close_logging();

void log_error(const char *function_name, int line_nr, const char *message, ...);
void log_error_no_exit(const char *function_name, int line_nr, const char *message, ...);

void log_info(const char *function_name, int log_category, int line_nr, const char *message, ...);

void log_write(const char *message, ...);

#endif //__LOGGING_H__

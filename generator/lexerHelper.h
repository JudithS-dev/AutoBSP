#ifndef __LEXER_HELPER_H__
#define __LEXER_HELPER_H__

char get_port(const char* pin_text);
unsigned int get_pin_number_PXn(const char* pin_text);
unsigned int get_pin_number_GPIOn(const char* pin_text);

#endif // __LEXER_HELPER_H__
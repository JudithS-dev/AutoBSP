#ifndef __AST_BUILD_H__
#define __AST_BUILD_H__

#include "astEnums.h"

// Constructors of builder
ast_dsl_builder_t* ast_new_dsl_builder();
ast_module_builder_t* ast_new_module_builder(int line_nr);

// DSL node functions
ast_dsl_node_t *ast_convert_dsl_builder_to_dsl_node(ast_dsl_builder_t* dsl_builder);
void ast_free_dsl_node(ast_dsl_node_t* dsl_node);

// DSL builder functions
void ast_dsl_builder_set_controller(int line_nr, ast_dsl_builder_t* dsl_builder, controller_t controller);
void ast_dsl_builder_append_module_builder( int line_nr, ast_dsl_builder_t* dsl_builder, ast_module_builder_t* module_builder);

// Module builder common setters
void ast_module_builder_set_name(  int line_nr, ast_module_builder_t* module_builder, const char* name);
void ast_module_builder_set_pin(   int line_nr, ast_module_builder_t* module_builder, pin_t pin);
void ast_module_builder_set_enable(int line_nr, ast_module_builder_t* module_builder, bool enable);
void ast_module_builder_set_kind(  int line_nr, ast_module_builder_t* module_builder, ast_module_kind_t kind);

// Module builder output specific setters
void ast_module_builder_set_output_type(        int line_nr, ast_module_builder_t* module_builder, gpio_type_t type);
void ast_module_builder_set_output_pull(        int line_nr, ast_module_builder_t* module_builder, gpio_pull_t pull);
void ast_module_builder_set_output_speed(       int line_nr, ast_module_builder_t* module_builder, gpio_speed_t speed);
void ast_module_builder_set_output_init(        int line_nr, ast_module_builder_t* module_builder, gpio_init_t init);
void ast_module_builder_set_output_active_level(int line_nr, ast_module_builder_t* module_builder, level_t level);

// Module builder input specific setters
void ast_module_builder_set_input_pull(        int line_nr, ast_module_builder_t* module_builder, gpio_pull_t pull);
void ast_module_builder_set_input_active_level(int line_nr, ast_module_builder_t* module_builder, level_t level);

// Module builder PWM specific setters
void ast_module_builder_set_pwm_pull(        int line_nr, ast_module_builder_t* module_builder, gpio_pull_t pull);
void ast_module_builder_set_pwm_speed(       int line_nr, ast_module_builder_t* module_builder, gpio_speed_t speed);
void ast_module_builder_set_pwm_active_level(int line_nr, ast_module_builder_t* module_builder, level_t level);
void ast_module_builder_set_pwm_frequency(   int line_nr, ast_module_builder_t* module_builder, uint32_t frequency);
void ast_module_builder_set_pwm_duty(        int line_nr, ast_module_builder_t* module_builder, uint32_t duty_cycle);

// Module builder UART specific setters
void ast_module_builder_set_uart_tx_pin(  int line_nr, ast_module_builder_t* module_builder, pin_t tx_pin);
void ast_module_builder_set_uart_rx_pin(  int line_nr, ast_module_builder_t* module_builder, pin_t rx_pin);
void ast_module_builder_set_uart_baudrate(int line_nr, ast_module_builder_t* module_builder, uint32_t baudrate);
void ast_module_builder_set_uart_databits(int line_nr, ast_module_builder_t* module_builder, uint32_t databits);
void ast_module_builder_set_uart_stopbits(int line_nr, ast_module_builder_t* module_builder, uint32_t stopbits);
void ast_module_builder_set_uart_parity(  int line_nr, ast_module_builder_t* module_builder, uart_parity_t parity);

#endif //__AST_BUILD_H__
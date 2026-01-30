idf_component_register(SRCS "main.c" "generated_bsp.c"
                       PRIV_REQUIRES esp_driver_gpio esp_driver_ledc
                       INCLUDE_DIRS "")

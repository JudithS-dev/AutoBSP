#ifndef __ESP32_H__
#define __ESP32_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Data structure representing a ESP32 microcontroller pin.
 */
typedef struct{
  uint8_t num;       // 0...35
} pin_key_t;

/**
 * @brief Data structure representing the capabilities of a pin on the ESP32.
 */
typedef struct{
  pin_key_t pin;
  
  bool can_gpio_in;
  bool can_gpio_out;
  
  uint8_t cost;      // 0=best, higher=try to avoid (e.g. Strapping pin)
  bool not_usable;   // If pin is not usable at all (e.g. GPIO6-GPIO11 used for flash memory)
} pin_cap_t;

/**
 * @brief Array of pin capabilities for the ESP32 microcontroller.
 * 
 * Each entry defines the capabilities of a specific pin.
 */
static const pin_cap_t PINCAP_ESP32[] = {
  /* GPIO0: // input works, output works, pwm works
   */
  { .pin={0},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO1: // input works, output works, pwm works
   */
  { .pin={1},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO2: // input works, output works, pwm works
   */
  { .pin={2},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO3: // input works, output works, pwm works
   */
  { .pin={3},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO4: // input works, output works, pwm works
   */
  { .pin={4},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO5: // input works, output works, pwm works
   */
  { .pin={5},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO6:
   * Not usable (used for flash memory)
   */
  { .pin={6},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO7:
   * Not usable (used for flash memory)
   */
  { .pin={7},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO8:
   * Not usable (used for flash memory)
   */
  { .pin={8},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO9:
   * Not usable (used for flash memory)
   */
  { .pin={9},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO10:
   * Not usable (used for flash memory)
   */
  { .pin={10},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO11:
   * Not usable (used for flash memory)
   */
  { .pin={11},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO12: // input works, output works, pwm works
   */
  { .pin={12},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO13: // input works, output works, pwm works
   */
  { .pin={13},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO14: // input works, output works, pwm works
   */
  { .pin={14},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO15: // input works, output works, pwm works
   */
  { .pin={15},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO16: // input works, output works, pwm works
   */
  { .pin={16},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO17: // input works, output works, pwm works
   */
  { .pin={17},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO18: // input works, output works, pwm works
   */
  { .pin={18},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO19: // input works, output works, pwm works
   */
  { .pin={19},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO20:
   * (does not exist)
   */
  { .pin={20},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO21: // input works, output works, pwm works
   */
  { .pin={21},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO22: // input works, output works, pwm works
   */
  { .pin={22},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO23: // input works, output works, pwm works
   */
  { .pin={23},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO24:
   * (does not exist)
   */
  { .pin={24},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO25: // input works, output works, pwm works
   */
  { .pin={25},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO26: // input works, output works, pwm works
   */
  { .pin={26},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO27: // input works, output works, pwm works
   */
  { .pin={27},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO28:
   * (does not exist)
   */
  { .pin={28},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO29:
   * (does not exist)
   */
  { .pin={29},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO30:
   * (does not exist)
   */
  { .pin={30},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO31:
   * (does not exist)
   */
  { .pin={31},
    .can_gpio_in=false, .can_gpio_out=false,
    .cost=255, .not_usable=true
  },
  
  /* GPIO32: // input works, output works, pwm works
   */
  { .pin={32},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO33: // input works, output works, pwm works
   */
  { .pin={33},
    .can_gpio_in=true, .can_gpio_out=true,
    .cost=0, .not_usable=false
  },
  
  /* GPIO34: // input works
   * Input only
   */
  { .pin={34},
    .can_gpio_in=true, .can_gpio_out=false,
    .cost=0, .not_usable=false
  },
  
  /* GPIO35: // input works
   * Input only
   */
  { .pin={35},
    .can_gpio_in=true, .can_gpio_out=false,
    .cost=0, .not_usable=false
  }
};

/**
 * @brief Length of the PINCAP_ESP32 array.
 */
static const uint16_t PINCAP_ESP32_LEN = sizeof(PINCAP_ESP32) / sizeof(PINCAP_ESP32[0]);

#endif // __ESP32_H__
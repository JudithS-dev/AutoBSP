// sudo ls /dev/ttyUSB*
// picocom -b 115200 -d 8 -y n -p 1 /dev/ttyUSB0 --echo
// -b <baudrate>: Baud rate (e.g. 9600, 115200)
// -d <data bits>: Number of data bits on the PC side
//                 (STM32: parity is part of the word length, so
//                  databits=9 with parity -> 8 data bits in picocom)
// -y <parity>: Parity (n = none, o = odd, e = even)
// -p <stop bits>: Number of stop bits (1 or 2)
// --echo: Echo back received characters
// Quit: ctrl + A then ctrl + X


AutoBSP{
  controller: STM32F446RE
  
  OUTPUT{
    name:    "LED_RED"
    pin:     PA5          // PA5 || PA6
    
    type:    pushpull
    pull:    none
    speed:   low
    init:    off
    active:  high
    
    enable:  true
  }
  
  OUTPUT{
    name:    "LED_GREEN"
    pin:     PC8
    enable:  false
  }
  
  INPUT{
    name:    "BUTTON_EXTERNAL"
    pin:     PC0              // PC0 || PC1

    pull:    up
    active:  low
    
    enable:  true
  }

  INPUT{
    name:    "BUTTON_BOARD"
    pin:     PC13
    
    pull:    none
    active:  low
    
    enable:  false
  }
  
  PWM_OUTPUT{
    name:       "PWM_SIGNAL_1"
    pin:        PC8              // PC8 || PC6
    
    pull:       none
    speed:      high
    frequency:  2000
    duty:       500
    active:     high
    
    enable:     true
  }
  
  PWM_OUTPUT{
    name:       "PWM_SIGNAL_2"
    pin:        PA1
    enable:     false
  }
  
  UART{ // picocom -b 115200 -d 8 -y n -p 1 /dev/ttyUSB0 --echo
    name:     "UART_USED"
    tx_pin:    PA0         // PA0 || PC10
    rx_pin:    PA1         // PA1 || PC11
    
    baudrate:  115200
    databits:  8
    stopbits:  1
    parity:    none
    
    enable:    false
  }
  
  UART{ // picocom -b 9600 -d 8 -y o -p 2 /dev/ttyUSB0 --echo
    name:     "UART_USED"
    tx_pin:    PA0         // PA0 || PC10
    rx_pin:    PA1         // PA1 || PC11
    
    baudrate:  9600
    databits:  9
    stopbits:  2
    parity:    odd
    
    enable:    true
  }
  
  UART{
    name:     "UART_OTHER"
    tx_pin:    PB10
    rx_pin:    PB11
    enable:    false
  }
}

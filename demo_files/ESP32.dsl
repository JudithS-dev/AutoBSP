// source ~/esp/esp-idf/export.sh
// idf.py build
// idf.py -p /dev/ttyUSB0 -b 115200 flash
// idf.py monitor
// Quit: Ctrl + ]

// sudo ls /dev/ttyUSB*
// picocom -b 115200 -d 8 -y n -p 1 /dev/ttyUSB0 --echo
// -b <baudrate>: Baud rate (e.g. 9600, 115200)
// -d <data bits>: Number of data bits on the PC side
//                 (ESP3232: <data bits> = databits of DSL)
// -y <parity>: Parity (n = none, o = odd, e = even)
// -p <stop bits>: Number of stop bits (1 or 2)
// --echo: Echo back received characters
// Quit: ctrl + A then ctrl + X

AutoBSP{
  controller: ESP32
  
  OUTPUT{ // no speed parameter supported
    name:    "LED_RED"
    pin:     GPIO0          // GPIO0 || GPIO2
    
    type:    pushpull
    pull:    none
    init:    off
    active:  high
    
    enable:  true
  }
  
  OUTPUT{
    name:    "LED_GREEN"
    pin:     GPIO26
    enable:  false
  }
  
  INPUT{
    name:    "BUTTON_EXTERNAL"
    pin:     GPIO18          // GPIO18 || GPIO19
    
    pull:    up
    active:  low
    
    enable:  true
  }
  
  INPUT{
    name:    "BUTTON_OTHER"
    pin:     GPIO34
    enable:  false
  }
  
  PWM_OUTPUT{  // no speed or pull parameter supported
    name:       "PWM_SIGNAL_1"
    pin:        GPIO22              // GPIO22 || GPIO23
    
    frequency:  2000
    duty:       500
    active:     high
    
    enable:     true
  }
  
  PWM_OUTPUT{
    name:       "PWM_SIGNAL_2"
    pin:        GPIO3
    enable:     false
  }
  
  UART{ // picocom -b 115200 -d 8 -y n -p 1 /dev/ttyUSB0 --echo
    name:     "UART_USED"
    tx_pin:    GPIO26         // GPIO26 || GPIO27
    rx_pin:    GPIO32         // GPIO32 || GPIO33
    
    baudrate:  115200
    databits:  8
    stopbits:  1
    parity:    none
    
    enable:    false
  }
  
  UART{ // picocom -b 9600 -d 7 -y o -p 2 /dev/ttyUSB0 --echo
    name:     "UART_USED"
    tx_pin:    GPIO26         // GPIO26 || GPIO27
    rx_pin:    GPIO32         // GPIO32 || GPIO33
    
    baudrate:  9600
    databits:  7
    stopbits:  2
    parity:    odd
    
    enable:    true
  }
  
  UART{
    name:     "UART_OTHER"
    tx_pin:    GPIO25
    rx_pin:    GPIO26
    enable:    false
  }
}

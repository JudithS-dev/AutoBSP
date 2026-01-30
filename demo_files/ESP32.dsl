// source ~/esp/esp-idf/export.sh
// idf.py build
// idf.py -p /dev/ttyUSB0 -b 115200 flash
// idf.py monitor
// Quit: Ctrl + ]

AutoBSP{
  controller: ESP32
  
  OUTPUT{ // no speed parameter supported
    name:    "LED_RED"
    pin:     GPIO12          // PA5 || PA6 TODO fix alternative Pins
    
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
    name:    "BUTTON_BOARD"
    pin:     GPIO33
    
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
    pin:        GPIO5              // PC8 || PC6 TODO fix alternative Pins
    
    frequency:  2000
    duty:       500
    active:     high
    
    enable:     true
  }
  
  PWM_OUTPUT{
    name:       "PWM_SIGNAL_2"
    pin:        GPIO1
    enable:     false
  }
  
  /*UART{ // picocom -b 115200 -d 8 -y n -p 1 /dev/ttyUSB0 --echo
    name:     "UART_USED"
    tx_pin:    GPIO0         // PA0 || PC10 TODO fix alternative Pins
    rx_pin:    GPIO1         // PA1 || PC11 TODO fix alternative Pins
    
    baudrate:  115200
    databits:  8
    stopbits:  1
    parity:    none
    
    enable:    false
  }
  
  UART{ // picocom -b 9600 -d 8 -y o -p 2 /dev/ttyUSB0 --echo
    name:     "UART_USED"
    tx_pin:    GPIO0         // PA0 || PC10 TODO fix alternative Pins
    rx_pin:    GPIO1         // PA1 || PC11 TODO fix alternative Pins
    
    baudrate:  9600
    databits:  9
    stopbits:  2
    parity:    odd
    
    enable:    true
  }
  
  UART{
    name:     "UART_OTHER"
    tx_pin:    GPIO10
    rx_pin:    GPIO11
    enable:    false
  }*/
}

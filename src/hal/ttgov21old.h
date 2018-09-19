/*  Hardware related definitions for TTGO V2.1 Board
// ATTENTION: check your board version!
// This settings are for boards without label on pcb, or labeled v1.5 on pcb
*/

#define HAS_LORA 1       // comment out if device shall not send data via LoRa
#define HAS_SPI 1        // comment out if device shall not send data via SPI
#define CFG_sx1276_radio 1 // HPD13A LoRa SoC
#define HAS_LED NOT_A_PIN // no usable LED on board

#define HAS_DISPLAY U8X8_SSD1306_128X64_NONAME_HW_I2C
#define DISPLAY_FLIP  1 // rotated display
#define HAS_BATTERY_PROBE ADC1_GPIO35_CHANNEL // uses GPIO7
#define BATT_FACTOR 2 // voltage divider 100k/100k on board

// re-define pin definitions of pins_arduino.h
#define PIN_LORA_SPI_SS    GPIO_NUM_18 // ESP32 GPIO18 (Pin18) -- HPD13A NSS/SEL (Pin4) SPI Chip Select Input
#define PIN_LORA_SPI_MOSI  GPIO_NUM_27 // ESP32 GPIO27 (Pin27) -- HPD13A MOSI/DSI (Pin6) SPI Data Input
#define PIN_LORA_SPI_MISO  GPIO_NUM_19 // ESP32 GPIO19 (Pin19) -- HPD13A MISO/DSO (Pin7) SPI Data Output
#define PIN_LORA_SPI_SCK   GPIO_NUM_5  // ESP32 GPIO5 (Pin5)   -- HPD13A SCK (Pin5) SPI Clock Input

// non arduino pin definitions
#define RST   LMIC_UNUSED_PIN   // connected to ESP32 RST/EN (old board)
//#define RST   GPIO_NUM_12     // (boards labeled v1.5)
#define DIO0  GPIO_NUM_26 // ESP32 GPIO26 <-> HPD13A IO0
#define DIO1  GPIO_NUM_33 // ESP32 GPIO33 <-> HPDIO1 <-> HPD13A IO1
#define DIO2  GPIO_NUM_32 // ESP32 GPIO32 <-> HPDIO2 <-> HPD13A IO2

// Hardware pin definitions for TTGO V2 Board with OLED SSD1306 0,96" I2C Display
#define OLED_RST U8X8_PIN_NONE // connected to CPU RST/EN
#define OLED_SDA GPIO_NUM_21 // ESP32 GPIO21 -- SD1306 D1+D2
#define OLED_SCL GPIO_NUM_22 // ESP32 GPIO22 -- SD1306 D0

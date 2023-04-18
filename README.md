# arduino
Arduino + NodeMCU ESP8266 via SPI protocol communications

- `mega_test_spi`               - Arduino MEGA 2560  as **SPI slave** <br />
- `nodemcu-esp8266_master_spi`  - NodeMCU ESP8266    as **SPI master**


## List of pinout for SPI protocol

||    NodeMCU     ||    Arduino MEGA    ||
- ||    SCLK - D5   ||    52 - SCLK       ||
- ||    MISO - D6   ||    50 - MISO       ||
- ||    MOSI - D7   ||    51 - MOSI       ||
- ||    CS - D8     ||    53 - SS         ||

## List of alternative device uses following the Arduino MEGA 2560 pinouts

### LCD 16x2
- VSS - GND
- VDD - 3.3/5V
- V0  - GND
- RS  - A0
- RW  - GND
- E   - A1
- _D0-D3 - Empty_
- D4  - A5
- D5  - A4
- D6  - A3
- D7  - A2
- A   - 12 (controlling the back LED)
- K   - GND

`LiquidCrystal LCD(A0, A1, A5, A4, A3, A2);`

### Keypad
22 -> 36

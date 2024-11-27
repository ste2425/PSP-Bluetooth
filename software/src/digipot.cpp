#include "digipot.h"

SPIClass spi = SPIClass(HSPI);
MCP_POT pot(15, 33, 25, &spi);

uint8_t PSP_map_controller(int controllerVal) {  
  /*
   * Controllers report range of -511 - 512
   * bring to 0 - 1023 then map to Digital pots acceptable range.
   * Digi pot accepts 0 - 257 however
   * for some reason PSP reports full range from a value of 50 - 200.
   */
   
  return map(controllerVal + 511, 0, 1023, 200, 50);
}

void write(int address, int value) {
    pot.setValue(address, value);
}

void DIGIPOT_write_x(int value) {
  write(0, PSP_map_controller(value));
}

void DIGIPOT_write_y(int value) {
  write(1, PSP_map_controller(value));  
}

void DIGIPOT_setup() {  
  spi.begin(14, 12, 13, 15);
  pot.begin();
}

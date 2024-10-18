#include <stdio.h>
#include "pico/stdlib.h"

#define LOWER_THRESHOLD 1
#define UPPER_THRESHOLD 10000

void read_water_level ()
{
  uint32_t data = 0;
  while (1) {
    data = adc_read();
    if (multicore_fifo_wready()) {
      printf("Adding data to FIFO\n");
      multicore_fifo_push_blocking(data);
    } else {
      printf("FIFO not ready for data...\n");
    }
    // sleep for one minute in betweeen checks
    sleep_ms(60000);
  }
}

int main ()
{
  stdio_init_all();

  adc_init();
  adc_gpio_init(26); //26-29 are valid inputs for adc

  uint32_t data = 0;
  multicore_launch_core1(read_water_level);

  while (1) {
    data = multicore_fifo_pop_blocking();
    if (data > UPPER_THRESHOLD) {
      //DO THIS
    } else if (data < LOWER_THRESHOLD) {
      //DO THAT
    }

  }

  return 0;
}

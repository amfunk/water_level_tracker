#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"

#define LOWER_THRESHOLD 1
#define UPPER_THRESHOLD 10000

#define ADC_PIN 26
#define VALVE_PIN 1

bool is_filling = false;

void read_water_level ()
{
  uint32_t data = 0;
  while (1) {
    data = adc_read();
    if (multicore_fifo_wready()) {
      printf("Adding data to FIFO: %u\n",data);
      multicore_fifo_push_blocking(data);
    } else {
      printf("FIFO not ready for data...\n");
    }

    if (is_filling) {
      // Check water level every second until stop filling
      sleep_ms(1000);

    } else if (!is_filling) {
      // Not currently filling reservoir
      sleep_ms(60000);
      // sleep for one minute in betweeen checks
    }
  }
}

void open_valve ()
{
  // TODO actually open valve
  printf("OPENING VALVE\n");



  is_filling = true;

  return;
}

void close_valve ()
{
  // TODO actually close valve
  printf("CLOSING VALVE\n");

  is_filling = false;

  return;
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
    printf("Reading data from FIFO: %u\n",data);
    if (data < LOWER_THRESHOLD) {
      // If water level is too low, open valve
      open_valve();
      
    } else if (data > UPPER_THRESHOLD) {
      // Water level has refilled sufficiently
      close_valve();
    }
  }

  return 0;
}
